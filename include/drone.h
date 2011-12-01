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
  * @file		/include/drone.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		drone header file
  */

#ifndef DRONE_H_
#define DRONE_H_

client_t *AddDrone(u_int16_t type, int32_t posx, int32_t posy, int32_t posz, u_int8_t country, u_int16_t plane, client_t *client);
void	RemoveDrone(client_t *drone);
void	DroneVisibleList(client_t *drone);
int		ProcessDrone(client_t *drone);
void	DroneGetTarget(client_t *drone);
void	FireAck(client_t *source, client_t *dest, u_int32_t dist, u_int8_t animate);
void	FireFlak(client_t *source, client_t *dest, u_int32_t dist, u_int8_t animate);
void	DropBomb(int32_t destx, int32_t desty, u_int16_t mun, client_t *client);
void	ThrowBomb(u_int8_t animate, int32_t origx, int32_t origy, int32_t origz, int32_t destx, int32_t desty, int32_t destz, client_t *client);
void	SendDronePos(client_t *drone, client_t *client);
void	SendXBombs(client_t *drone);
u_int8_t HitStructsNear(int32_t x, int32_t y, int32_t z, u_int8_t type, u_int16_t speed, u_int8_t nuke, client_t *client);
void	PFAUDamage(client_t *fau);
void	DroneWings(client_t *client);
u_int32_t NewDroneName(client_t *client);
void	LaunchTanks(u_int8_t fieldfrom, u_int8_t fieldto, u_int8_t country, client_t *client);

#endif /* DRONE_H_ */
