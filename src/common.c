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

#include "shared.h"

FILE *logfile[MAX_LOGFILE]; // extern

/**
 Z_Malloc

 Allocates and returns a Zeroed memory block
 */

void *Z_Malloc(u_int32_t size)
{
	void *z;

	z = malloc(size);
	if (!z)
	{
		Com_Printf(VERBOSE_ERROR, "Z_Malloc()\n");
		ExitServer(1);
	}
	memset(z, 0, size);

	return z;
}

/**
 ascii2wbnick

 Converts 6bytes ascii string to 4bytes wbnick
 */

u_int32_t ascii2wbnick(char *playernick, u_int8_t attr)
{
	nick_t wbnick;
	u_int8_t buffer[7];
	u_int8_t n;

	if (!playernick)
		return 0;

	for (n=0; n<6; n++)
	{
		if (isalpha(playernick[n]))
		{
			buffer[n] = tolower(playernick[n]) - 'a';
		}
		else
			switch (playernick[n])
			{
				case '-':
					buffer[n] = 26;
					break;
				case '+':
					buffer[n] = 27;
					break;
				case '=':
					buffer[n] = 28;
					break;
				case '<':
					buffer[n] = 29;
					break;
				case '>':
					buffer[n] = 30;
					break;
				case '*':
					buffer[n] = 31;
					break;
				default:
					buffer[n] = 26;
					break;
			}
	}

	wbnick.longnick.attr = attr == 1 ? attr : 0;
	wbnick.longnick.a = buffer[0];
	wbnick.longnick.b = buffer[1];
	wbnick.longnick.c = buffer[2];
	wbnick.longnick.d = buffer[3];
	wbnick.longnick.e = buffer[4];
	wbnick.longnick.f = buffer[5];

	return wbnick.shortnick.value;
}

/**
 wbnick2ascii

 Converts 4bytes wbnick to 6bytes ascii string
 Returning value: is a static array. You must use CopyString() to set a value to something
 */

char *wbnick2ascii(u_int32_t shortnick)
{
	nick_t wbnick;
	static char playernick[7];
	u_int8_t n;

	wbnick.shortnick.value = shortnick;
	playernick[0] = wbnick.longnick.a;
	playernick[1] = wbnick.longnick.b;
	playernick[2] = wbnick.longnick.c;
	playernick[3] = wbnick.longnick.d;
	playernick[4] = wbnick.longnick.e;
	playernick[5] = wbnick.longnick.f;

	for (n=0; n<6; n++)
	{
		if (playernick[n]<26)
		{
			playernick[n] += wbnick.longnick.attr & 1 ? 'A' : 'a'; // & 1 ou == 1??
		}
		else
			switch (playernick[n])
			{
				case 26:
					playernick[n] = '-';
					break;
				case 27:
					playernick[n] = '+';
					break;
				case 28:
					playernick[n] = '=';
					break;
				case 29:
					playernick[n] = '<';
					break;
				case 30:
					playernick[n] = '>';
					break;
				case 31:
					playernick[n] = '*';
					break;
				default:
					playernick[n] = '-';
					break;
			}
	}
	playernick[6]='\0';

	return playernick;
}

/**
 wbcrypt

 Encrypt/decrypt warbirds data. (XOR algorithm)
 */

u_int8_t *wbcrypt(u_int8_t *buffer, u_int32_t key, u_int16_t size, u_int8_t oldcrypt)
{
	u_int16_t n, i;

	if (wb3->value && !oldcrypt)
	{
		// key = (key >> (size%3)) & 0xFF; // WB 2007
		key |= (size << 2);
	}
	else
	{
		key = (0xEA + (key >> (size%8))) & 0xFF;
	}

	for (n=0/*3*/, i = 0; n<size/*+3*/; n++) // n = 0 if removed 3 first bytes form packet
	{
		if (wb3->value && !oldcrypt)
		{
			//  buffer[n] ^= key; // wb2007
			buffer[n] ^= (key >> i++);

			if (!(key >> i))
				i = 0;
		}
		else
		{
			buffer[n] ^= key;
		}
	}
	return buffer;
}

/**
 memncat

 Concatenate memory data
 */

void memncat(u_int8_t **dest, u_int8_t *orig, u_int32_t destsize, u_int32_t origsize)
{
	u_int32_t n;
	u_int8_t *buffer;

	buffer = (u_int8_t *)Z_Malloc(sizeof(u_int8_t) * destsize+origsize);

	for (n=0; n<destsize; n++)
	{
		*(buffer+n) = *(*dest+n);
	}

	for (n=0; n<origsize; n++)
	{
		*(buffer+destsize+n) = *(orig+n);
	}
	free(*dest);

	*dest = buffer;
}

/**
 asc2time

 Return a formated date (like: Sun Sep 16 01:03:52 1973) but without \n like std asctime()
 */

char *asc2time(const struct tm *timeptr)
{
	static const char wday_name[7][3] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	static const char mon_name[12][3] =
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	static char result[26];
	snprintf(result, sizeof(result), "%.3s %.3s %02d %.2d:%.2d:%.2d %d", wday_name[timeptr->tm_wday], mon_name[timeptr->tm_mon], timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec,
			1900 + timeptr->tm_year);
	return result;
}

/**
 sqltime

 Return a sql-formated date (like: 1973-09-16 01:03:52)
 */

char *sqltime(const struct tm *timeptr)
{
	static char result[26];

	snprintf(result, sizeof(result), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 1900 + timeptr->tm_year, 1 + timeptr->tm_mon, timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);

	return result;
}

/**
 Com_Close

 Padronized closing socket function
 */

void Com_Close(int *fd)
{
	int i;

	shutdown(*fd, SHUT_RDWR); // TODO: Misc: make socket errors 
	/*
	 errno:
	 [EBADF] 
	 The socket argument is not a valid file descriptor. 
	 [EINVAL] 
	 The how argument is invalid. 
	 [ENOTCONN] 
	 The socket is not connected. 
	 [ENOTSOCK] 
	 The socket argument does not refer to a socket. 
	 The shutdown() function may fail if:

	 [ENOBUFS] 
	 Insufficient resources were available in the system to perform the operation. 
	 */

#ifdef _WIN32
	i = closesocket(*fd);
#else
	i = close(*fd);
#endif
	if (!i) // set socket to zero if sucessfully closed
		*fd = 0;
}

/**
 Com_Recv

 Just a recv() with padronized return value
 */

int Com_Recv(int s, u_int8_t *buf, int len)
{
	int n;
	
	if(!s)
	{
		ConnError(ENOTSOCK);
		return -1;
	}

	if ((n=recv(s, buf, len, 0)) ==
#ifdef _WIN32
			SOCKET_ERROR
#else
			-1
#endif
	)
	{
#ifdef	_WIN32
		n = WSAGetLastError();
		if (n != WSAEWOULDBLOCK)
#else
		n = errno;
		if (n != EWOULDBLOCK)
#endif
		{
			Com_Printf(VERBOSE_WARNING, "Com_Recv() socket %d error\n", s);
			ConnError(n);
			return -1;
		}
		else
			return 0;
	}
	else
	{
		if (n)
		{
#ifdef DEBUGLOGIN
			Com_Printf(VERBOSE_DEBUG, "<<--");
			Com_Printfhex(buf, n);
#endif
			if (server_speeds->value)
				arena->recv += n;
			return n;
		}
		else
			return -1;
	}
}

/**
 Com_Pow

 Potency
 */

double Com_Pow(double x, u_int32_t y)
{
#ifdef _WIN32
	return pow(x, y);
#else
	u_int32_t i;
	double z;

	if (!y)
		return 1;

	z = x;

	for (i = 1; i < y; i++)
	{
		z *= x;
	}

	return z;
#endif
}

/**
 Com_RecordLogBuffer

 Record a Send Log buffer
 */

void Com_RecordLogBuffer(client_t * client, u_int8_t *buffer, int len)
{
	u_int16_t offset;
	u_int16_t index;

	if(len <= 0)
		return;

	for(offset = client->logbuf_start, index = 0; index < len; index++, offset++)
	{
		if(offset == MAX_LOGBUFFER)
		{
			//PPrintf(client, RADIO_GREEN, "DEBUG: Record Log Buffer full");
			offset = 0;
		}

		client->logbuffer[offset] = buffer[index];
	}

	client->logbuf_start = offset;
	if(offset)
	{
		client->logbuf_end = client->logbuf_start - 1;
	}
	else
	{
		client->logbuf_end = MAX_LOGBUFFER;
	}
}

/**
 Com_PrintLogBuffer

 Print Send Log buffer
 */

void Com_PrintLogBuffer(client_t * client)
{
	u_int16_t offset;

	printf("( %u - ", client->key);

	if (logfile_active->value)
	{
		if (!logfile[0])
		{
			logfile[0] = fopen(FILE_CONSOLE, "a+");
		}

		if (logfile[0])
		{
			fprintf(logfile[0], "( %u - ", client->key);
		}
	}

	for (offset = client->logbuf_start; offset != client->logbuf_end; offset++)
	{
		if(offset == MAX_LOGBUFFER)
			offset = 0;

		printf("%02X ", client->logbuffer[offset]);

		if (logfile_active->value)
		{
			if (logfile[0])
			{
				fprintf(logfile[0], "%02X ", client->logbuffer[offset]);
			}
		}
	}
	printf(")\n");
	fflush(stdout);

	if (logfile_active->value)
	{
		if (logfile[0])
		{
			fprintf(logfile[0], ")\n");
			fflush(logfile[0]);
		}
	}
}

/**
 Com_Send

 Just a send() with padronized return value
 */

int Com_Send(client_t *client, u_int8_t *buf, int len)
{
	int n, tlen;
	//u_int8_t i;
	u_int8_t *tbuf;

	if (!client->socket) // to avoid SIGPIPE
	{
		Com_Printf(VERBOSE_WARNING, "Com_Send() tried to send data to socket zero\n");
		return -1;
	}

	// save the pointers and length
	tbuf = buf;
	tlen = len;
	
	// if there is something in buffer, send it first
	if(client->buf_offset)
	{
		// redirect the sending buffer
		buf = client->buffer;
		len = client->buf_offset;
	}

	if ((n = send(client->socket, buf, len, 0)) == -1)
	{
		if (errno != EWOULDBLOCK)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Send() socket %d error\n", client->socket);
			ConnError(errno);
			return -1;
		}
		else
		{
			if(arena->bufferit)
			{
				// socket blocked, copy data to buffer
				if((client->buf_offset + tlen) < MAX_SENDBUFFER)
				{
					memcpy(client->buffer+client->buf_offset, tbuf, tlen);
					client->buf_offset += tlen;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "%s send buffer overflow\n", client->longnick);
					return -1;
				}
			}
		
			if(!client->wouldblock)
			{
				Com_Printf(VERBOSE_WARNING, "Com_Send() %s EWOULDBLOCK %d;%d;%d;%d;%d %d;%d;%d;%d;%d\n",
						client->longnick,
						client->sendcount[0][0],
						client->sendcount[1][0]/5,
						client->sendcount[2][0]/10,
						client->sendcount[3][0]/30,
						client->sendcount[4][0]/60,
						client->recvcount[0][0],
						client->recvcount[1][0]/5,
						client->recvcount[2][0]/10,
						client->recvcount[3][0]/30,
						client->recvcount[4][0]/60);

				Com_PrintLogBuffer(client);
			}
			
			client->wouldblock = 1;
			arena->bufferit = 1;
			return 0;
		}
	}
	else
	{
#ifdef DEBUGLOGIN
		Com_Printf(VERBOSE_DEBUG, "-->>");
		Com_Printfhex(buf, n);
#endif
		if (server_speeds->value)
			arena->sent += n;
		
		Com_RecordLogBuffer(client, buf, n);
		ConnStatistics(client, n, 0 /*send*/);

		if (len != n)
		{
			buf += n;
			len -= n;
			
			Com_Printf(VERBOSE_WARNING, "%s sent %d offset %d\n", client->longnick, n, client->buf_offset);
			
				if(client->buf_offset)
				{
					// copy unsent bytes to top of the list
					memcpy(client->buffer, client->buffer+n, len);
					client->buf_offset = len;
					
					if(arena->bufferit)
					{
						// now copy the rest of data to buffer
						if((client->buf_offset + tlen) < MAX_SENDBUFFER)
						{
							memcpy(client->buffer+client->buf_offset, tbuf, tlen);
							client->buf_offset += tlen;
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "%s send buffer overflow\n", client->longnick);
						}
					}
				}
				else
				{
					if(arena->bufferit)
					{
						memcpy(client->buffer, buf, len);
						client->buf_offset = len;
					}
				}

			arena->bufferit = 1;
			return n;
		}
		else
		{
			// if all data sent from buffer, clear it and send the actual data
			if(client->buf_offset)
			{
				Com_Printf(VERBOSE_WARNING, "%s full buffer sent, now sending the actual data\n", client->longnick);
				client->wouldblock = 0;
				client->buf_offset = 0;
				return Com_Send(client, tbuf, tlen);
			}
			
			arena->bufferit = 1;
			return n;
		}
	}
}

/**
 ConnError

 Prints the recv/send error
 */

void ConnError(int n)
{
	switch (n)
	{
#ifdef _WIN32
		case WSANOTINITIALISED:
		Com_Printf(VERBOSE_WARNING, "WSAStartup() not initializated\n");
		break;
		case WSAECONNRESET:
		Com_Printf(VERBOSE_WARNING, "Connection reset by peer\n");
		break;
		case WSAENETDOWN:
		Com_Printf(VERBOSE_WARNING, "Network subsystem has failed\n");
		break;
		case WSAEFAULT:
		Com_Printf(VERBOSE_WARNING, "Buffer is not totally contained in a valid part of the user address space\n");
		break;
		case WSAENETRESET:
		Com_Printf(VERBOSE_WARNING, "Connection has been broken due to the remote host resetting\n");
		break;
		case WSAENOBUFS:
		Com_Printf(VERBOSE_WARNING, "Not enough internal buffer space available\n");
		break;
		case WSAENOTCONN:
		Com_Printf(VERBOSE_WARNING, "Socket is not connected\n");
		break;
		case WSAENOTSOCK:
		Com_Printf(VERBOSE_WARNING, "Descriptor is not a socket\n");
		break;
		case WSAESHUTDOWN:
		Com_Printf(VERBOSE_WARNING, "Socket has been shut down\n");
		break;
		case WSAEMSGSIZE:
		Com_Printf(VERBOSE_WARNING, "Message too large to send atomically or message was too large to fit into the specified buffer and was truncated\n");
		break;
		case WSAEHOSTUNREACH:
		Com_Printf(VERBOSE_WARNING, "Remote host cannot be reached from this host at this time\n");
		break;
		case WSAEINVAL:
		Com_Printf(VERBOSE_WARNING, "Socket not bound with bind()\n");
		break;
		case WSAECONNABORTED:
		Com_Printf(VERBOSE_WARNING, "Virtual circuit was terminated due to a time-out or other failure\n");
		break;
		case WSAETIMEDOUT:
		Com_Printf(VERBOSE_WARNING, "Connection has been dropped, because of a network failure or because the system on the other end went down without notice\n");
		break;
#else
		case ENOTCONN:
			Com_Printf(VERBOSE_WARNING, "Socket is not connected\n");
			break;
		case ENOTSOCK:
			Com_Printf(VERBOSE_WARNING, "Descriptor is not a socket\n");
			break;
		case EBADF:
			Com_Printf(VERBOSE_WARNING, "Socket argument is not a valid file descriptor\n");
			break;
		case EMSGSIZE:
			Com_Printf(VERBOSE_WARNING, "Message too large to send atomically\n");
			break;
		case EPIPE:
			Com_Printf(VERBOSE_WARNING, "Connection broken, failed sending SIGPIPE\n");
			break;
		case EOPNOTSUPP:
			Com_Printf(VERBOSE_WARNING, "Socket is associated with a socket that does not support one or more of the values set in flags\n");
			break;
		case EINTR:
			Com_Printf(VERBOSE_WARNING, "A signal interrupted send() before any data was transmitted\n");
			break;
		case EDESTADDRREQ:
			Com_Printf(VERBOSE_WARNING, "Socket is not connection-mode and no peer address is set\n");
			break;
		case ECONNRESET:
			Com_Printf(VERBOSE_WARNING, "A connection was forcibly closed by a peer\n");
			break;
		case ETIMEDOUT:
			Com_Printf(VERBOSE_WARNING, "Connection timed out\n");
			break;
		case EINVAL:
			Com_Printf(VERBOSE_WARNING, "The MSG_OOB flag is set and no out-of-band data is available\n");
			break;

		case EACCES:
			Com_Printf(VERBOSE_WARNING, "The calling process does not have the appropriate privileges\n");
			break;
		case EIO:
			Com_Printf(VERBOSE_WARNING, "An I/O error occurred while reading from or writing to the file system\n");
			break;
		case ENOBUFS:
			Com_Printf(VERBOSE_WARNING, "Not enough internal buffer space available\n");
			break;
		case ENOMEM:
			Com_Printf(VERBOSE_WARNING, "Insufficient memory was available to fulfill the request\n");
			break;
		case ENETUNREACH:
			Com_Printf(VERBOSE_WARNING, "No route to the network is present\n");
			break;
		case ENETDOWN:
			Com_Printf(VERBOSE_WARNING, "The local network interface used to reach the destination is down\n");
			break;
#endif
		default:
			Com_Printf(VERBOSE_WARNING, "Connection dropped (Errno: %d)\n", n);
			break;
	}
}

/**
 Com_Read

 Read from given input
 // TODO: FIXME: function not used yet in TabaHost, should this be used??
 */

int Com_Read(FILE *fp, u_int8_t *buffer, u_int32_t num)
{

#ifdef _WIN32
	return read(fp->_file, buffer, num);
#else
	return fread(buffer, 1, num, fp);
#endif

}

/**
 Com_LogEvent

 Logs an Event
 */

void Com_LogEvent(u_int32_t event, u_int32_t player_id, u_int32_t victim_id)
{
	char query_log[256];

	// FIXME: DEBUG
		return;

	snprintf(query_log, sizeof(query_log), "INSERT INTO log_events SET event = '%u', player_id = '%u', victim_id = '%u', date = FROM_UNIXTIME(%u)", event, player_id, victim_id, (u_int32_t)time(NULL));

	if (d_mysql_query(&my_sock, query_log))
	{
		Com_Printf(VERBOSE_WARNING, "Com_LogEvent(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	my_id = (u_int32_t)mysql_insert_id(&my_sock);
}

/**
 Com_LogDescription

 Logs description of an event
 */

void Com_LogDescription(u_int32_t type, double value, char *string)
{
	char query_log[256];

	// FIXME: DEBUG
		return;

	snprintf(query_log, sizeof(query_log), "INSERT INTO log_descriptions SET event_id = '%u', type = '%u'", my_id, type);

	if (value)
		sprintf(query_log, "%s, value = '%.3f'", query_log, value);
	if (string)
		sprintf(query_log, "%s, string = '%s'", query_log, string);

	if (d_mysql_query(&my_sock, query_log))
	{
		Com_Printf(VERBOSE_WARNING, "Com_LogDescription(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/**
 Com_Printf

 Prints data in console and in logfile if enabled
 */

void Com_Printf(int8_t verb, char *fmt, ...)
{
	va_list argptr;
	char msg[MAX_PRINTMSG];
	u_int8_t tverb;
	time_t ltime;

	if(verb <= verbose->value);
	{
		tverb = MODULUS(verb);
		va_start(argptr, fmt);
		vsprintf(msg, fmt, argptr);
		va_end(argptr);

		time(&ltime);
		if (logfile_active->value)
		{
			if (!logfile[0])
				logfile[0] = fopen(FILE_CONSOLE, "a+");
			if(!logfile[0])
				printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_CONSOLE);

			if(tverb)
			{
				if (!logfile[tverb])
				{
					switch (verb)
					{
						case VERBOSE_ATTENTION:
							logfile[tverb] = fopen(FILE_ATTENTION, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_ATTENTION);
							break;
						case VERBOSE_WARNING:
							logfile[tverb] = fopen(FILE_WARNING, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_WARNING);
							break;
						case VERBOSE_ERROR:
							logfile[tverb] = fopen(FILE_ERROR, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_ERROR);
							break;
						case VERBOSE_DEBUG_SCORES:
							logfile[tverb] = fopen(FILE_DEBUG_SCORES, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_DEBUG_SCORES);
							break;
						case VERBOSE_DEBUG_DAMAGE:
							logfile[tverb] = fopen(FILE_DEBUG_DAMAGE, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_DEBUG_DAMAGE);
							break;
						case VERBOSE_DEBUG:
							logfile[tverb] = fopen(FILE_DEBUG, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_DEBUG);
							break;
						case VERBOSE_ONLINE:
							logfile[tverb] = fopen(FILE_ONLINE, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_ONLINE);
							break;
						case VERBOSE_CHAT:
							logfile[tverb] = fopen(FILE_CHAT, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_CHAT);
							break;
						case VERBOSE_KILL:
							logfile[tverb] = fopen(FILE_KILL, "a+");
							if(!logfile[tverb])
								printf("%sZ: WARNING: Com_Printf() Could not open %s\n", asc2time(gmtime(&ltime)), FILE_KILL);
							break;
					}
				}
			}
		}

		printf("%sZ: ", asc2time(gmtime(&ltime)));
		if (logfile[0])
			fprintf(logfile[0], "%sZ: ", asc2time(gmtime(&ltime)));
		if (tverb && logfile[tverb])
			fprintf(logfile[tverb], "%sZ: ", asc2time(gmtime(&ltime)));

		switch(verb)
		{
			case VERBOSE_ATTENTION:
				printf("ATTENTION: ");
				if (logfile[0])
					fprintf(logfile[0], "ATTENTION: ");
				if (tverb && logfile[tverb])
					fprintf(logfile[tverb], "ATTENTION: ");
				break;
			case VERBOSE_WARNING:
				printf("WARNING: ");
				if (logfile[0])
					fprintf(logfile[0], "WARNING: ");
				if (tverb && logfile[tverb])
					fprintf(logfile[tverb], "WARNING: ");
				break;
			case VERBOSE_ERROR:
				printf("ERROR: ");
				if (logfile[0])
					fprintf(logfile[0], "ERROR: ");
				if (tverb && logfile[tverb])
					fprintf(logfile[tverb], "ERROR: ");
				break;
			case VERBOSE_DEBUG_SCORES:
				printf("DEBUG: SCORES: ");
				if (logfile[0])
					fprintf(logfile[0], "DEBUG: SCORES: ");
				if (tverb && logfile[tverb])
					fprintf(logfile[tverb], "DEBUG: SCORES: ");
				break;
			case VERBOSE_DEBUG_DAMAGE:
				printf("DEBUG: DAMAGE: ");
				if (logfile[0])
					fprintf(logfile[0], "DEBUG: DAMAGE: ");
				if (tverb && logfile[tverb])
					fprintf(logfile[tverb], "DEBUG: DAMAGE: ");
				break;
			case VERBOSE_DEBUG:
				printf("DEBUG: ");
				if (logfile[0])
					fprintf(logfile[0], "DEBUG: ");
				if (tverb && logfile[tverb])
					fprintf(logfile[tverb], "DEBUG: ");
				break;
		}
		printf("%s", msg);
		fflush(stdout);
		if (logfile[0])
		{
			fprintf(logfile[0], "%s", msg);
			fflush(logfile[0]); // forces data to be writed
		}
		if (tverb && logfile[tverb])
		{
			fprintf(logfile[tverb], "%s", msg);
			fflush(logfile[tverb]); // forces data to be writed
		}
	}
}

/**
 Com_TimeSeconds

 Prints time in Minutes and Seconds format: 12'45"
 */

char *Com_TimeSeconds(u_int32_t seconds)
{
	static char buffer[32];

	snprintf(buffer, sizeof(buffer), "%3u'%02u\"", (seconds / 60), (seconds % 60));

	return buffer;
}

/**
 Com_Padloc

 Prints coordinates in padloc x.x.x.x format
 */

char *Com_Padloc(int32_t x, int32_t y)
{
	static char buffer[32];
	char szPadLoc[20];

	PadLoc(szPadLoc, x, y);

	snprintf(buffer, sizeof(buffer), "%.0f.%.0f%s", WBLongitude(x), WBLatitude(y), szPadLoc);

	return buffer;
}

int d_mysql_query(MYSQL *mysql, const char *query)
{
	u_int32_t querytime;
	int32_t i;

	if (printqueries->value)
		Com_Printf(VERBOSE_DEBUG, "MYSQL: \"%s\"\n", query);

	querytime = Sys_Milliseconds();
	if ((i = mysql_query(mysql, query)))
	{
		return i;
	}

	if ((querytime = Sys_Milliseconds() - querytime) > 70 /*ms*/)// || mysqlview->value)
	{
		Com_Printf(VERBOSE_WARNING, "d_mysql_query() delayed query %ums \"%s\"\n", querytime, query);
	}

	return i;
}

/**
 Com_MySQL_Query

 This function avoid client SQL flooding
 */

int Com_MySQL_Query(client_t *client, MYSQL *mysql, const char *query)
{
	u_int32_t querytime;
	int32_t i;

	if (printqueries->value)
		Com_Printf(VERBOSE_DEBUG, "MYSQL \"%s\"\n", query);

	if (!client || ((arena->frame - client->lastsql) > 300/*3 sec*/))
	{
		querytime = Sys_Milliseconds();
		if ((i = mysql_query(mysql, query)))
		{
			Com_Printf(VERBOSE_WARNING, "Com_MySQL_Query(): %s error %d: %s\n", client ? client->longnick : "-HOST-", mysql_errno(mysql), mysql_error(mysql));
			PPrintf(client, RADIO_YELLOW, "SQL Error (%d), please contact admin", mysql_errno(mysql));
			return i;
		}

		if ((querytime = Sys_Milliseconds() - querytime) > 70 /*ms*/)// || mysqlview->value)
		{
			Com_Printf(VERBOSE_WARNING, "Com_MySQL_Query() delayed query %ums \"%s\"\n", querytime, query);
		}
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "SQL flood, wait %.2f seconds", (double)(300 - (arena->frame - client->lastsql))/100);
		Com_Printf(VERBOSE_WARNING, "%s SQL flood\n", client->longnick);
		return -1;
	}

	if (client)
	{
		client->lastsql = arena->frame;
	}

	return 0;
}

/**
 Com_MySQL_Flush

 This function flushes multiple statements results
 */

void Com_MySQL_Flush(MYSQL *mysql, char *file, u_int32_t line)
{
	MYSQL_RES *result= NULL;
	int16_t status = 0;

	do
	{
		// did current statement return data?
		result = mysql_store_result(mysql);

		if (result)
		{
			mysql_free_result(result);
		}
		else // no result set or error
		{
			if (mysql_field_count(mysql) == 0)
			{
				Com_Printf(VERBOSE_DEBUG, "Com_MySQL_Flush(%s, %u): %lld rows affected\n", file, line, mysql_affected_rows(mysql));
			}
			else // some error occurred
			{
				Com_Printf(VERBOSE_WARNING, "Com_MySQL_Flush(%s, %u): Could not retrieve result set\n", file, line);
				break;
			}
		}
		// more results? -1 = no, >0 = error, 0 = yes (keep looping)
		if ((status = mysql_next_result(mysql)) > 0)
			Com_Printf(VERBOSE_WARNING, "Com_MySQL_Flush(%s, %u): Could not execute statement\n", file, line);
	} while (status == 0);
}
 
/**
 Com_Printfhex

 Prints a buffer in hexadecimal format
 */

void Com_Printfhex(unsigned char *buffer, int len)
{
	int i;

	printf("( ");

	if (logfile_active->value)
	{
		if (!logfile[0])
		{
			logfile[0] = fopen(FILE_CONSOLE, "a+");
		}

		if (logfile[0])
		{
			fprintf(logfile[0], "( ");
		}
	}

	if (!((buffer[0] == 7) && (buffer[2] == (len - 1))))
	{
		printf("07 00 %02X ", len-1);

		if (logfile_active->value)
		{
			if (logfile[0])
			{
				fprintf(logfile[0], "07 00 %02X ", len-1);
			}
		}
	}

	for (i=0; i<len; i++)
	{
		printf("%02X ", buffer[i]);

		if (logfile_active->value)
		{
			if (!logfile[0])
			{
				logfile[0] = fopen(FILE_CONSOLE, "a+");
			}

			if (logfile[0])
			{
				fprintf(logfile[0], "%02X ", buffer[i]);
			}
		}
	}
	printf(")\n");
	fflush(stdout);

	if (logfile_active->value)
	{
		if (logfile[0])
		{
			fprintf(logfile[0], ")\n");
			fflush(logfile[0]);
		}
	}
}

/**
 Com_Stricmp

 Compares strings ignoring case
 */

int Com_Stricmp(char *s1, char *s2)
{
	if (s1 && s2)
	{
#ifdef _WIN32
		return stricmp(s1, s2);
#else
		return strcasecmp(s1, s2);
#endif
	}
	else
	{
		if (!s1)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Stricmp() s1 NULL array\n");
		}
		if (!s2)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Stricmp() s2 NULL array\n");
		}
		return 1;
	}
}

/**
 Com_CheckAphaNum

 Parse string for valid chars "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
 */

u_int8_t Com_CheckAphaNum(char *string)
{
	u_int8_t i, j;

	j = strlen(string);

	for (i = 0; i < j; i++)
	{
		if (!((string[i] >= '0' && string[i] <= '9') || (string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= 'a' && string[i] <= 'z')))
			return 1;
	}

	return 0;
}

/**
 Com_CheckWBUsername

 Parse string for valid chars "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-"
 */

u_int8_t Com_CheckWBUsername(char *string)
{
	u_int8_t i, j;

	j = strlen(string);

	for (i = 0; i < j; i++)
	{
		string[i] = tolower(string[i]);

		if (!((string[i] >= 'a' && string[i] <= 'z') || string[i] == '-'))
			return 1;
	}

	return 0;
}

/**
 Com_ParseString

 Remove invalid chars: ', ", \, \n, \r, \t
 */

void Com_ParseString(char *string)
{
	u_int8_t i, j;

	j = strlen(string);

	for (i = 0; i < j; i++)
	{
		if (string[i] == '\'')
		{
			string[i] = '`';
		}
		else if (string[i] == '\\' || string[i] < ' ' || string[i] > '~')
		{
			string[i] = 0x20; // space
		}
	}
}

/**
 GetSlot

 Get client's slot in visible list
 */

u_int8_t GetSlot(client_t *plane, client_t *client)
{
	u_int8_t i;

	if (!plane)
	{
		Com_Printf(VERBOSE_WARNING, "GetSlot(), plane == NULL\n");
		return MAX_SCREEN;
	}

	for (i = 0; i < MAX_SCREEN; i++)
	{
		if (client->visible[i].client == plane)
			return i;
	}

	return MAX_SCREEN;
}

/**
 CheckSum

 Returns the wb packet checksum
 */

u_int8_t CheckSum(u_int8_t *buffer, u_int16_t len)
{
	u_int16_t i;
	u_int8_t sum=0;

	for (i=0; i<len; i++)
	{
		sum+=buffer[i];
	}

	return sum;
}

/**
 RocketAngle

 Calculates the angle of rocket lauching to reach dist
 */

double RocketAngle(u_int16_t dist)
{
	double a, b, c, x;

	a = -5.3418;
	b = 500.11;
	c = -2163.2 - dist;

	x = (b * b) - (4 * a * c);

	if (x < 0)
		return -1;
	else
		c = (b * -1 + sqrt(x)) / (2 * a);

	return c;
}

/**
 RocketTime

 Calculates the time of rocket lauching to reach dist
 */

double RocketTime(double angle)
{
	double a, b, c, y;

	a = -0.0029;
	b = 0.7115;
	c = -2.1338;

	y = a * angle * angle + b * angle + c;

	return y;
}

/**
 WBtoHdg

 Converts WB yaw angle to compass Heading
 */

double WBtoHdg(int16_t angle)
{
	double heading;

	if (angle < 0)
		heading = (double)((double)(angle + 3600) / 10);
	else
		heading = (double)((double)angle / 10);

	if (wb3->value)
		return (360.0L - heading);
	else
		return heading;
}

/**
 AngleTo

 Calculates the angle from origin to destiny
 */

double AngleTo(int32_t origx, int32_t origy, int32_t destx, int32_t desty)
{
	int32_t x, y;
	double ang;

	x = origx - destx;
	y = desty - origy;

	if (!y)
	{
		if (x > 0)
			ang = 90;
		else
			ang = 270;
	}
	else
		ang = Com_Deg(atan2(x, y));

	if (ang < 0)
	{
		ang += 360;
	}

	return ang;
}

/**
 CopyString

 Reserve memory space and copy
 */

char *CopyString(char *in)
{
	char *out;

	if(!in)
		return NULL;

	out = Z_Malloc(strlen(in)+1);
	strcpy(out, in);
	return out;
}

/**
 crc32

 Generates a 32bytes CRC
 */

static unsigned long int
		crc_32_tab[] =
		{ /* CRC polynomial 0xedb88320 */
		0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
				0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63,
				0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75,
				0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87,
				0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
				0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
				0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc,
				0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e,
				0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
				0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671,
				0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767,
				0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795,
				0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
				0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8,
				0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee,
				0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c,
				0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };

#define UPDC32(octet, crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

u_int32_t crc32(char *buffer, u_int8_t size)
{
	register unsigned long oldcrc32;
	register unsigned long crc32;
	register unsigned long oldcrc;
	register int n;

	oldcrc32 = 0xFFFFFFFF;

	for (n=0; n<size; n++)
	{
		oldcrc32 = UPDC32(buffer[n], oldcrc32);
	}

	crc32 = oldcrc32;
	oldcrc = oldcrc32 = ~oldcrc32;

	/**/
	crc32 = UPDC32((oldcrc32 & 0377), crc32);
	oldcrc32 >>=8;
	crc32 = UPDC32((oldcrc32 & 0377), crc32);
	oldcrc32 >>=8;
	crc32 = UPDC32((oldcrc32 & 0377), crc32);
	oldcrc32 >>=8;
	crc32 = UPDC32((oldcrc32 & 0377), crc32);
	oldcrc32 >>=8;
	/**/
	return oldcrc;
}

/**
 Com_Rad

 Converts Degrees to Radians
 */

double Com_Rad(double angle)
{
	return (M_PI * angle / 180);
}

/**
 Com_Deg

 Converts Radians to Degrees
 */

double Com_Deg(double angle)
{
	return (angle * 180 / M_PI);
}

/**
 Com_Atoi

 atoi handling NULL arrays
 */

int Com_Atoi(char *string)
{
	if (string)
		return atoi(string);
	else
	{
		Com_Printf(VERBOSE_WARNING, "Com_Atoi() NULL array\n");
		return 0;
	}
}

/**
 Com_Atou

 atou handling NULL arrays
 */

u_int32_t Com_Atou(char *string)
{
	u_int32_t result = 0;
	u_int16_t i;
	char ch;

	if (string)
	{
		for (i = 0; i < strlen(string); i++)
		{
			ch = string[i];

			if (ch > '9' || ch < '0')
				break;
			//if(ch >= '0' && ch <= '9')	// comments the 2 lines above and uncomment
			// this line to process all number in the string
			// as one number (123+456 == 123456)

			result = result * 10 + ch - '0';
		}

		return result;
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Com_Atou() NULL array\n");
		return 0;
	}
}

/**
 Com_Atof

 atof handling NULL arrays
 */

double Com_Atof(char *string)
{
	if (string)
		return atof(string);
	else
	{
		Com_Printf(VERBOSE_WARNING, "Com_Atof() NULL array\n");
		return 0;
	}
}

/**
 Com_MyRow

 Return the column value from given column name
 */

char *Com_MyRow(char *string)
{
	MYSQL_FIELD *fields;
	u_int8_t i, num_fields;

	if (my_result && my_row)
	{
		num_fields = mysql_num_fields(my_result);
		fields = mysql_fetch_fields(my_result);

		for (i = 0; i < num_fields; i++)
		{
			if (!Com_Stricmp(string, fields[i].name))
			{
				return my_row[i];
			}
		}

		Com_Printf(VERBOSE_WARNING, "Com_MyRow(): column '%s' not found\n", string);
	}

	return NULL;
}

/**
 Com_SquadronName

 Prints the Squadron name
 */

char *Com_SquadronName(u_int32_t owner)
{
	static char buffer[64];

	if (!owner)
		return NULL;

	memset(buffer, 0, sizeof(buffer));

	sprintf(my_query, "SELECT name FROM squads WHERE owner = '%u'", owner);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if ((my_row = mysql_fetch_row(my_result)))
			{
				snprintf(buffer, sizeof(buffer), "%s", Com_MyRow("name") ? Com_MyRow("name") : "(null)");
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Com_SquadronName(): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Com_SquadronName(): my_result == NULL, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Com_SquadronName(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return buffer;
}

/**
 Com_Strncmp

 strncmp handling null arrays
 */

int Com_Strncmp(char *s1, char *s2, int n)
{
	if (s1 && s2)
	{
		return strncmp(s1, s2, n);
	}
	else
	{
		if (!s1)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Strncmp() s1 NULL array\n");
		}
		if (!s2)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Strncmp() s2 NULL array\n");
		}
		return 1;
	}
}

/**
 Com_Strcmp

 Compares strings case sensitive
 */

int Com_Strcmp(char *s1, char *s2)
{
	if (s1 && s2)
	{
		return strcmp(s1, s2);
	}
	else
	{
		if (!s1)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Strcmp() s1 NULL array\n");
		}
		if (!s2)
		{
			Com_Printf(VERBOSE_WARNING, "Com_Strcmp() s2 NULL array\n");
		}
		return 1;
	}
}

/**
 Com_PacketTabIndex

 Returns index from packet tab
 */

int16_t Com_PacketTabIndex(u_int16_t packetid, u_int8_t wbv)
{
	u_int16_t i, j;

	j = 209; // sizeof(packets_tab); // FRANZ verificar

	if (wbv > 2)
		wbv = 2;

	for (i = 0; i < j; i++)
	{
		if (packetid == packets_tab[i][wbv])
		{
			return i;
		}
	}

	return -1;
}

/**
 Com_WBntoh

 Converts external packet_id to internal handling
 */

void Com_WBntoh(u_int16_t *packetid)
{
	int16_t tab_index;
	u_int8_t wbv;

	wbv = wb3->value;

	if (wbv > V_WB2008)
		wbv = V_WB2008;

	if (wbv != V_WB2007)
	{
		if ((tab_index = Com_PacketTabIndex(*packetid, wbv)) < 0)
		{
			Com_Printf(VERBOSE_WARNING, "Com_WBntoh() (S<-C) packet not identified yet (packetid = 0x%04X)\n", *packetid);
			// leave packet_id as it is to be detected below
		}
		else
		{
			*packetid = packets_tab[tab_index][V_WB2007]; // convert packet id to internal id
			if (packets_tab[tab_index][V_WB2007] == 0xffff)
				Com_Printf(VERBOSE_WARNING, "Com_WBntoh() (S<-C) invalid packetid conversion (0x%04X)\n", *packetid);
		}
	}
}

/**
 Com_WBhton

 Converts internal packet_id to external handling
 */

u_int16_t Com_WBhton(u_int16_t packetid)
{
	int16_t tab_index;
	u_int8_t wbv;

	wbv = wb3->value;

	if (wbv > V_WB2008)
		wbv = V_WB2008;

	if (wbv != V_WB2007)
	{
		if ((tab_index = Com_PacketTabIndex(packetid, V_WB2007)) < 0)
		{
			Com_Printf(VERBOSE_WARNING, "Com_WBhton() (S->C) packet id not found (packetid = 0x%04X)\n", packetid);
			packetid = 0xffff;
			// change packetid to be detected below
		}
		else
		{
			packetid = packets_tab[tab_index][wbv]; // convert packet id to external id
			if (packetid == 0xffff)
				Com_Printf(VERBOSE_WARNING, "Com_WBhton() (S->C) packet not identified yet (0x%04X)\n", packets_tab[tab_index][V_WB2007]);
		}
	}

	return packetid;
}

/**
 Com_WB3ntoh DEBUG: OBSOLETE

 Converts packets ID from WB3 to WB2
 */

u_int8_t Com_WB3ntoh(u_int8_t n)
{
	switch (n)
	{
		case 0x19:
			return 0x1B;
		case 0x00:
			return 0x21;
		case 0x03:
			return 0x03; //
		case 0x04:
			return 0x02;
		case 0x0D:
			return 0x0C;
		case 0x21:
			return 0x0F;
		case 0x16:
			return 0x1F;
		case 0x1D:
			return 0x09;
		case 0x12:
			return 0x12; //
		case 0x0E:
			return 0x0E; //
		case 0x20:
			return 0x1A;
		case 0x24:
			return 0x12;
			//	case 0x1B:
			//		return 0x18;
		default:
			Com_Printf(VERBOSE_WARNING, "Com_WB3ntoh() Unknown ID %X\n", n);
			return 0x00;
	}
}

/**
 Com_WB3hton DEBUG: OBSOLETE

 Converts packets ID from WB2 to WB3
 */

u_int16_t Com_WB3hton(u_int16_t m)
{
	u_int16_t n, o;

	n = (m >> 8);
	o = (m & 0xff);

	switch (n)
	{
		case 0x1A:
			n = 0x20;
			break;
		case 0x1B:
			n = 0x19;
			break;
		case 0x18:
			n = 0x1B;
			break;
		case 0x21:
			n = 0x00;
			break;
		case 0x03:
			n = 0x03; //
			break;
		case 0x02:
			n = 0x04;
			break;
		case 0x0C:
			n = 0x0D;
			break;
		case 0x0F:
			n = 0x21;
			break;
		case 0x1F:
			n = 0x16; // 0x16 debug
			break;
		case 0x09:
			n = 0x1D;
			break;
		case 0x12:
			n = 0x12; // FRANZ Verificar
			break;
		case 0x0E:
			n = 0x0E; //
			break;
		default:
			Com_Printf(VERBOSE_WARNING, "Com_WB3hton() Unknown ID %X\n", n);
			//n = 0x99;
			break;
	}

	return ((n << 8) + o);
}

/**
 DistBetween

 Returns the distance between two point in space
 */

int32_t DistBetween(int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2, int32_t envelope)
{
	double x, y, z, distance;

	x = x1 - x2;
	y = y1 - y2;
	z = z1 - z2;

	distance = envelope;

	if (envelope < 0)
	{
		distance = sqrt(Com_Pow(x, 2) + Com_Pow(y, 2));
		distance = sqrt(Com_Pow(distance, 2) + Com_Pow(z, 2));
		return (int32_t)distance;
	}
	else if (x > -envelope && x < envelope && y > -envelope && y < envelope && z > -envelope && z < envelope)
	{
		distance = sqrt(Com_Pow(x, 2) + Com_Pow(y, 2));
		distance = sqrt(Com_Pow(distance, 2) + Com_Pow(z, 2));
	}

	if (distance < envelope)
		return (int32_t)distance;
	else
		return -1;
}

double Com_Log(double number, double base)
{
	return log(number) / log(base);
}

/**
 FloorDiv

 gets a floor from num/demon
 */

double FloorDiv(double numerator, double denominator)
{
	return floor(numerator / denominator);
}

/**
 ModRange

 returns the value between 0 and denominator
 */

double ModRange(double numerator, double denominator)
{
	return numerator - floor(numerator / denominator) * denominator;
}

/**
 RoundtoBase

 rounds value to nearest base multiple
 */

double RoundtoBase(double value, double base)
{
	return floor((value + base / 2.0L) / base) * base;
}

/**
 WBLongitude

 returns the West-East (X) square position
 */

double WBLongitude(double dAbsLongitude)
{
	double cdSquare00Longitude;
	double cdSquareWidth;

	cdSquareWidth = (double)mapscale->value * 5280;
	cdSquare00Longitude = (double)mapsize->value * 165;

	return floor((dAbsLongitude - cdSquare00Longitude) / cdSquareWidth);
}

/**
 WBLatitude

 returns the North-South (Y) square position
 */

double WBLatitude(double dAbsLatitude)
{
	double cdSquare00Latitude;
	double cdSquareHeight;

	cdSquareHeight = (double)mapscale->value * 5280;
	cdSquare00Latitude = (double)mapsize->value * 165;

	return floor((dAbsLatitude - cdSquare00Latitude) / cdSquareHeight);
}

/**
 WBAngels

 converts feets to Angels
 */

double WBAngels(double dAbsAltitude)
{
	return floor((dAbsAltitude + 500.0) / 1000.0L);
}

/**
 WBAltMeters

 converts feets to meters, rounding to nearest 100 multiple
 */

double WBAltMeters(double dAbsAltitude)
{
	return RoundtoBase(dAbsAltitude * 0.3048, 100.0L);
}

/**
 WBVSI

 returns a string with climb situation
 */

char *WBVSI(double dClimbRate, int fIsBomber)
{
	static char vszResult[5][20] =
	{
	{ "diving" },
	{ "descending" },
	{ "heading" },
	{ "climbing" },
	{ "zooming" } };

	if (fIsBomber != 0 && fIsBomber != 1)
		return vszResult[2];

	if (dClimbRate < -164.0L / (fIsBomber + 1.0L))
		return vszResult[0];
	else if (dClimbRate < -33.0L / (fIsBomber + 1.0L))
		return vszResult[1];
	else if (dClimbRate < 16.0L / (fIsBomber + 1.0L))
		return vszResult[2];
	else if (dClimbRate < 82.0L / (fIsBomber + 1.0L))
		return vszResult[3];
	else
		return vszResult[4];
}

/**
 WBRhumb

 returns a string with compass notation heading
 */

char *WBRhumb(double dHeading /* in degrees*/)
{
	static char vszRhumbs[18][20] =
	{ "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", "N", "nowhere" };

	int nIdx = FloorDiv(ModRange(dHeading, 360.0L) + 11.25L, 22.5L);

	return vszRhumbs[nIdx];
}

/**
 WBHeading

 returns heading rounded to 5 unities
 */

double WBHeading(double dHeading)
{
	return RoundtoBase(dHeading, 5.0L);
}

/**
 PredictorCorrector32

 Made a predictor-corrector analysis and return next prediction for the value
 */

int32_t PredictorCorrector32(int32_t *values, u_int8_t degree)
{
	u_int8_t i;
	int64_t val[MAX_PREDICT];

	for (i = 0; i < MAX_PREDICT; i++)
	{
		val[i] = (int64_t)values[i];
	}

	switch (degree)
	{
		case 1:
			return (int32_t) 3*val[0] - 3*val[1] + val[2];
		case 2:
			return (int32_t) 4*val[0] - 6*val[1] + 4*val[2] - val[3];
		case 3:
			return (int32_t) 5*val[0] - 10*val[1] + 10*val[2] - 5*val[3] + val[4];
		case 4:
			return (int32_t) 6*val[0] - 15*val[1] + 20*val[2] - 15*val[3] + 6*val[4] - val[5];
		default:
			return 0;
	}
}

/**
 PredictorCorrector16

 Made a predictor-corrector analysis and return next prediction for the value
 */

int16_t PredictorCorrector16(int16_t *values, u_int8_t degree)
{
	u_int8_t i;
	int32_t val[MAX_PREDICT];

	for (i = 0; i < MAX_PREDICT; i++)
	{
		val[i] = (int32_t)values[i];
	}

	switch (degree)
	{
		case 1:
			return (int16_t) 3*val[0] - 3*val[1] + val[2];
		case 2:
			return (int16_t) 4*val[0] - 6*val[1] + 4*val[2] - val[3];
		case 3:
			return (int16_t) 5*val[0] - 10*val[1] + 10*val[2] - 5*val[3] + val[4];
		case 4:
			return (int16_t) 6*val[0] - 15*val[1] + 20*val[2] - 15*val[3] + 6*val[4] - val[5];
		default:
			return 0;
	}
}

/**
 PadLoc

 returns a string with padloc notation
 */

char *PadLoc(char *szBuffer, double dLongitude, double dLatitude)
{
	double dPad1;
	double dPad2;
	double cdSquareWidth;
	memset(szBuffer, 0, 20);

	cdSquareWidth = (double)mapscale->value * 5280;
	dLongitude -= (double)mapsize->value * 165;
	dLatitude -= (double)mapsize->value * 165;

	/*normalisation to 0..105600 range*/
	while(dLongitude < 0.0L)
		dLongitude += cdSquareWidth;

	dLongitude = ModRange(dLongitude, cdSquareWidth); // returns a number between zero and cdSquareWidth

	while(dLatitude < 0.0L)
		dLatitude += cdSquareWidth;

	dLatitude = ModRange(dLatitude, cdSquareWidth); // returns a number between zero and cdSquareWidth

	/*calculation*/
	dPad1 = (FloorDiv(dLongitude, (cdSquareWidth/3)) + 1) + (3 * FloorDiv(dLatitude, (cdSquareWidth/3)));

	/*normalisation to 0..35200 range*/
	while(dLatitude < 0.0L)
		dLatitude += (cdSquareWidth/3);
	dLatitude = ModRange(dLatitude, (cdSquareWidth/3)); // returns a number between zero and cdSquareWidth/3

	while(dLongitude < 0.0L)
		dLongitude += (cdSquareWidth/3);
	dLongitude = ModRange(dLongitude, (cdSquareWidth/3)); // returns a number between zero and cdSquareWidth/3

	/*calculation*/
	dPad2 = (FloorDiv(dLongitude, (cdSquareWidth/9)) + 1) + (3 * FloorDiv(dLatitude, (cdSquareWidth/9)));

	sprintf(szBuffer, ".%1.0f.%1.0f", dPad1, dPad2);

	return szBuffer;
}

/*
char *replace_str(char *str, char *orig, char *rep)
{
	static char buffer[4096];
	char *p;

	if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
	return str;

	strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
	buffer[p-str] = '\0';

	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

	return buffer;
}
*/
