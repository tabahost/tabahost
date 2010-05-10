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

void RunShips_Walk(ship_t *B)
{
	// IncVeloc
	B->Vel.curr = B->Vel.curr + B->Acel.curr;
	if(B->Vel.curr > B->Vel.max)
		B->Vel.curr = B->Vel.max;
	else if(B->Vel.curr < B->Vel.min)
		B->Vel.curr = B->Vel.min;

	B->Position.x = B->Position.x + B->Vel.curr * cos(B->Yaw.curr);
	B->Position.y = B->Position.y + B->Vel.curr * sin(B->Yaw.curr);
}

double RunShips_Angle(double ang)
{
	static float F = 2 * M_PI;

	while(ang >= F)
		ang -= F;
	while(ang < 0)
		ang += F;
	return ang;
}

double RunShips_AngleDef(double ang)
{
	ang = RunShips_Angle(ang);
	if(ang > M_PI)
		ang -= 2*M_PI;
	return ang;
}

void RunShips_Yaw(ship_t *B, ship_t *CV)
{
	double dx, dy;

	dx = B->Target.x - B->Position.x;
	dy = B->Target.y - B->Position.y;

	// ajusta angulo alvo
	if(sqrt(dx*dx+dy*dy) < 100) // ship reached WP
	{
		B->Yaw.target = RunShips_Angle(CV->Yaw.curr);
	}
	else
	{
		if(dy == 0)
		{
			if(dx > 0) // direita
				B->Yaw.target = 0;
			else // esquerda
				B->Yaw.target = M_PI;
		}
		else if(dx == 0)
		{
			if(dy > 0) // baixo
				B->Yaw.target = 1.5*M_PI;
			else // cima
				B->Yaw.target = 0.5*M_PI;
		}
		else
		{
			B->Yaw.target = atan(dy/dx);
			if(dx<0)
				B->Yaw.target = B->Yaw.target + M_PI;
		}
	}

	B->Yaw.target = RunShips_Angle(B->Yaw.target);
	// ajusta velocidade angular
	// B->YawVel = B->Yawtarget-B->Yaw;
	B->YawVel.curr = RunShips_AngleDef(B->Yaw.target - B->Yaw.curr);
	if(B->YawVel.curr > B->YawVel.max)
		B->YawVel.curr = B->YawVel.max;
	else if(B->YawVel.curr < B->YawVel.min)
		B->YawVel.curr = B->YawVel.min;
	// incrementa yaw
	//if(abs(B->Yaw.curr - B->Yaw.target) > (0.01 * M_PI))
		B->Yaw.curr = RunShips_Angle(B->Yaw.curr + B->YawVel.curr);
	// ajusta velocidade
	B->Vel.target = sqrt(dx*dx+dy*dy) / 3;
	dx = B->Vel.max * (1 - MODULUS(RunShips_AngleDef(B->Yaw.target - B->Yaw.curr) / (0.5 * M_PI))); // 1 - x�/90�

	if(dx < B->Vel.min)
		dx = B->Vel.min;

	if(B->Vel.target > dx)
		B->Vel.target = dx;

	B->Acel.curr = B->Vel.target - B->Vel.curr; // controle P
	if(B->Acel.curr > B->Acel.max)
		B->Acel.curr = B->Acel.max;
	else if(B->Acel.curr < B->Acel.min)
		B->Acel.curr = B->Acel.min;
	// B->Vel.curr = B->Vel.target;
}

void RunShips_ReTarget(ship_t *B, ship_t *CV, const double *A)
{
	B->Target.x = CV->Position.x + A[0] * cos(CV->Yaw.curr+A[1]*M_PI);
	B->Target.y = CV->Position.y + A[0] * sin(CV->Yaw.curr+A[1]*M_PI);
}

void RunShips_Prepare(ship_t *ship, ship_t *mainShip, const double *A)
{
	double dx, dy;

	if(!A || ship == mainShip) // main Ship
	{
		dx = ship->Target.x - ship->Position.x;
		dy = ship->Target.y - ship->Position.y;

		if(dy == 0)
		{
			if(dx > 0) // direita
				ship->Yaw.target = 0;
			else // esquerda
				ship->Yaw.target = M_PI;
		}
		else if(dx == 0)
		{
			if(dy > 0) // baixo
				ship->Yaw.target = 1.5*M_PI;
			else // cima
				ship->Yaw.target = 0.5*M_PI;
		}
		else
		{
			ship->Yaw.target = atan(dy/dx);
			if(dx<0)
				ship->Yaw.target = ship->Yaw.target + M_PI;
		}

		ship->Yaw.target = RunShips_Angle(ship->Yaw.target);
		ship->Yaw.curr = ship->Yaw.target;
	}
	else
	{
		ship->Yaw.target = mainShip->Yaw.target;
		ship->Yaw.curr = mainShip->Yaw.curr;
		ship->Target.x = mainShip->Position.x + A[0] * cos(mainShip->Yaw.curr+A[1]*M_PI);
		ship->Target.y = mainShip->Position.y + A[0] * sin(mainShip->Yaw.curr+A[1]*M_PI);
		ship->Position.x = ship->Target.x;
		ship->Position.y = ship->Target.y;
	}
}

void RunShips(u_int8_t group, u_int8_t formation) // Call every 500ms
{
	const double Form[4][6][2] = {
		{{2500,0.25},{2500,-0.25},{4243,0.75},{4243,-0.75},{3000,1},{4000,0}},
		{{600,0.25},{600,-0.25},{848.5281,0.5},{848.5281,-0.5},{600,1},{700,0}},
		{{500,0.5},{500,-0.5},{5,1},{707.1068,0.75},{707.1068,-0.75},{500,0}},
		{{600,0.1667},{600,-0.1667},{600,0.5},{6,-0.5},{600,0.8333},{600,-0.8333}}
	};

	u_int8_t i;
	ship_t *mainShip = NULL;
	ship_t *ship = NULL;
	client_t *near = NULL;

	mainShip = MainShipTarget(group);
	if(!mainShip)
		return;

	// if first steps, configure all ships
	if((arena->frame - mainShip->drone->frame) < 100)
	{
		RunShips_Prepare(mainShip, mainShip, NULL);

		for(i = 0, ship = arena->cvs[group].ships; ship && i < 6; ship = ship->next)
		{
			if(ship == mainShip)
				continue;
			else
				RunShips_Prepare(ship, mainShip, Form[formation][i++]);
		}
	}

	RunShips_Yaw(mainShip, mainShip);
	RunShips_Walk(mainShip);

	if(ProcessDroneShips(mainShip) < 0)
	{
		if(RemoveShip(mainShip))
		{
			Com_Printf(VERBOSE_DEBUG, "RunShips() ProcessDroneShips(error), going to recursive call\n");
			RunShips(group, formation); // Find next mainship and continue. If no mainShip is found, it will return here.
		}
		else // no more ships in array, reset it
			ResetCV(group);
		return;
	}

//	if((near = NearPlane(mainShip->drone, 0, planerangelimit->value)))
//	{
//		Com_Printf(VERBOSE_DEBUG, "Near %s X %u Y %u Z %u\n",
//			near->longnick,
//			near->posxy[0][0],
//			near->posxy[1][0],
//			near->posalt[0]);
//	}

	for(i = 0, ship = arena->cvs[group].ships; ship && i < 6; ship = ship->next)
	{
//		Com_Printf(VERBOSE_DEBUG, "Type %u CV %s X %u Y %u Head %.2f S %.2f S.tar %.2f\n",
//				ship->type,
//				ship->drone->longnick,
//				ship->drone->posxy[0][0],
//				ship->drone->posxy[1][0],
//				Com_Deg(ship->Yaw.curr),
//				ship->Vel.curr,
//				ship->Vel.target);

		if(ship == mainShip)
		{
			continue;
		}

		RunShips_ReTarget(ship, mainShip, Form[formation][i++]);
		RunShips_Yaw(ship, mainShip);
		RunShips_Walk(ship);
		if(ProcessDroneShips(ship) < 0)
		{
			if(!RemoveShip(ship)) // no more ships in array, reset it
				ResetCV(group);
		}
	}
}

int8_t ProcessDroneShips(ship_t *ship)
{
	client_t *drone;

	if(!(drone = ship->drone))
	{
		return -1;
	}

	if (drone->bugged)
	{
		BPrintf(RADIO_YELLOW, "DroneShip %s bugged, and will be removed", drone->longnick);
		RemoveDrone(drone);
		return -1;
	}

	if (drone->status_damage & (STATUS_RWING | STATUS_LWING | STATUS_PILOT | STATUS_CENTERFUSE | STATUS_REARFUSE))
	{
		ScoresEvent(SCORE_KILLED, drone, 0);
		RemoveDrone(drone);
		return -1;
	}

	DroneVisibleList(drone);

	drone->posxy[0][0] = ship->Position.x; // X
	drone->posxy[1][0] = ship->Position.y; // Y
	drone->posalt[0] = 0; // Z
	drone->speedxyz[0][0] = ship->Vel.curr * cos(ship->Yaw.curr); // X
	drone->speedxyz[1][0] = ship->Vel.curr * sin(ship->Yaw.curr); // Y
	drone->speedxyz[2][0] = 0; // Z
	drone->angles[0][0] = 0; // Roll
	drone->angles[1][0] = 0; // Pitch
	drone->angles[2][0] = floor(Com_Deg(ship->Yaw.curr + 4.69493569) * 10); // Yaw
	drone->accelxyz[0][0] = ship->Acel.curr * cos(ship->Yaw.curr); // X
	drone->accelxyz[1][0] = ship->Acel.curr * sin(ship->Yaw.curr); // Y
	drone->accelxyz[2][0] = 0; // Z
	drone->aspeeds[0][0] = 0; // Roll
	drone->aspeeds[1][0] = 0; // Pitch
	drone->aspeeds[2][0] = floor(Com_Deg(ship->YawVel.curr)); // debug * 2); // Yaw - degrees per second?

	drone->offset = -500;
	drone->timer += 500;

	return 0;
}

ship_t *MainShip(u_int8_t group)
{
	ship_t *ship;
	ship_t *cv = NULL;
	ship_t *ca = NULL;
	ship_t *dd = NULL;

	if(!arena->cvs[group].ships)
	{
		Com_Printf(VERBOSE_WARNING, "MainShip(): No ship in group %d\n", group);
		return NULL;
	}
	
	// locate the main ship (first ship of the biggest type)
	for(ship = arena->cvs[group].ships; ship; ship = ship->next)
	{
		switch(ship->type)
		{
			case SHIPTYPE_CV:
				if(!cv)
					cv = ship;
				break;
			case SHIPTYPE_CA:
				if(!ca)
					ca = ship;
				break;
			case SHIPTYPE_DD:
				if(!dd)
					dd = ship;
				break;
			default:
				Com_Printf(VERBOSE_WARNING, "MainShip(): unknown ship type\n");
		}
		
		if(cv)
			break;
	}

	if(cv || ca || dd )
	{
		if(cv)
			ship = cv;
		else if(ca)
			ship = ca;
		else if(dd)
			ship = dd;

		return ship;
	}

	return NULL;
}

ship_t *MainShipTarget(u_int8_t group)
{
	ship_t *ship;

	if(!arena->cvs[group].ships)
	{
		Com_Printf(VERBOSE_WARNING, "MainShipTarget(): No ship in group %d\n", group);
		return NULL;
	}

	if(ship = MainShip(group))
	{
		// update field position
		arena->fields[arena->cvs[group].field].posxyz[0] = ship->Position.x;
		arena->fields[arena->cvs[group].field].posxyz[1] = ship->Position.y;

		// update target waypoint (this may be changed manually or automatically for defensive maneuver)
		ship->Target.x = arena->cvs[group].wp[arena->cvs[group].wpnum][0];
		ship->Target.y = arena->cvs[group].wp[arena->cvs[group].wpnum][1];
		
		// Check waypoint
		if((abs(ship->Target.y - ship->Position.y) > 70) || (abs(ship->Target.x - ship->Position.x) > 70))
			return ship;

		// Next waypoint
		ReadCVWaypoints(group); // reload waypoints from file

		if(arena->cvs[group].threatened)
		{
			arena->cvs[group].threatened = 0;
		}

		arena->cvs[group].wpnum++;

		if(arena->cvs[group].wpnum == arena->cvs[group].wptotal) // reset waypoint index
		{
			arena->cvs[group].wpnum = 1;
		}

		return ship;
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "MainShipTarget(): No main ship found\n");
		return NULL;
	}
}

void RemoveAllShips(u_int8_t group)
{
	ship_t *ship;

	Com_Printf(VERBOSE_DEBUG, "RemoveAllShips() group %u\n", group);

	for(ship = arena->cvs[group].ships; ship; ship = ship->next)
	{
		if(ship->drone)
			RemoveDrone(ship->drone);
		free(ship);
	}

	arena->cvs[group].ships = NULL;
}

void CreateAllShips(u_int8_t group)
{
	u_int8_t i;
	Com_Printf(VERBOSE_DEBUG, "CreateAllShips() group %u - total %u ships\n", group, arena->cvs[group].fleetshipstotal);

	for(i = 0; i < arena->cvs[group].fleetshipstotal; i++)
	{
		Com_Printf(VERBOSE_DEBUG, "Creating Ship %u - group %u\n", arena->cvs[group].fleetships[i], group);
		AddShip(group, arena->cvs[group].fleetships[i], arena->cvs[group].country);
	}
}

/**
 ResetCV

 Reset CV back to starting point (port)
 */

void ResetCV(u_int8_t group)
{
	// CREATE ALL SHIPS AGAIN
	Com_Printf(VERBOSE_DEBUG, "ResetCV() group %u\n", group);
	RemoveAllShips(group);
	CreateAllShips(group);

	arena->cvs[group].threatened = 0;
	arena->cvs[group].outofport = 0;
	arena->cvs[group].wpnum = 1;
	snprintf(arena->cvs[group].logfile, sizeof(arena->cvs[group].logfile), "%s,cv%u,%s,%u", mapname->string, arena->cvs[group].id, GetCountry(arena->cvs[group].country), (u_int32_t)time(NULL));
}

/**
 ReadCVWaypoints

 Read CV waypoints from file
 */

void ReadCVWaypoints(u_int8_t group)
{
	char file[32];
	u_int8_t i;
	FILE *fp;
	char buffer[128];

	Com_Printf(VERBOSE_DEBUG, "ReadCVWaypoints() group %u\n", group);

	snprintf(file, sizeof(file), "./arenas/%s/cv%d.rte", dirname->string, group);

	arena->cvs[group].wptotal = 0;

	if (!(fp = fopen(file, "r")))
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() Cannot open file \"%s\"", file);
		return;
	}

	for (i = 0; fgets(buffer, sizeof(buffer), fp); i++)
	{
		arena->cvs[group].wp[i][0] = Com_Atoi((char *)strtok(buffer, ";"));
		arena->cvs[group].wp[i][1] = Com_Atoi((char *)strtok(NULL, ";"));

		arena->cvs[group].wptotal++;

		if (arena->cvs[group].wptotal >= MAX_WAYPOINTS)
			break;

		memset(buffer, 0, sizeof(buffer));
	}

	if (!arena->cvs[group].wptotal)
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() error reading \"%s\"", file);
	}

	fclose(fp);
}

/**
 ChangeCVRoute

 Change Route of CV, in threathness or by command
 */

void ChangeCVRoute(cvs_t *cv, double angle /*0*/, u_int16_t distance /*10000*/, client_t *client)
{
	int8_t angleoffset = 0;

	Com_Printf(VERBOSE_DEBUG, "WP %d\n", cv->wpnum);

	if(!cv->threatened) // step wpnum back, because its an automatic route change or first manual change
	{
		cv->wpnum--;
		cv->threatened = 1;
	}
	// else // mantain wpnum, because is a manual route change

	if (!client)
	{
		// grab the pathway angle
		// or cvpos? cv->ships->Position.x, cv->ships->Position.y
		angle = AngleTo(cv->wp[cv->wpnum][0], cv->wp[cv->wpnum][1], cv->wp[cv->wpnum+1][0], cv->wp[cv->wpnum+1][1]);

		// change the route based in the pathway angle
		if (rand()%100 < 60) // zigzag
		{
			angleoffset = 45 * Com_Pow(-1, cv->zigzag);

			if (cv->zigzag == 1)
			{
				cv->zigzag = 2;
			}
			else
			{
				cv->zigzag = 1;
			}
		}
		else
		{
			angleoffset = 45 * Com_Pow(-1, rand()%2);
		}

		// check if new waypoint is over land
		if (GetHeightAt(cv->ships->Position.x - (10000 * sin(Com_Rad(angle + angleoffset))), cv->ships->Position.y + (10000 * cos(Com_Rad(angle + angleoffset))))) // WP is over land
		{
			angleoffset *= -1;
		}

		angle += angleoffset;
		distance = 2000;
	}

	Com_Printf(VERBOSE_DEBUG, "WP 0 X %d Y %d\n", cv->wp[0][0], cv->wp[0][1]);
	Com_Printf(VERBOSE_DEBUG, "WP 1 X %d Y %d\n", cv->wp[1][0], cv->wp[1][1]);
	Com_Printf(VERBOSE_DEBUG, "WP 2 X %d Y %d\n", cv->wp[2][0], cv->wp[2][1]);
	Com_Printf(VERBOSE_DEBUG, "WP 3 X %d Y %d\n---------------------\n", cv->wp[3][0], cv->wp[3][1]);

	cv->wp[cv->wpnum][0] = arena->fields[cv->field].posxyz[0] + (distance * sin(Com_Rad(angle)));
	cv->wp[cv->wpnum][1] = arena->fields[cv->field].posxyz[1] + (distance * cos(Com_Rad(angle)));

	Com_Printf(VERBOSE_DEBUG, "WP %d\n", cv->wpnum);
	Com_Printf(VERBOSE_DEBUG, "WP 0 X %d Y %d\n", cv->wp[0][0], cv->wp[0][1]);
	Com_Printf(VERBOSE_DEBUG, "WP 1 X %d Y %d\n", cv->wp[1][0], cv->wp[1][1]);
	Com_Printf(VERBOSE_DEBUG, "WP 2 X %d Y %d\n", cv->wp[2][0], cv->wp[2][1]);
	Com_Printf(VERBOSE_DEBUG, "WP 3 X %d Y %d\n=====================\n", cv->wp[3][0], cv->wp[3][1]);

	if (client)
	{
		PPrintf(client, RADIO_YELLOW, "Waypoint changed to %s", Com_Padloc(cv->wp[cv->wpnum][0], cv->wp[cv->wpnum][1]));
		PPrintf(client, RADIO_YELLOW, "ETA: %s\"", Com_TimeSeconds(distance / cv->ships->Vel.curr));
	}

	// configure to next wpnum be that nearest to cv->wp[lastwp][0]
	// coded at threatened = 0;
}

ship_t *RemoveShip(ship_t *ship)
{
	u_int8_t group;
	ship_t *previous = NULL;

	if(!ship)
	{
		Com_Printf(VERBOSE_WARNING, "RemoveShip() ship == NULL\n");
		return NULL;
	}

	group = ship->group;

	if(arena->cvs[group].ships != ship)
	{
		for(previous = arena->cvs[group].ships; previous; previous = previous->next)
		{
			if(previous->next == ship)
				break;
		}
	}

	if(!previous)
	{
		if(arena->cvs[group].ships == ship) // ship is the first of chain
		{
			arena->cvs[group].ships = ship->next; // fix the chain
		}
		//	else // ship is not in group(!)
	}
	else if(previous->next == ship) // more than one ship in chain (if statement is redundant)
	{
		previous->next = ship->next; // fix the chain
	}

	if(ship->drone)
		RemoveDrone(ship->drone);
	free(ship);

	return arena->cvs[group].ships;
}

ship_t *AddShip(u_int8_t group, u_int8_t plane, u_int8_t country)
{
	ship_t *ship;
	ship_t *link;

	ship = Z_Malloc(sizeof(ship_t));

	if(ship)
	{
		ship->group = group;
		ship->country = country;
		ship->Position.x = arena->cvs[group].wp[0][0];
		ship->Position.y = arena->cvs[group].wp[0][1];
		ship->Target.x = arena->cvs[group].wp[1][0];
		ship->Target.y = arena->cvs[group].wp[1][1];
		ship->Vel.curr = 0;
		ship->Vel.target = ship->Vel.curr;
		ship->Acel.curr = 0;
		ship->Acel.target = ship->Acel.curr;
		ship->Acel.min = -2;
		ship->Acel.max = 4;
		ship->Yaw.curr = Com_Rad(AngleTo(ship->Position.x, ship->Position.y, ship->Target.x, ship->Target.y));
		ship->Yaw.target = ship->Yaw.curr;
		ship->YawVel.curr = 0;
		ship->YawVel.target = 0;
		
		switch(plane)
		{
			// CV
			case SHIP_KAGA: // KAGA 73
				ship->plane = SHIP_KAGA;
			case SHIP_ENTERPRISE: // ENTERPRISE 78
				ship->plane = SHIP_ENTERPRISE;
				ship->type = SHIPTYPE_CV;
				ship->radius = 400; // 800 feet
				ship->Vel.max = 27; // 54 feet per second
				ship->Vel.min = 0.2;
				ship->YawVel.max = 1 * M_PI / 180; // 2º per second (in radians)
				ship->YawVel.min = -ship->YawVel.max;
				break;
			case SHIP_CA: // CA 77
				ship->plane = SHIP_CA;
				ship->type = SHIPTYPE_CA;
				ship->radius = 165; // 330 feet
				ship->Vel.max = 31; // 62 feet per second
				ship->Vel.min = 0.2;
				ship->YawVel.max = 1.3334 * M_PI / 180; // 2.6666º per second (in radians)
				ship->YawVel.min = -ship->YawVel.max;
				break;
			case SHIP_DD: // DD 74
				ship->plane = SHIP_DD;
				ship->type = SHIPTYPE_DD;
				ship->radius = 165; // 330 feet
				ship->Vel.max = 31; // 61 feet per second
				ship->Vel.min = 0.2;
				ship->YawVel.max = 1.5 * M_PI / 180; // 3º per second (in radians)
				ship->YawVel.min = -ship->YawVel.max;
				break;
			default:
				Com_Printf(VERBOSE_WARNING, "AddShip(): unknown ship type\n");
				break;
		}

		ship->drone = AddDrone(DRONE_SHIP, ship->Position.x, ship->Position.y, 0, ship->country, plane, NULL);

		if(ship->drone)
		{
			Com_Printf(VERBOSE_ALWAYS, "Ship %s created. Group %d type %d\n", ship->drone->longnick, group, ship->type);
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "AddShip(): Cannot create drone\n");
			RemoveShip(ship);
			return NULL;
		}
		
		// link the variable at end of queue
		if(!arena->cvs[group].ships)
		{
			arena->cvs[group].ships = ship;
		}
		else
		{
			link = arena->cvs[group].ships;
			while(link->next)
				link = link->next;

			ship->next = NULL;
			link->next = ship;
		}
	}
	else
		Com_Printf(VERBOSE_WARNING, "AddShip(): ship == NULL\n");

	return ship;
}
