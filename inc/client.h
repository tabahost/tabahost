/******************************************************************************
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
 *****************************************************************************/

/**
  * @file		/include/client.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		client header file
  */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "shared.h"

void	InitClients(void);
void	AddClient(int socket, struct sockaddr_in *cli_addr);
void	RemoveClient(client_t *client);
void	DebugClient(const char *file, u_int32_t line, u_int8_t kick, client_t *client);
int		ProcessClient (client_t *client);
void	BackupPosition(client_t *client, u_int8_t predict);
int32_t ProcessLogin (client_t *client);
int 	CalcLoginKey(u_int8_t *Data, int Len);
u_int8_t LoginKey(client_t *client);
void 	DecryptOctet(u_int8_t Octet[8], unsigned long Key);
void 	DecryptBlock(u_int8_t *Buf, int Len, long Key);
int8_t	CheckUserPasswd(client_t *client, u_int8_t *userpass);
int8_t LoginTypeRequest(u_int8_t *buffer, client_t *client);
client_t *FindDBClient(u_int32_t dbid);
client_t *FindSClient(u_int32_t shortnick);
client_t *FindLClient(char *longnick);
int		PPrintf(client_t *client, u_int16_t radio, const char *fmt, ...);
void	CPrintf(u_int8_t country, u_int16_t radio, const char *fmt, ...);
void	BPrintf(u_int16_t radio, const char *fmt, ...);
u_int8_t CheckBanned(client_t *client);
u_int8_t CheckTK(client_t *client);
u_int8_t GetClientInfo(client_t *client);
void	UpdateClientFile(client_t *client);
int8_t	AddKiller(client_t *victim, client_t *client);
void	ClearKillers(client_t *client);
void	ClearBombers(u_int8_t field);
void	CalcEloRating(client_t *winner, client_t *looser, u_int8_t flags);
client_t *NearPlane(client_t *client, u_int8_t country, int32_t limit);
void	ForceEndFlight(u_int8_t remdron, client_t *client);
void	ReloadWeapon(u_int16_t weapon, u_int16_t value, client_t *client);
void	WB3AiMount(u_int8_t *buffer, client_t *client);
void	WB3ClientSkin(u_int8_t *buffer, client_t *client);
char	*CreateSkin(client_t *client, u_int8_t number);
void	WB3OverrideSkin(u_int8_t slot, client_t *client);
void	ClientHDSerial(u_int8_t *buffer, client_t *client);
void	ClientIpaddr(client_t *client);
void	LogRAWPosition(u_int8_t server, client_t *client);
void	LogPosition(client_t *client);
void	HardHit(u_int8_t munition, u_int8_t penalty, client_t *client);

#endif /* CLIENT_H_ */
