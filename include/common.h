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
  * @file		/include/common.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		common header file
  */

#ifndef COMMON_H_
#define COMMON_H_

#include "shared.h"

void	*Z_Malloc (u_int32_t size);
u_int32_t ascii2wbnick(const char *playernick, u_int8_t attr);
char *wbnick2ascii(u_int32_t shortnick); //void 	wbnick2ascii(u_int32_t shortnick, u_int8_t *playernick);
u_int8_t *wbcrypt(u_int8_t *buffer, u_int32_t key, u_int16_t size, u_int8_t oldcrypt);
void	memncat(u_int8_t **dest, u_int8_t *orig, u_int32_t destsize, u_int32_t origsize);
char	*asc2time(const struct tm *timeptr);
char	*sqltime(const struct tm *timeptr);
void	Com_Close(int *fd);
int		Com_Recv(int s, u_int8_t *buf, int len);
double	Com_Pow(double x, u_int32_t y);
void	Com_RecordLogBuffer(client_t * client, u_int8_t *buffer, int len);
void	Com_PrintLogBuffer(client_t * client);
int		Com_Send(client_t *client, u_int8_t *buf, int len);
void	ConnError(int n);
int		Com_Read(FILE *fp, u_int8_t *buffer, u_int32_t num);
void	Com_LogEvent(u_int32_t event, u_int32_t player_id, u_int32_t victim_id);
void	Com_LogDescription(u_int32_t type, double value, char *string);
void	Com_Printf(int8_t verb, const char *fmt, ...);
char	*Com_TimeSeconds(u_int32_t seconds);
char	*Com_Padloc(int32_t x, int32_t y);
int		d_mysql_query(MYSQL *mysql, const char *query);
int		Com_MySQL_Query(client_t *client, MYSQL *mysql, const char *query);
void	Com_MySQL_Flush(MYSQL *mysql, const char *file, u_int32_t line);
void	Com_Printfhex(unsigned char *buffer, int len);
int		Com_Stricmp (const char *s1, const char *s2);
u_int8_t Com_CheckAphaNum(char *string);
u_int8_t Com_CheckWBUsername(char *string);
void	Com_ParseString(char *string);
u_int8_t GetSlot(client_t *plane, client_t *client);
u_int8_t CheckSum(u_int8_t *buffer, u_int16_t len);
double	RocketAngle(u_int16_t dist);
double	RocketTime(double angle);
double	WBtoHdg(int16_t angle);
double	AngleTo(int32_t origx, int32_t origy, int32_t destx, int32_t desty);
char	*CopyString (const char *in);
u_int32_t crc32(char *buffer, u_int8_t size);
double	Com_Rad(double angle);
double	Com_Deg(double angle);
int		Com_Atoi(const char *string);
u_int32_t Com_Atou(const char *string);
double	Com_Atof(const char *string);
char	*Com_MyRow(const char *string);
char	*Com_SquadronName(u_int32_t owner);
int		Com_Strncmp(const char *s1, const char *s2, int n);
int		Com_Strcmp(const char *s1, const char *s2);
void	Com_WBntoh(u_int16_t *packetid);
u_int16_t Com_WBhton(u_int16_t packetid);
u_int8_t Com_WB3ntoh(u_int8_t n);
u_int16_t Com_WB3hton(u_int16_t m);
int32_t DistBetween(int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2, int32_t envelope);
double	Com_Log(double number, double base);
double	FloorDiv(double numerator, double denominator);
double	ModRange(double numerator, double denominator);
double	RoundtoBase(double value, double base);
double	WBLongitude(double dAbsLongitude);
double	WBLatitude(double dAbsLatitude);
double	WBAngels(double dAbsAltitude);
double	WBAltMeters(double dAbsAltitude);
char	*WBVSI(double dClimbRate, int fIsBomber);
char	*WBRhumb(double dHeading /* in degrees*/);
double	WBHeading(double dHeading);
int32_t PredictorCorrector32(int32_t *values, u_int8_t degree);
int16_t PredictorCorrector16(int16_t *values, u_int8_t degree);
char	*PadLoc(char *szBuffer, double dLongitude, double dLatitude);

#endif /* COMMON_H_ */
