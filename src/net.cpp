/***
 *  Copyright (C) 2004-2009 Francisco Bischoff
 *  Copyright (C) 2006 MaxMind LLC
 *  Copyright (C) 2000-2003 MySQL AB
 *
 *  This file is part of Tabajara Host Server.
 *
 *  Tabajara Host Server is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tabajara Host Server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Tabajara Host Server.  If not, see <http://www.gnu.org/licenses/agpl.html>.
 *
 ***/

/**
  * @file		net.c
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		network stuff
  */

#include "client.h"
#include "common.h"
#include "game.h"
#include "net.h"
#include "wbserver.h"

/**
 InitTCPNet

 Initializes all net stuffs and return a server-side socket
 */

int InitTCPNet(int portno)
{
	int isockfd;
	u_long ioctlv;
	struct sockaddr_in serv_addr;

	ioctlv = 1;

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(0x0202, &wsaData))
	{
		Com_Printf(VERBOSE_ERROR, "initializing winsock\n");
		ExitServer(1);
	}
#endif

	if((isockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Com_Printf(VERBOSE_ERROR, "opening TCP socket\n");
		ExitServer(1);
	}

#ifdef _WIN32
	if (ioctlsocket(isockfd, FIONBIO, &ioctlv) == -1)
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "setting nonblocking TCP socket (isockfd)\n");
		ExitServer(1);
	}
#else
	if(ioctl(isockfd, FIONBIO, &ioctlv) == -1)
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "setting nonblocking TCP socket (isockfd)\n");
		ExitServer(1);
	}
#endif

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	memset((char *) &(serv_addr.sin_zero), 0, 8);

	if(bind(isockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "binding TCP socket\n");
		ExitServer(1);
	}

	if(listen(isockfd, maxclients->value))
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "listen()\n");
		ExitServer(1);
	}

	return isockfd;
}

/**
 InitUDPNet

 bind UDP listen socket and return socket descriptor
 */

int InitUDPNet(int portno)
{
	int isockfd;
	u_long ioctlv;
	struct sockaddr_in serv_addr;

	ioctlv = 1;

	if((isockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		Com_Printf(VERBOSE_ERROR, "opening UDP socket\n");
		ExitServer(1);
	}

#ifdef _WIN32
	if (ioctlsocket(isockfd, FIONBIO, &ioctlv) == -1)
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "setting nonblocking UDP socket (isockfd)\n");
		ExitServer(1);
	}
#else
	if(ioctl(isockfd, FIONBIO, &ioctlv) == -1)
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "setting nonblocking UDP socket (isockfd)\n");
		ExitServer(1);
	}
#endif

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	memset((char *) &(serv_addr.sin_zero), 0, 8);

	if(bind(isockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		Com_Close(&isockfd);
		Com_Printf(VERBOSE_ERROR, "binding UDP socket\n");
		ExitServer(1);
	}

	memset(arenaslist, 0, sizeof(arenaslist_t) * MAX_ARENASLIST); // clean up arenaslist

	return isockfd;
}

/**
 SendPacketTHL

 Gets a pre-made packet, encrypt, checksum, and send to THL
 */

int32_t SendPacketTHL(u_int8_t *buffer, u_int16_t len, client_t *client)
{
	// int i;
	u_int8_t datagram[MAX_SENDDATA];
	datagram_t *packet;
	u_int16_t header;

	memset(datagram, 0, MAX_SENDDATA);

	if(buffer)
	{
		if((buffer[0] == 0xff) && (buffer[1] == 0xff))
		{
			Com_Printf(VERBOSE_WARNING, "SendPacketTHL() header 0xffff, Packet not sent\n");
			Com_Printfhex(datagram, len + 4);
			return 0;
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "SendPacketTHL() buffer points to NULL\n");
		return 0;
	}

	if((len + 4) <= MAX_SENDDATA)
	{
		packet = (datagram_t *) (datagram + 1);

		datagram[0] = 0x10; // THL

		header = (u_int16_t) (datagram[3] << 2) + datagram[4];

		arena->bufferit = 1;

		packet->size = htons(len);

		memcpy(&(packet->data), buffer, len);

		datagram[len + 3] = CheckSum(&(packet->data), len); // checksum

		if(debug->value)
		{
			Com_Printf(VERBOSE_DEBUG, "--->>> ");
			Com_Printfhex(datagram, len + 4);
		}

		wbcrypt(&(packet->data), client->key, len, FALSE); // encrypting

		if(client->login || (client->inuse && client->socket))
		{
			return Com_Send(client, datagram, (int) len + 4);
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "client not in use or without socket\n");
			Com_Printf(VERBOSE_ALWAYS, "%s client not in use or without socket\n", client->longnick);
			RemoveClient(client);
			return -1;
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "SendPacketTHL() overflowed (packet to %s - %s)\n", client->longnick, client->ip);
		RemoveClient(client);
		return -1;
	}
}

/**
 SendPacket

 Gets a pre-made packet, encrypt, checksum, and send to client
 */

int32_t SendPacket(u_int8_t *buffer, u_int16_t len, client_t *client)
{
	//int i;
	u_int8_t datagram[MAX_SENDDATA];
	datagram_t *packet;
	u_int16_t header;

	if(!client)
	{
		Com_Printf(VERBOSE_WARNING, "SendPacket() client not exist\n");
		return -1;
	}

	if(!client->inuse)
	{
		Com_Printf(VERBOSE_WARNING, "SendPacket() client not in use\n");
		return -1;
	}

	if(!client->socket)
	{
		Com_Printf(VERBOSE_WARNING, "SendPacket() client without socket\n");
		return -1;
	}

	if(!buffer)
	{
		Com_Printf(VERBOSE_WARNING, "SendPacket() client without buffer\n");
		return -1;
	}

	memset(datagram, 0, MAX_SENDDATA);

	if(buffer)
	{
		if((buffer[0] == 0xff) && (buffer[1] == 0xff))
		{
			Com_Printf(VERBOSE_WARNING, "SendPacket() header 0xffff, Packet not sent\n");
			Com_Printfhex(datagram, len + 4);
			return 0;
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "SendPacket() buffer points to NULL\n");
		return 0;
	}

	if((len + 4) <= MAX_SENDDATA)
	{
		packet = (datagram_t *) (datagram + 1);

		datagram[0] = 0x09; // wb3

		header = (u_int16_t) (datagram[3] << 2) + datagram[4];

		Com_WBntoh(&header);

		switch(header)
		{
			case 0x0301:
			case 0x1D04:
			case 0x2101:
			case 0x1900:
			case 0x1907:
			case 0x1909:
			case 0x1917:
			case 0x0007:
			case 0x001E:
				arena->bufferit = 0;
				break;
			default:
				arena->bufferit = 1;
		}

		packet->size = htons(len);

		memcpy(&(packet->data), buffer, len);

		datagram[len + 3] = CheckSum(&(packet->data), len); // checksum

		if(debug->value)
		{
			Com_Printf(VERBOSE_DEBUG, "--->>> ");
			Com_Printfhex(datagram, len + 4);
		}

		if(!client->login)
		{
			if(client->loginkey)
				wbcrypt(&(packet->data), client->key, len, FALSE); // encrypting
			else
				wbcrypt(&(packet->data), client->key, len, TRUE); // encrypting
		}

		//		if (client->login || (client->inuse && client->socket))
		//		{
		return Com_Send(client, datagram, (int) len + 4);
		//		}
		//		else
		//		{
		//			Com_Printf(VERBOSE_WARNING, "client not in use or without socket\n");
		//			Com_Printf(VERBOSE_ALWAYS, "%s client not in use or without socket\n", client->longnick);
		//			RemoveClient(client);
		//			return -1;
		//		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "SendPacket() overflowed (packet to %s - %s len %u)\n", client->longnick, client->ip, len);
		Com_Printfhex(buffer, MAX_SENDDATA);
		return 0;
		//RemoveClient(client); // FIXME: temporary fix to catch a bug
		//return -1;
	}
}

/**
 GetPacket

 Receive last wbpacket at socket buffer and process it
 */

int GetPacket(client_t *client)
{
	u_int16_t len, recvlen;
	int32_t n, m;

	if(!client || !client->inuse)
		return 0;

	memset(mainbuffer, 0, MAX_RECVDATA);

	n = Com_Recv(client->socket, mainbuffer, 3);

	if(n <= 0)
	{
		return n;
	}
	else
	{
		ConnStatistics(client, n, 1 /*recv*/);
	}

	if((mainbuffer[0] == 0x09) /*wb3*/|| (mainbuffer[0] == 0x10) /*THL*/)
	{
		client->timeout = 0;

		len = 0;
		if(mainbuffer[1] > 0) // check===> len = (buffer[1]*0x100) | buffer[2];
		{
			len = mainbuffer[1] * 0x100;
		}
		len += mainbuffer[2] + 1; // +1 to recv checksum

		client->packetlen = len - 1;

		if(len > MAX_RECVDATA)
		{
			Com_Printf(VERBOSE_WARNING, "%s(%s) GetPacket(): mainbuffer oversized, packet skipped\n", client->longnick, client->ip);
			FlushSocket(client->socket);
			return 0;
		}
		else
		{
			recvlen = len;
		}

		if((n = Com_Recv(client->socket, mainbuffer, recvlen)) > 0)
		{
			if(n == len) //
			{ //
				if(!setjmp(debug_buffer))
				{
					m = ProcessPacket(mainbuffer, len - 1, client); // len-1 to remove checksum from buffer

					if(m < 0) // invalid checksum
					{
						Com_Printf(VERBOSE_WARNING, "%s(%s) - Invalid Packet Checksum\n", client->longnick, client->ip);
						FlushSocket(client->socket);
						return 0;
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
					return 0;
				}
			} //

			return n;
		}
		else
			return n;
	}

	Com_Printf(VERBOSE_WARNING, "%s(%s) %d - Invalid Packet\n", client->longnick, client->ip, n);
	FlushSocket(client->socket);
	return 0;
}

/**
 ConnStatistics

 Record client connection flow statistics
 */

void ConnStatistics(client_t *client, u_int32_t len, u_int8_t type)
{
	u_int16_t i;

	if(client)
	{
		if(type == 0) // send
		{
			client->sendcount[0][1] += len;
		}
		else if(type == 1) // recv
		{
			client->recvcount[0][1] += len;
		}
	}
	else
	{
		switch(type)
		{
			case 1 /*second*/:
				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && !clients[i].drone)
					{
						clients[i].sendcount[0][0] = clients[i].sendcount[0][1];
						clients[i].sendcount[0][1] = 0;
						clients[i].sendcount[1][1] += clients[i].sendcount[0][0];

						clients[i].recvcount[0][0] = clients[i].recvcount[0][1];
						clients[i].recvcount[0][1] = 0;
						clients[i].recvcount[1][1] += clients[i].recvcount[0][0];
					}
				}
				break;
			case 5 /*5 seconds*/:
				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && !clients[i].drone)
					{
						clients[i].sendcount[1][0] = clients[i].sendcount[1][1];
						clients[i].sendcount[1][1] = 0;
						clients[i].sendcount[2][1] += clients[i].sendcount[1][0];

						clients[i].recvcount[1][0] = clients[i].recvcount[1][1];
						clients[i].recvcount[1][1] = 0;
						clients[i].recvcount[2][1] += clients[i].recvcount[1][0];
					}
				}
				break;
			case 10 /*10 second*/:
				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && !clients[i].drone)
					{
						clients[i].sendcount[2][0] = clients[i].sendcount[2][1];
						clients[i].sendcount[2][1] = 0;
						clients[i].sendcount[3][1] += clients[i].sendcount[2][0];

						clients[i].recvcount[2][0] = clients[i].recvcount[2][1];
						clients[i].recvcount[2][1] = 0;
						clients[i].recvcount[3][1] += clients[i].recvcount[2][0];
					}
				}
				break;
			case 30 /*30 second*/:
				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && !clients[i].drone)
					{
						clients[i].sendcount[3][0] = clients[i].sendcount[3][1];
						clients[i].sendcount[3][1] = 0;
						clients[i].sendcount[4][1] += clients[i].sendcount[3][0];

						clients[i].recvcount[3][0] = clients[i].recvcount[3][1];
						clients[i].recvcount[3][1] = 0;
						clients[i].recvcount[4][1] += clients[i].recvcount[3][0];
					}
				}
				break;
			case 60 /*60 second*/:
				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && !clients[i].drone)
					{
						clients[i].sendcount[4][0] = clients[i].sendcount[4][1];
						clients[i].sendcount[4][1] = 0;

						clients[i].recvcount[4][0] = clients[i].recvcount[4][1];
						clients[i].recvcount[4][1] = 0;
					}
				}
				break;
		}
	}
}

/**
 FlushSocket

 Flush the client socket
 */

void FlushSocket(int sockfd)
{
	int n;

	if(!sockfd)
	{
		Com_Printf(VERBOSE_WARNING, "FlushSocket() tried to flush socket ZERO\n");
		return;
	}

	memset(mainbuffer, 0, MAX_RECVDATA);

#ifdef _WIN32
	//ConnError(WSAGetLastError());
#else
	//ConnError(errno);
#endif

	while((n = Com_Recv(sockfd, mainbuffer, MAX_RECVDATA)) > 0)
	{
		//		if(debug->value)
		//		{
		Com_Printf(VERBOSE_WARNING, "Flushing socket (%d) n(%d):\n", socket, n);
		Com_Printfhex(mainbuffer, n);
		//		}
		//		else
		//			;
	}
}

/**
 ProtocolError

 Send back to client the protocol error message code
 */

void ProtocolError(client_t *client)
{
	u_int8_t buffer[2];

	buffer[0] = 0xC0;
	buffer[1] = 0x04;

	Com_Printf(VERBOSE_WARNING, "%s(%s) Invalid login protocol (Code %d)\n", client->longnick, client->ip, client->login);

	Com_Send(client, buffer, 2);
}
