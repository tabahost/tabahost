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
  * @file		/include/arena.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		arena header file
  */

#ifndef ARENA_H_
#define ARENA_H_

#include "shared.h"

building_t	*GetBuilding(u_int16_t id);
int32_t	GetFieldRadius(u_int8_t fieldtype);
const char *GetFieldType(u_int8_t type);
const char *GetBuildingType(u_int16_t type);
void	LoadArenaStatus(const char *filename, client_t *client, u_int8_t reset);
void	SaveArenaStatus(const char *filename, client_t *client);
void	SaveWebsiteData(void);
void	LoadPlanesPool(const char *filename, client_t *client);
void	SavePlanesPool(const char *filename, client_t *client);
u_int32_t GetBuildingArmor(u_int8_t type, client_t *client);
u_int32_t GetBuildingAPstop(u_int8_t type, client_t *client);
u_int32_t GetBuildingImmunity(u_int8_t type, client_t *client);
void	SendMapDots(void);
u_int8_t SeeEnemyDot(client_t *client, u_int8_t country);
void	ClearMapDots(client_t *client);
void	ListWaypoints(client_t *client);
const char *GetPlaneName(u_int16_t plane);
const char *GetSmallPlaneName(u_int16_t plane);
const char *GetPlaneDir(u_int16_t plane);
void	UpdateRPS(u_int16_t minutes);
void	SendRPS(client_t *client);
void	WB3SendAcks(client_t *client);
void	AddBomb(u_int16_t id, int32_t destx, int32_t desty, int32_t destz, u_int8_t type, int16_t speed, u_int32_t timer, client_t *client);
void	LoadRPS(const char *path, client_t *client);
void	SaveRPS(const char *path, client_t *client);
void	ShowRPS(client_t *client);
void	LoadMapcycle(const char *path, client_t *client);
int8_t	IsFighter(client_t *client, ...);
int8_t	IsBomber(client_t *client, ...);
int8_t	IsCargo(client_t *client, ...);
int8_t	IsGround(client_t *client, ...);
int8_t	IsShip(client_t *client, ...);
int8_t	HaveGunner(u_int16_t plane);
void	LoadAmmo(client_t *client);
void	LoadDamageModel(client_t *client);
void	SaveDamageModel(client_t *client, char *row);
//void	CheckBoatDamage(building_t *building, client_t *client);
void	CaptureField(u_int8_t field, client_t *client);
u_int16_t TimetoNextArena(void);
void	InitArena(void);
void	ChangeArena(char *map, client_t *client);
void	CalcTimemultBasedOnTime(void);
void	NewWar(void);
int32_t NearestField(int32_t posx, int32_t posy, u_int8_t country, u_int8_t city, u_int8_t cvs, u_int32_t *pdist);
void	ReducePlanes(u_int8_t field);
void	IncreaseAcksReup(u_int8_t field);
u_int8_t IsVitalBuilding(u_int8_t building, u_int8_t notot);
u_int8_t IsVitalBuilding(building_t *building, u_int8_t notot);
u_int8_t GetFieldParas(u_int8_t type);
double	GetTonnageToClose(u_int8_t field);
u_int8_t Alt2Index(int32_t alt);
void	WB3MapTopography(client_t *client);
void	WB3Mapper(client_t *client);
u_int32_t GetHeightAt(int32_t x, int32_t y);
u_int8_t LoadEarthMap(char *FileName);
u_int8_t SaveEarthMap(char *FileName);
int32_t IsVisible(int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2);
void	NoopArenalist(void);
void	AddFieldDamage(u_int8_t field, u_int32_t damage, client_t *client);
int8_t	AddBomber(u_int8_t field, client_t *client);
void	SetBFieldType(building_t *buildings, u_int16_t type);
void	CalcFactoryBuildings(void);
void	DebiteFactoryBuildings(city_t *city);
void	CrediteFactoryBuildings(city_t *city);
void	DebugArena(const char *file, u_int32_t line);

#endif /* ARENA_H_ */
