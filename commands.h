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
  * @file		/include/commands.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		commands header file
  */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "shared.h"

void	Cmd_LoadBatch(client_t *client);
void	Cmd_LoadConfig(const char *filename, client_t *client, bool verbose = true);
void	Cmd_Ros(client_t *client);
void	Cmd_Ammo(client_t *client, u_int8_t arg, char *arg2);
void	Cmd_Saveammo(client_t *client, char *row);
void	Cmd_VarList(client_t *client, char *string);
void	Cmd_Move(char *field, int country, client_t *client);
void	Cmd_Plane(u_int16_t planenumber, client_t *client);
void	Cmd_Fuel(int8_t fuel, client_t *client);
void	Cmd_Conv(u_int16_t distance, client_t *client);
void	Cmd_Ord(u_int8_t ord, client_t *client);
void	Cmd_Easy(u_int8_t easy, client_t *client);
void	Cmd_TK(char *tkiller, u_int8_t newvalue, client_t *client);
u_int8_t Cmd_Fly(u_int16_t position, client_t *client);
u_int8_t Cmd_Capt(u_int16_t field, u_int8_t country, client_t *client);
void	Cmd_White(char *user, u_int8_t white, client_t *client);
void	Cmd_Chmod(char *user, int8_t mod, client_t *client);
void	Cmd_Part(char *argv[], u_int8_t argc, client_t *client);
void	Cmd_Decl(char *argv[], u_int8_t argc, client_t *client);
void	Cmd_Pingtest(u_int16_t frame, client_t *client);
void	Cmd_Undecl(u_int16_t id, client_t *client);
void	Cmd_Time(u_int16_t time, char *mult, client_t *client);
void	Cmd_Date(u_int16_t year, u_int8_t month, u_int8_t day, client_t *client);
void	Cmd_Field(u_int8_t field, client_t *client);
void	Cmd_City(u_int8_t city, client_t *client);
void	Cmd_StartDrone(u_int32_t field, u_int32_t plane, double angle, client_t *client);
void	Cmd_StartFau(u_int32_t dist, double angle, u_int8_t attached, client_t *client);
void	Cmd_Say(char *argv[], u_int8_t argc, client_t *client);
void	Cmd_Seta(char *field, int8_t country, int16_t plane, int8_t amount);
void	Cmd_Show(client_t *client);
void	Cmd_Score(char *player, client_t *client);
void	Cmd_Clear(client_t *client);
void	Cmd_Whoare(u_int8_t radio, client_t *client);
void	Cmd_Invite(char *nick, client_t *client);
void	Cmd_Jsquad(client_t *client);
void	Cmd_Name(char *name, client_t *client);
void	Cmd_Slogan(char *motto, client_t *client);
void	Cmd_Remove(char *nick, client_t *client);
void	Cmd_Withdraw(client_t *client);
void	Cmd_Disband(client_t *client);
void	Cmd_Psq(char *nick, u_int8_t attr, client_t *client);
void	Cmd_Hls(client_t *client);
void	Cmd_Listavail(u_int8_t field, client_t *client);
void	Cmd_Wings(u_int8_t mode, client_t *client);
void	Cmd_Hmack(client_t *client, const char *command, u_int8_t tank);
void	Cmd_Commandos(client_t *client);
void	Cmd_Info(char *nick, client_t *client);
void	Cmd_Ban(char *nick, u_int8_t newvalue, client_t *client);
void	Cmd_Gclear(char *nick, client_t *client);
void	Cmd_Shanghai(u_int8_t *buffer, client_t *client);
void	Cmd_View(client_t *victim, client_t *client);
void	Cmd_Minen(u_int32_t dist, double angle, client_t *client);
void	Cmd_Tanks(char *field, client_t *client);
void	Cmd_Pos(u_int32_t freq, client_t *client, client_t *peek);
void	Cmd_Thanks(char *argv[], u_int8_t argc, client_t *client);
void	Cmd_Restore(u_int8_t field, client_t *client);
void	Cmd_Destroy(u_int8_t field, int32_t time, client_t *client);
void	Cmd_ChangeCVRoute(double angle, u_int16_t distance, client_t *client);
void	Cmd_UTC(client_t *client);
void	Cmd_Lives(char *nick, int8_t amount, client_t *client);
void	Cmd_Reload(client_t *client);
//void	Cmd_CheckWaypoints(client_t *client);
void	Cmd_Flare(client_t *client);
void	Cmd_Rocket(int32_t y, double angle, double angle2, client_t *client); // debug
void	Cmd_Sink(u_int16_t a, u_int16_t b, client_t *client); // debug
void	Cmd_CheckBuildings(client_t *client); // debug

#endif /* COMMANDS_H_ */
