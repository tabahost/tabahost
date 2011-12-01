/******************************************************************************
 *  Copyright (C) 2004-2009 Francisco Bischoff
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
  * @file		/include/net.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		network header file
  */

#ifndef NET_H_
#define NET_H_

#include "shared.h"

int		InitTCPNet(int portno);
int32_t	SendPacket(u_int8_t *buffer, u_int16_t len, client_t *client);
int32_t	SendPacketTHL(u_int8_t *buffer, u_int16_t len, client_t *client);
int		GetPacket(client_t *client); // recv and process packet (unfinished?)
void	FlushSocket(int sockfd);
void	ProtocolError(client_t *client);
void	ConnStatistics(client_t *client, u_int32_t len, u_int8_t type);

#endif /* NET_H_ */
