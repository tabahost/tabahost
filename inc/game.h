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
  * @file		/include/game.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		game header file
  */

#ifndef GAME_H_
#define GAME_H_

void	CheckArenaRules(void);
void	ProcessMetarWeather(void);
void	ProcessCommands(char *command, client_t *client);
void	SendFileSeq1(const char *file, const char *clifile, client_t *client);
void	SendFileSeq3(client_t *client);
void	SendFileSeq5(u_int16_t seek, client_t *client);
void	SendFileSeq6(u_int8_t *buffer, client_t *client);
void	SendFileSeq7(client_t *client);
int		ProcessPacket(u_int8_t *buffer, u_int16_t len, client_t *client);
void	PingTest(u_int8_t *buffer, client_t *client);
void	PReqBomberList(client_t *client);
void	PEndFlight(u_int8_t *buffer, u_int16_t len, client_t *client);
void	PPlanePosition(u_int8_t *buffer, client_t *client, u_int8_t attached);
void	CheckMaxG(client_t *client);
double	ClientG(client_t *client);
void	PChutePos(u_int8_t *buffer, client_t *client);
void	WB3GunnerUpdate(u_int8_t *buffer, client_t *client);
void	WB3FireSuppression(u_int8_t *buffer, client_t *client);
void	WB3SupressFire(u_int8_t slot, client_t *client);
void	PPlaneStatus(u_int8_t *buffer, client_t *client);
void	WB3ExternalAmmoCnt(u_int8_t *buffer, u_int16_t len, client_t *client);
void	PDropItem(u_int8_t *buffer, u_int8_t len, client_t *client);
void	WB3TonnageOnTarget(u_int8_t *buffer, client_t *client);
void	PRemoveDropItem(u_int8_t *buffer, u_int8_t len, client_t *client);
void	PFlakHit(u_int8_t *buffer, client_t *client);
void	PHitStructure(u_int8_t *buffer, client_t *client);
void	PSetRadioChannel(u_int8_t *buffer, client_t *client);
void	PHardHitStructure(u_int8_t *buffer, client_t *client);
void	PHitPlane(u_int8_t *buffer, client_t *client);
void	WB3FuelConsumed(u_int8_t *buffer, client_t *client);
void	WB3DelayedFuse(u_int8_t *buffer, client_t *client);
void	SendPings(u_int8_t hits, u_int8_t type, client_t *client);
void	PHardHitPlane(u_int8_t *buffer, client_t *client);
const char	*GetHitSite(u_int8_t id);
const char	*GetSmallHitSite(u_int8_t id);
munition_t *GetMunition(u_int8_t id);
u_int16_t AddPlaneDamage(int8_t place, u_int16_t he, u_int16_t ap, char *phe, char *pap, client_t *client);
double	RebuildTime(building_t *building);
u_int8_t AddBuildingDamage(building_t *building, u_int16_t he, u_int16_t ap, client_t *client);
void	SendFieldStatus(u_int16_t field, client_t *client);
void	SetBuildingStatus(building_t *building, u_int8_t status, client_t *client);
void	SetPlaneDamage(u_int16_t plane, client_t *client);
void	PFireMG(u_int8_t *buffer, u_int8_t len, client_t *client);
void	POttoFiring(u_int8_t *buffer, u_int8_t len, client_t *client);
void	SendForceStatus(u_int32_t status_damage, u_int32_t status_status, client_t *client);
void	SendPlaneStatus(client_t *plane, client_t *client);
void	WB3DotCommand(client_t *client, const char *fmt, ...);
void	PRadioMessage(u_int8_t *buffer, client_t *client);
void	PrintRadioMessage(u_int32_t msgto, u_int32_t msgfrom, char *message, u_int8_t msgsize, client_t *client);
void	PNewNick(u_int8_t *buffer, client_t *client);
int32_t	SendCopyright(client_t *client);
void	UpdateIngameClients(u_int8_t attr);
const char	*GetCountry(u_int8_t country);
const char	*GetRanking(u_int8_t ranking);
int32_t	SendArenaNames(client_t *client);
void	SendPlayersNames(client_t *client);
void	SendIdle(client_t *client);
void	SendPlayersNear(client_t *client);
void	SendOttoParams(client_t *client);
void	SendOttoParams2(client_t *client);
void	SendLastConfig(client_t *client);
void	AddRemovePlaneScreen(client_t *plane, client_t *client, u_int8_t remove);
void	SendScreenUpdates(client_t *client);
void	SendDeckUpdates(client_t *client);
int		CanHear(client_t *client1, client_t *client2, u_int32_t msgto);
void	SendArenaRules(client_t *client);
void	WB3SendGruntConfig(client_t *client);
void	WB3SendArenaFlags3(client_t *client);
void	WB3RandomSeed(client_t *client);
void	WB3ConfigFM(client_t *client);
void	WB3ArenaConfig2(client_t *client);
void	WB3NWAttachSlot(client_t *client);
void	PRadioCommand(char *message, u_int8_t size, client_t *client);
void	PFileCheck(u_int8_t *buffer, client_t *client);
void	CheckCockpitCRC(char *path, u_int32_t crc, client_t *client);
void	SendExecutablesCheck(u_int32_t exemisc, client_t *client);
void	SendFieldsCountries(client_t *client);
int8_t	FirstFieldCountry(u_int8_t country);
void	SendGunnerStatusChange(client_t *gunner, u_int16_t status, client_t *client);
void	SendAttachList(u_int8_t *packet, client_t *client);
void	PCurrentView(u_int8_t *buffer, client_t *client);
void	PSquadLookup(u_int8_t *buffer, client_t *client);
u_int16_t GunPos(u_int16_t pos, u_int8_t reverse);
void	WB3RequestStartFly(u_int8_t *buffer, client_t *client);
void	WB3RequestMannedAck(u_int8_t *buffer, client_t *client);
void	PHostVar(u_int8_t *buffer, client_t *client);
void	THAIWatchDog(u_int8_t *buffer, client_t *client);
void	PRequestGunner(u_int8_t *buffer, client_t *client);
void	PAcceptGunner(u_int8_t *buffer, client_t *client);
void	PSwitchOttoPos(u_int8_t *buffer, client_t *client);
void	PClientMedals(u_int8_t *buffer, client_t *client);
void	PSquadInfo(char *nick, client_t *client);
double	CalcDamage(u_int32_t mass, u_int16_t vel);
void	Kamikase(client_t *client);
u_int32_t GetFactoryReupTime(u_int8_t country);
u_int32_t GetRPSLag(u_int8_t country);

#endif /* GAME_H_ */
