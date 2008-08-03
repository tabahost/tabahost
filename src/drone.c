/***
 *  Copyright (C) 2004-2008 Francisco Bischoff
 *  Copyright (C) 2006 MaxMind LLC
 *  Copyright (C) 2000-2003 MySQL AB
 * 
 *  This file is part of Tabajara Host.
 *
 *  Tabajara Host is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tabajara Host is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Tabajara Host.  If not, see <http://www.gnu.org/licenses/agpl.html>.
 * 
 ***/

#include "shared.h"

/*************
AddDrone

Add a new drone in clients array
*************/

client_t *AddDrone(u_int16_t type, int32_t posx, int32_t posy, int32_t posz, u_int8_t country, u_int16_t plane, client_t *client)
{
	int i = 0, j = 0;
	u_int32_t shortnick;

	if(client && !((client->attr & FLAG_ADMIN) && (type & (DRONE_TANK1 | DRONE_TANK2 | DRONE_AAA | DRONE_KATY))))
	{
		for(i = 0; i < MAX_RELATED; i++)
		{
			if(client->related[i] && (client->related[i]->drone & type))	//	if(client->related[i]->drone & (DRONE_FAU | DRONE_WINGS2 | DRONE_HMACK))
			{
				PPrintf(client, RADIO_YELLOW, "You have such kind of drone already flying");
				return NULL;
			}
		}

		for(j = 0; j < MAX_RELATED; j++) // select an empty slot
		{
			if(!client->related[j])
				break;
		}

		if(j == MAX_RELATED)
		{
			PPrintf(client, RADIO_YELLOW, "Can't create a new drone for you now");
		return NULL;
		}
	}

	for(i = 0; i < maxentities->value; i++)
	{
		if(!clients[i].inuse)
		{
			arena->numdrones++;

			if(client)
				shortnick = ascii2wbnick(client->longnick, 1);
			else
				shortnick = NewDroneName(client);
				
			if(client && !((client->attr & FLAG_ADMIN) && (type & (DRONE_TANK1 | DRONE_TANK2 | DRONE_AAA | DRONE_KATY))))
			{
				client->related[j] = &clients[i]; // start relationship
				clients[i].related[0] = client;
			}
			else
				clients[i].related[0] = NULL;

			clients[i].inuse = 1;
			clients[i].ready = 1;
			clients[i].drone = type;
			clients[i].country = country;
			clients[i].infly = 1;
			switch(type)
			{
				case DRONE_FAU:
					clients[i].plane = plane;
					clients[i].posxy[0][0] = posx;
					clients[i].posxy[1][0] = posy;
					clients[i].shortnick = shortnick;
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				case DRONE_WINGS1:
					clients[i].droneformation = client->droneformation;
					clients[i].droneformchanged = 0;
					clients[i].dronedistance = 1;
					clients[i].plane = plane;
					clients[i].posalt[0] = posz;
					//clients[i].status1 = client->status1;
					clients[i].status2 = client->status2;
					clients[i].shortnick = NewDroneName(client);
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				case DRONE_WINGS2:
					clients[i].droneformation = client->droneformation;
					clients[i].droneformchanged = 0;
					clients[i].dronedistance = 1;
					clients[i].plane = plane;
					clients[i].posalt[0] = posz;
					//clients[i].status1 = client->status1;
					clients[i].status2 = client->status2;
					clients[i].shortnick = NewDroneName(client);
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				case DRONE_TANK1:
				case DRONE_TANK2:
				case DRONE_AAA:
				case DRONE_KATY:
					if(type & DRONE_TANK1)
					{
						clients[i].posxy[0][0] = posx;
						clients[i].posxy[1][0] = posy;
					}
					else
					{
						clients[i].posxy[0][0] = posx + (Com_Pow(-1, rand()%2) * ((rand()%50) + 100));
						clients[i].posxy[1][0] = posy + (Com_Pow(-1, rand()%2) * ((rand()%50) + 100));
					}

					if(type & DRONE_KATY)
					{
						clients[i].plane = plane;
						clients[i].dronetimer = DRONE_KATYSALVO * 3;
						clients[i].conv = 0;
					}
					else
					{
						clients[i].plane = plane;
						clients[i].dronetimer = 40; // 60
					}
					clients[i].ord = 0;
					clients[i].posalt[0] = 0;
					clients[i].status1 = clients[i].status2 = 0;
					clients[i].shortnick = NewDroneName(client);
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				case DRONE_HTANK:
				case DRONE_HMACK:
					if(type & DRONE_HTANK)
						clients[i].plane = plane;
					else
						clients[i].plane = plane;
					clients[i].posxy[0][0] = posx + (Com_Pow(-1, rand()%2) * (rand()%1000));
					clients[i].posxy[1][0] = posy + (Com_Pow(-1, rand()%2) * (rand()%1000));
					clients[i].posalt[0] = posz;
					clients[i].status1 = 0;
					clients[i].shortnick = shortnick;
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				case DRONE_EJECTED:
					clients[i].plane = 0x3D;
					clients[i].posxy[0][0] = posx;
					clients[i].posxy[1][0] = posy;
					clients[i].posalt[0] = posz;
					clients[i].country = country;
					clients[i].status1 = 0;
					clients[i].status2 = 0;
					clients[i].shortnick = shortnick;
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				case DRONE_COMMANDOS:
					clients[i].plane = plane;
					clients[i].posxy[0][0] = posx;
					clients[i].posxy[1][0] = posy;
					clients[i].posalt[0] = posz;
//					clients[i].atradar = 0x10; // not seen
					clients[i].country = country;
					clients[i].status1 = 0;
					clients[i].shortnick = shortnick;
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					clients[i].dronetimer = 360*100;
					clients[i].ready = 0;
					clients[i].countrytime = 100;
					break;
				case DRONE_DEBUG:
					clients[i].plane = plane;
					clients[i].posxy[0][0] = posx;
					clients[i].posxy[1][0] = posy;
					clients[i].posalt[0] = posz + 100;
//					clients[i].atradar = 0;
					clients[i].country = country;
					clients[i].shortnick = shortnick;
					strcpy(clients[i].longnick, wbnick2ascii(clients[i].shortnick));
					break;
				default:
					clients[i].plane = 1;
			}
			clients[i].frame = arena->frame + (rand()%1000);

			clients[i].offset = clients[i].timer - arena->time;
			clients[i].timer = arena->time;

//			if(!(type & DRONE_EJECTED))
				SetPlaneDamage(clients[i].plane, &clients[i]);
				
			break;
		}
	}

	if(i == maxentities->value)
	{
		PPrintf(client, RADIO_YELLOW, "Server reached max number of drones in arena");
		return NULL;
	}
	else
	{
		return &clients[i];
	}
}


/*************
RemoveDrone

Revove drone from world
*************/

void RemoveDrone(client_t *drone)
{
	u_int8_t i; //, slot;

	if(!drone || !drone->inuse)
		return;

	drone->inuse = 0;
	arena->numdrones--;

	for(i = 0; i < MAX_RELATED; i++)
	{
		if(drone->related[0])
		{
			if(drone->related[0]->related[i] == drone)
			{
				if(drone->related[0]->attached == drone)
				{
					ForceEndFlight(FALSE, drone->related[0]);
				}
				drone->related[0]->related[i] = 0;
				break;
			}
		}
		else
			break;
	}

	if(i == MAX_RELATED)
		Com_Printf("Server didnt find relationship between %s and his drone\n", drone->related[0]?drone->related[0]->longnick:"(null)");

//	slot = drone->slot;
	memset(drone, 0, sizeof(client_t)); //
//	drone->slot = slot;
}

/*************
DroneVisibleList

Makes drone visible lists like in SendPlayersNear()
*************/

void DroneVisibleList(client_t *drone)
{
	register int16_t mid, i, j, k;
	client_t *carray[MAX_ENTITIES];
	client_t *temp;

	memset(carray, 0, sizeof(carray));
//	for(i = 0; i < maxentities->value; i++)
//		carray[i] = NULL;

	/* calculate distance */
	for(k = i = 0; i < maxentities->value; i++)
	{
		if(drone != &clients[i] && clients[i].inuse && clients[i].infly && !clients[i].attached)
		{
			if((clients[i].reldist = DistBetween(drone->posxy[0][0], drone->posxy[1][0], drone->posalt[0], clients[i].posxy[0][0], clients[i].posxy[1][0], clients[i].posalt[0], MAX_INT16)) >= 0)
				carray[k++] = &clients[i];
		}
	}

	/* sort by dist*/
	for(mid = k / 2; mid > 0; mid /= 2)
	{
		for(i = mid; i < k; i++)
		{
			for(j = i - mid; j >= 0; j -= mid)
			{
				if(carray[j]->reldist <= carray[j+mid]->reldist)	/* compare */
					break;
				temp = carray[j];				/* permute */
				carray[j] = carray[j+mid];
				carray[j+mid] = temp;
			}
		}
	}

	for(i = 0; i < MAX_SCREEN; i++)
	{
		drone->visible[i].client = carray[i];
	}
}


/*************
ProcessDrone

Make drones live!
*************/

int ProcessDrone(client_t *drone)
{
	int16_t angle = 0, offset = 0;
	double ang;
	u_int16_t i = 0, k = 0;
	int16_t j;
	int32_t x, y;
	u_int32_t dist;
	client_t *near;
	building_t *buildings;

	if(drone->bugged)
	{
		BPrintf(RADIO_YELLOW, "%s bugged, and will be kicked", drone->longnick);
		return -1;
	}

	DroneVisibleList(drone);

	switch(drone->drone)
	{
		case DRONE_FAU: // FAU
			if(drone->dronetimer)
			{
				if(!((arena->frame - drone->frame) % 50))
				{
					drone->posxy[0][0] += drone->speedxyz[0][0]/2;
					drone->posxy[1][0] += drone->speedxyz[1][0]/2;
					drone->posalt[0] += drone->speedxyz[2][0]/2;
				}

				if(drone->posalt[0] >= 3000 && drone->speedxyz[2][0] > 0)
				{
					drone->speedxyz[2][0] = 0;
					drone->angles[0][0] = 0;
				}

				if(drone->dronetimer == 3000 && !drone->speedxyz[2][0])
				{
					drone->speedxyz[2][0] = -100;
					drone->angles[0][0] = -80;
				}

				drone->dronetimer--;

				if(!((arena->frame - drone->frame) % 50) && drone->related[0] && drone->related[0]->attached == drone && !drone->related[0]->chute)
				{
					drone->related[0]->posxy[0][0] = drone->posxy[0][0];
					drone->related[0]->posxy[1][0] = drone->posxy[1][0];
					drone->related[0]->posalt[0] = drone->posalt[0];
					SendDronePos(drone, drone->related[0]);
				}
			}
			else
			{
				SendXBombs(drone);
				PFAUDamage(drone);
				RemoveDrone(drone);
			}
			break;
		case DRONE_WINGS1:
			if(!((arena->frame - drone->frame) % 10))
			{
				if(drone->related[0])
				{
					angle = WBtoHdg(drone->related[0]->angles[2][0]) * 10;

/*
					if(drone->related[0]->angles[2][0] < 0)
						angle = drone->related[0]->angles[2][0] + 3600;
					else
						angle = drone->related[0]->angles[2][0];
*/

					switch (drone->droneformation)
					{
						case FORMATION_VWING:
							offset = 1350;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}
							break;
						case FORMATION_LABREAST:
							offset = 2700;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}
							break;
						case FORMATION_LASTERN:
							offset = 1800;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}
							break;
						case FORMATION_ECHELON:
							offset = 2250;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}
							break;
					}

					if((angle += offset) >= 3600)
						angle %= 3600;
					angle /= 10;

					if(wb3->value)
						drone->posxy[0][0] = drone->related[0]->posxy[0][0] + (DRONE_DIST * sin(Com_Rad(angle)));
					else
						drone->posxy[0][0] = drone->related[0]->posxy[0][0] - (DRONE_DIST * sin(Com_Rad(angle)));
					drone->posxy[1][0] = drone->related[0]->posxy[1][0] + (DRONE_DIST * cos(Com_Rad(angle)));

					drone->posalt[0] = drone->related[0]->posalt[0];

					if((drone->status2 != drone->related[0]->status2))// || (drone->status1 != drone->related[0]->status1))
					{
						//drone->status1 = drone->related[0]->status1;
						drone->status2 = drone->related[0]->status2;
						PPlaneStatus(NULL, drone);
					}
//					drone->atradar = drone->related[0]->atradar;
					drone->speedxyz[0][0] = drone->related[0]->speedxyz[0][0];
					drone->speedxyz[1][0] = drone->related[0]->speedxyz[1][0];
					drone->speedxyz[2][0] = drone->related[0]->speedxyz[2][0];
					drone->accelxyz[0][0] = drone->related[0]->accelxyz[0][0];
					drone->accelxyz[1][0] = drone->related[0]->accelxyz[1][0];
					drone->accelxyz[2][0] = drone->related[0]->accelxyz[2][0];
					drone->angles[0][0] = drone->related[0]->angles[0][0];
					drone->angles[1][0] = drone->related[0]->angles[1][0];
					drone->angles[2][0] = drone->related[0]->angles[2][0];
					drone->aspeeds[0][0] = drone->related[0]->aspeeds[0][0];
					drone->aspeeds[1][0] = drone->related[0]->aspeeds[1][0];
					drone->aspeeds[2][0] = drone->related[0]->aspeeds[2][0];
				}
				else
					RemoveDrone(drone);
			}
			break;
		case DRONE_WINGS2:
			if(!((arena->frame - drone->frame) % 10))
			{
				if(drone->related[0])
				{
					angle = WBtoHdg(drone->related[0]->angles[2][0]) * 10;

					/*
					if(drone->related[0]->angles[2] < 0)
						angle = drone->related[0]->angles[2][0] + 3600;
					else
						angle = drone->related[0]->angles[2][0];
					*/

					switch (drone->droneformation)
					{
						case FORMATION_VWING:
						case FORMATION_ECHELON:
							offset = 2250;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}

							break;
						case FORMATION_LABREAST:
							offset = 2700;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}

							break;
						case FORMATION_LASTERN:
							offset = 1800;

							if(drone->droneformchanged)
							{
								offset += drone->droneformchanged;

								if(drone->droneformchanged < 0)
								{
									if(drone->droneformchanged > -10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged += 10;
								}
								else
								{
									if(drone->droneformchanged < 10)
										drone->droneformchanged = 0;
									else
										drone->droneformchanged -= 10;
								}
							}

							break;
					}

					if((angle += offset) >= 3600)
						angle %= 3600;
					angle /= 10;

					if(drone->droneformation == FORMATION_VWING)
					{
						if(drone->dronedistance > 1)
						{
							drone->dronedistance -= 0.1;
						}
					}
					else
					{
						if(drone->dronedistance < 2)
						{
							drone->dronedistance += 0.1;
						}
					}

					if(wb3->value)
						drone->posxy[0][0] = drone->related[0]->posxy[0][0] + ((DRONE_DIST * drone->dronedistance) * sin(Com_Rad(angle)));
					else
						drone->posxy[0][0] = drone->related[0]->posxy[0][0] - ((DRONE_DIST * drone->dronedistance) * sin(Com_Rad(angle)));
					drone->posxy[1][0] = drone->related[0]->posxy[1][0] + ((DRONE_DIST * drone->dronedistance) * cos(Com_Rad(angle)));
					drone->posalt[0] = drone->related[0]->posalt[0];

					if((drone->status2 != drone->related[0]->status2))// || (drone->status1 != drone->related[0]->status1))
					{
						//drone->status1 = drone->related[0]->status1;
						drone->status2 = drone->related[0]->status2;
						PPlaneStatus(NULL, drone);
					}
//					drone->atradar = drone->related[0]->atradar;
					drone->speedxyz[0][0] = drone->related[0]->speedxyz[0][0];
					drone->speedxyz[1][0] = drone->related[0]->speedxyz[1][0];
					drone->speedxyz[2][0] = drone->related[0]->speedxyz[2][0];
					drone->accelxyz[0][0] = drone->related[0]->accelxyz[0][0];
					drone->accelxyz[1][0] = drone->related[0]->accelxyz[1][0];
					drone->accelxyz[2][0] = drone->related[0]->accelxyz[2][0];
					drone->angles[0][0] = drone->related[0]->angles[0][0];
					drone->angles[1][0] = drone->related[0]->angles[1][0];
					drone->angles[2][0] = drone->related[0]->angles[2][0];
					drone->aspeeds[0][0] = drone->related[0]->aspeeds[0][0];
					drone->aspeeds[1][0] = drone->related[0]->aspeeds[1][0];
					drone->aspeeds[2][0] = drone->related[0]->aspeeds[2][0];
				}
				else
					RemoveDrone(drone);
			}
			break;
		case DRONE_TANK1:
		case DRONE_TANK2:
		case DRONE_AAA:
		case DRONE_KATY:

			near = NULL;
			ang = 0;

			if(arena->fields[drone->dronefield].country == drone->country)
				drone->dronetimer = 0;

			if(drone->dronetimer)
			{
				j = NearestField(drone->posxy[0][0], drone->posxy[1][0], drone->country, TRUE, TRUE, &dist);

				if(!((arena->frame - drone->frame) % 50))
				{
					drone->posxy[0][0] += drone->speedxyz[0][0]/2;
					drone->posxy[1][0] += drone->speedxyz[1][0]/2;
					drone->posalt[0] = GetHeightAt(drone->posxy[0][0], drone->posxy[1][0]);

					if(drone->drone & DRONE_KATY)
					{
						if(drone->conv) // conv == katy salvo
						{
							ThrowBomb(FALSE, drone->posxy[0][0], drone->posxy[1][0], drone->posalt[0] + 13, drone->aim[0], drone->aim[1], drone->aim[2], drone);
							drone->dronetimer--;
							drone->conv--;
						}
					}
				}

				if(!((arena->frame - drone->frame + rand()%500) % 500) && !drone->conv /*just for katy*/)
				{
					// adjust route towards field tower
					ang = AngleTo(drone->posxy[0][0], drone->posxy[1][0], arena->fields[drone->dronefield].posxyz[0], arena->fields[drone->dronefield].posxyz[1]);

					near = NearPlane(drone, drone->country, enemyidlim->value);
				}

				if(!((arena->frame - drone->frame) % 1000) && (drone->drone & (DRONE_TANK1 | DRONE_TANK2)))
				{
					k = 0;		
				
					if((j >= 0) && (dist < tanksrange->value))
					{
						// use dronetimer to count maxshots than remove drones

						if(j < fields->value)
						{
							buildings = arena->fields[j].buildings;
						}
						else
						{
							buildings = arena->cities[j - (int16_t)fields->value].buildings;
						}

						for(i = 0; i < MAX_BUILDINGS; i++)
						{
							if(!buildings[i].field)
							{
								break;
							}
							else if(!buildings[i].status && IsVitalBuilding(&(buildings[i])))
							{
								x = drone->posxy[0][0] - buildings[i].posx;
								y = drone->posxy[1][0] - buildings[i].posy;

								if((MODULUS(x) < tanksrange->value) && (MODULUS(y) < tanksrange->value))
								{
									if((MODULUS(x) > 200) && (MODULUS(y) > 200))
									{
										ThrowBomb(FALSE, drone->posxy[0][0], drone->posxy[1][0], drone->posalt[0] + 2, buildings[i].posx, buildings[i].posy, 0, drone);
										drone->dronetimer--;
										break;
									}
									else
									{
										k++;
									}
								}
							}
						}
					}
					else if(near && (near->plane == 0x55 || near->plane == 0x65))
					{
						ThrowBomb(FALSE, drone->posxy[0][0], drone->posxy[1][0], drone->posalt[0] + 2, near->posxy[0][0], near->posxy[1][0], near->posxy[2][0], drone);
					}
				}

				if(ang)
				{
					if(drone->dronefield == j && (drone->drone & (DRONE_TANK1 | DRONE_TANK2)))  // tanks reached the dest field
					{
						if(dist < tanksrange->value && i < MAX_BUILDINGS)
						{
							ang += 75;
							if(ang > 360)
							{
								ang -= 360;
							}
							
							if(!drone->ord)
							{
								drone->ord = 1;

								PPrintf(drone->related[0], RADIO_DARKGREEN, "Tank %s reached f%d", drone->longnick, drone->dronefield+1, dist);
								Com_Printf("DEBUG: Tank %s reached f%d\n", drone->longnick, drone->dronefield+1, dist);
							}
						}
						else if(k) // no target far but near
						{
							ang += 180;
							if(ang > 360)
							{
								ang -= 360;
							}
						}
					}

					// Evasive tick
					if(near) // there is a enemy plane near
					{
						if(drone->drone & DRONE_AAA)
							FireAck(drone, near, 0);
						
						drone->threatened = 1; // just for katy

						if(drone->ord /*just for katy*/)
							ang += 80;

						ang += 30 * Com_Pow(-1, rand()%2);

						if(ang > 360)
						{
							ang -= 360;
						}
					}
					else
					{
						drone->threatened = 0; // just for katy
					}
				}

				if(drone->drone & DRONE_KATY)
				{
					if(!((arena->frame - drone->frame) % 3000))
					{
						if(!drone->threatened)
						{
							if(j > 0 && j == drone->dronefield)
							{
								if(dist < katyrange->value)
								{
									if(!drone->ord)
									{
										drone->ord = 1;
										PPrintf(drone->related[0], RADIO_DARKGREEN, "Katyusha %s reached f%d", drone->longnick, drone->dronefield+1, dist);
										Com_Printf("DEBUG: Katyusha %s reached f%d\n", drone->longnick, drone->dronefield+1, dist);
									}

									if(j < fields->value)
									{
										buildings = arena->fields[j].buildings;
									}
									else
									{
										buildings = arena->cities[j - (int16_t)fields->value].buildings;
									}

									for(i = 0; i < MAX_BUILDINGS; i++)
									{
										if(!buildings[i].field)
										{
											break;
										}
										else if(!buildings[i].status && (buildings[i].type == BUILD_RADAR || buildings[i].type == BUILD_RADIOHUT || buildings[i].type == BUILD_ANTENNA))
										{
											drone->aim[0] = buildings[i].posx;
											drone->aim[1] = buildings[i].posy;
											drone->aim[2] = GetHeightAt(buildings[i].posx, buildings[i].posy);

											drone->conv = DRONE_KATYSALVO;
											break;
										}
									}
								}
							}
						}
					}
				}

				if(ang)
				{
					// set route
					if(drone->drone & DRONE_KATY)
					{
						if(drone->threatened || !(j == drone->dronefield && dist < katyrange->value))
						{
							x = DRONE_TANK_SPEED * sin(Com_Rad(ang)) * -1;
							y = DRONE_TANK_SPEED * cos(Com_Rad(ang));
						}
						else
							x = y = 0;
					}
					else
					{
						if(i < MAX_BUILDINGS || k || dist > 300 || drone->threatened)
						{
							x = DRONE_TANK_SPEED * sin(Com_Rad(ang)) * -1;
							y = DRONE_TANK_SPEED * cos(Com_Rad(ang));
						}
						else
							x = y = 0;
					}

					if((ang *= 10) > 901)
					{
						ang -= 3600;
					}

					drone->speedxyz[0][0] = x;
					drone->speedxyz[1][0] = y;
					drone->angles[2][0] = ang;
				}
			}
			else
			{
				if(drone->drone & DRONE_KATY)
					PPrintf(drone->related[0], RADIO_DARKGREEN, "Katyusha %s finished its mission", drone->longnick);
				else
				PPrintf(drone->related[0], RADIO_DARKGREEN, "Tank %s finished its mission", drone->longnick);
				RemoveDrone(drone);
			}
			break;
			
		case DRONE_HTANK:
		case DRONE_HMACK:
			if(!((arena->frame - drone->frame) % 50))
			{
				if(drone->related[0])
				{
					drone->posxy[0][0] += drone->related[0]->speedxyz[0][0]/2;
					drone->posxy[1][0] += drone->related[0]->speedxyz[1][0]/2;

					if(!((arena->frame - drone->frame) % 200))
						drone->posalt[0] = GetHeightAt(drone->posxy[0][0], drone->posxy[1][0]);

					if(drone->posalt[0])
					{
						drone->speedxyz[0][0] = drone->related[0]->speedxyz[0][0];
						drone->speedxyz[1][0] = drone->related[0]->speedxyz[1][0];
					}
					else
					{
					drone->speedxyz[0][0] = drone->related[0]->speedxyz[0][0] = 0;
						drone->speedxyz[1][0] = drone->related[0]->speedxyz[1][0] = 0;
					}

						drone->angles[2][0] = drone->related[0]->angles[2][0];

					if(drone->related[0]->attached == drone)
					{
						drone->related[0]->posxy[0][0] = drone->posxy[0][0];
						drone->related[0]->posxy[1][0] = drone->posxy[1][0];
						drone->related[0]->posalt[0] = drone->posalt[0];
						SendDronePos(drone, drone->related[0]);
					}
				}
			}
			break;
		case DRONE_COMMANDOS:
			if(drone->dronetimer)
			{
				if(!(drone->countrytime))
				{
					if(arena->fields[drone->dronefield].country != drone->country)
					{
						for(i = 0; i < MAX_BUILDINGS; i++)
						{
							if(!arena->fields[drone->dronefield].buildings[i].field)
							{
								break;
							}
							else if(!arena->fields[drone->dronefield].buildings[i].status && IsVitalBuilding(&(arena->fields[drone->dronefield].buildings[i])))
							{
								x = drone->posxy[0][0] - arena->fields[drone->dronefield].buildings[i].posx;
								y = drone->posxy[1][0] - arena->fields[drone->dronefield].buildings[i].posy;

								if(x < 10000 && x > -10000 && y < 10000 && y > -10000)
								{
									if(sqrt(Com_Pow(x, 2) + Com_Pow(y, 2)) < 14000)
									{
										ThrowBomb(FALSE, drone->posxy[0][0], drone->posxy[1][0], GetHeightAt(drone->posxy[0][0], drone->posxy[1][0]) + 50, arena->fields[drone->dronefield].buildings[i].posx, arena->fields[drone->dronefield].buildings[i].posy, 0, drone);
//										ThrowBomb(TRUE, drone->posxy[0][0], drone->posxy[1][0], drone->posalt[0], arena->fields[drone->dronefield].buildings[i].posx, arena->fields[drone->dronefield].buildings[i].posy, 0, drone);
									}
									break;
								}
							}
						}
					}
					else
						drone->dronetimer = 0;
				}
				drone->countrytime--;

				if(drone->countrytime > 36000) // remove drone if throw time is more than 6 minutes (bug)
				{
					PPrintf(drone->related[0], RADIO_DARKGREEN, "Commandos has finished his mission");
					RemoveDrone(drone);
				}

				drone->dronetimer--;
			}
			else
			{
				PPrintf(drone->related[0], RADIO_DARKGREEN, "Commandos has finished his mission");
				RemoveDrone(drone);
			}
			break;
		case DRONE_DEBUG:
			drone->angles[0][0] = dpitch->value;
			drone->angles[1][0] = droll->value;
			drone->angles[2][0] = dyaw->value;
			break;
		case DRONE_EJECTED:
			break;
		default:
			return -1;
	}

	if(drone->related[0])
	{
		drone->offset = drone->timer - drone->related[0]->timer;
		drone->timer = drone->related[0]->timer;
	}
	else
	{
		if(drone->drone & (DRONE_FAU | DRONE_WINGS1 | DRONE_WINGS2 | DRONE_HMACK | DRONE_HTANK | DRONE_KATY | DRONE_EJECTED | DRONE_COMMANDOS | DRONE_DEBUG))
		{
			Com_Printf("DEBUG: Removed unrelated drone %s type %u\n", drone->longnick, drone->drone);
			return -1;
		}

		drone->offset = drone->timer - arena->time;
		drone->timer = arena->time;
	}

	return 0;
}

/*************
FireAck

Fires ack fire from pos1 to pos2
*************/

void FireAck(client_t *source, client_t *dest, u_int8_t animate)
{
	u_int8_t i, buffer[31];
	int16_t velx, vely, velz, part;
	int32_t dist;
	ottofiring_t *otto;

	if((dist = DistBetween(source->posxy[0][0], source->posxy[1][0], source->posalt[0], dest->posxy[0][0], dest->posxy[1][0], dest->posalt[0], -1)) > 3000)
		return;

	memset(buffer, 0, sizeof(buffer));

	otto = (ottofiring_t *) buffer;

	velx = (float)((dest->posxy[0][0] + dest->speedxyz[0][0]) - source->posxy[0][0]) * 3170 / dist;
	vely = (float)((dest->posxy[1][0] + dest->speedxyz[1][0]) - source->posxy[1][0]) * 3170 / dist;
	velz = (float)((dest->posalt[0] + dest->speedxyz[2][0]) - source->posalt[0]) * 3170 / dist;

	if(!animate)
	{
		if(rand()%100 < ((1.1 - (float)dist/3000) * 100))
		{
			i = 0;
			while(dest->armor.points[part = (rand()%32)] < 0)
			{
				i++;
//				Com_Printf("WARNING: DEBUG LOOP: dest->armor.points[%u] = %d\n", part, dest->armor.points[part]);
				if(i > 150)
				{
					Com_Printf("WARNING: DEBUG LOOP: Infinite loop detected, breaking off\n");
					return;
				}
			}

			AddPlaneDamage(part, 40, 0, NULL, NULL, dest);

			part = AddKiller(dest, source);
			if(part >= 0)
				dest->damby[part] += 40;

			SendPings(1, 143, dest);
		}
	}

	if(wb3->value)
	{
		otto->packetid = htons(Com_WBhton(0x1900));
	}
	else
	{
		otto->packetid = htons(0x2101);
	}
	otto->item = 143;
	otto->posx = htonl(source->posxy[0][0]);
	otto->posy = htonl(source->posxy[1][0]);
	otto->alt = htonl(source->posalt[0] + 2);
	otto->xspeed = htons(velx);
	otto->yspeed = htons(vely);
	otto->zspeed = htons(velz);
	otto->unknown6 = htonl(0x40243);
	otto->shortnick = htonl(source->shortnick);

	////////////

	for(i = 0; i < MAX_SCREEN; i++)
	{
		if(source->visible[i].client && !source->visible[i].client->drone)
		{
			SendPacket(buffer, sizeof(buffer), source->visible[i].client);
		}
	}
}


/*************
CVFire

Fires artillary fire to nearest enemy field
*************/

void CVFire(int32_t origx, int32_t origy, int32_t origz, int32_t destx, int32_t desty, int32_t destz)
{
	u_int8_t i;

	for(i = 0; i < cvsalvo->value; i++)
	{
		ThrowBomb(FALSE, origx, origy, origz, destx, desty, destz, NULL);
	}

	ThrowBomb(TRUE, origx, origy, origz, destx, desty, destz, NULL);
}

/*************
ThrowBomb

Throw a bomb from orig to dest position with some precision
*************/

void ThrowBomb(u_int8_t animate, int32_t origx, int32_t origy, int32_t origz, int32_t destx, int32_t desty, int32_t destz, client_t *client)
{
	rocketbomb_t *rocketbomb;
	u_int8_t mun = 0;
	int32_t dist;
	int16_t velx = 0, vely = 0, velz = 0;
	double angle;
	u_int8_t i;
	u_int8_t buffer[31];

	if(!animate)
	{
		if(!(dist = sqrt(Com_Pow(origx - destx, 2) + Com_Pow(origy - desty, 2))))
		{
			PPrintf(client?client->related[0]:NULL, RADIO_YELLOW, "Error calculating distance (dist = 0, %s) %s",
				Com_Padloc(origx, origy),
				client?client->longnick:"");
			return;
		}

		if(client && !(client->drone & (DRONE_TANK1 | DRONE_TANK2 | DRONE_KATY))) // COMMANDOS or MINEN
		{
			if(client->drone & DRONE_COMMANDOS)
			{
				//dispersion
				if(dist > 125)
				{
					velx = rand()%(dist / 125); // old value = 50
					destx += Com_Pow(-1, rand()%2) * velx;
					vely = rand()%(dist / 125); // old value = 50
					desty += Com_Pow(-1, rand()%2) * vely;
				}
			}
		}
		else if(client && (client->drone & DRONE_KATY)) // KATY
		{
			//dispersion
			if(dist > 500)
			{
				velx = rand()%(dist / 500);
				destx += Com_Pow(-1, rand()%2) * velx;
				vely = rand()%(dist / 500);
				desty += Com_Pow(-1, rand()%2) * vely;
			}
		}
		else // CV, TANKS
		{
			//dispersion
			if(dist > 20)
			{
				velx = rand()%(dist / 20);
				destx += Com_Pow(-1, rand()%2) * velx;
				vely = rand()%(dist / 20);
				desty += Com_Pow(-1, rand()%2) * vely;
			}
		}

		dist = sqrt(Com_Pow(origx - destx, 2) + Com_Pow(origy - desty, 2));

		if(client && (client->drone & DRONE_KATY))
		{
			angle = RocketAngle(dist);

			if(angle < 0)
			{
				PPrintf(client->related[0], RADIO_YELLOW, "Error on calculating katyusha launching angle");
				return;
			}
		}
		else if(client && !(client->drone & (DRONE_TANK1 | DRONE_TANK2))) // COMMANDOS or MINEN
			angle = Com_Deg(asin((GRAVITY * dist) / Com_Pow(MORTAR, 2)) / 2);
		else // CV
			angle = Com_Deg(asin((GRAVITY * dist) / Com_Pow(1500, 2)) / 2); // CV, TANKS fire

		//velz=>velxy
		if(client && (client->drone & DRONE_KATY))
			velz = 70 * cos(Com_Rad(angle));
		else if(client && !(client->drone & (DRONE_TANK1 | DRONE_TANK2))) // COMMANDOS or MINEN
			velz = MORTAR * cos(Com_Rad(90 - angle));
		else // CV
		{
			if((origz * 10) > destz)
				velz = 1500 * cos(Com_Rad(angle)); // CV, TANKS fire
			else
				velz = 1500 * cos(Com_Rad(90 - angle)); // CV, TANKS fire
		}

		velx = (float)(destx - origx) * velz / dist;
		vely = (float)(desty - origy) * velz / dist;

		//
		if(client && (client->drone & DRONE_KATY))
			velz = 70 * sin(Com_Rad(angle));
		else if(client && !(client->drone & (DRONE_TANK1 | DRONE_TANK2))) // COMMANDOS or MINEN
			velz = MORTAR * sin(Com_Rad(90 - angle));
		else // CV
		{
			if((origz * 10) > destz)
				velz = 1500 * sin(Com_Rad(angle)); // CV, TANKS fire
			else
				velz = 1500 * sin(Com_Rad(90 - angle)); // CV, TANKS fire
		}

		if(client) // COMMANDOS or MINEN (or HKATY?)
		{
			if(client->drone & DRONE_COMMANDOS) // COMMANDOS
			{
				client->countrytime = (float) 200 * velz / GRAVITY;

				if(wb3->value)
					mun = 88; // 250kg AP
				else
					mun = 113; // Flare

				AddBomb(0x01F9, destx, desty, mun/*Mortar*/, MORTAR, client->countrytime, client->related[0]);

				client->countrytime += 100; // give one second to commandos change target.
			}
			else if(client->drone & (DRONE_TANK1 | DRONE_TANK2))
			{
				dist = (float) 200 * velz / GRAVITY;

				if(wb3->value)
					mun = 103; // 75mm M1897
				else
					mun = 112; // Bomb

				AddBomb(0x01F9, destx, desty, mun/*Mortar*/, 1500, dist /*timer*/, client);
			}
			else if(client->drone & DRONE_KATY)
			{
				dist = RocketTime(angle) * 100;

				if(dist < 0)
				{
					PPrintf(client->related[0], RADIO_YELLOW, "Error on calculating Katyusha launching time");
					return;
				}

				mun = 57;//64/*RS132*/;

				AddBomb(0x01F9, destx, desty, mun/*4.5in M10 Rocket*/, 2000, dist /*timer*/, client);
			}
			else // MINEN
			{
				dist = (float) 200 * velz / GRAVITY;

				if(wb3->value)
					mun = 88; // 250kg AP
				else
					mun = 113; // Flare

				AddBomb(0x01F9, destx, desty, mun/*Mortar*/, MORTAR, dist /*timer*/, client);
			}
		}
		else // CV, TANKS
		{
			dist = (float) 200 * velz / GRAVITY;

			mun = 83;

			AddBomb(0x01F9, destx, desty, mun/*Mortar*/, 1500, dist /*timer*/, NULL);
		}
	}

	rocketbomb = (rocketbomb_t *) buffer;

	rocketbomb->packetid = htons(Com_WBhton(0x1900));

	if(animate)
	{
		if(wb3->value)
			rocketbomb->item = 149 /* Flak */;
		else
			rocketbomb->item = 150 /* Hvy Flak */;
	}
	else
		rocketbomb->item = mun;
	rocketbomb->id = htons(0x01F9);
	rocketbomb->posx = htonl(origx);
	rocketbomb->posy = htonl(origy);
	rocketbomb->alt = htonl(origz);
	rocketbomb->xspeed = htons(velx);
	rocketbomb->yspeed = htons(vely);
	rocketbomb->zspeed = htons(velz);
	rocketbomb->unknown1 = htonl(0x20);
	rocketbomb->shortnick = client?htonl(client->shortnick):0;

	if(client && !(client->drone & (DRONE_COMMANDOS | DRONE_TANK1 | DRONE_TANK2 | DRONE_KATY))) // MINEN
	{
		SendPacket(buffer, sizeof(buffer), client);
		rocketbomb->item = 150 /* Hvy Flak */;
		SendPacket(buffer, sizeof(buffer), client);
		rocketbomb->item = mun;
	}

	if(client) // COMMANDOS or MINEN or KATY
		ProcessPacket(buffer, sizeof(buffer), client);
	else // CV
	{
		for(i = 0; i < maxentities->value; i++)
		{
			if(clients[i].inuse && clients[i].ready && !clients[i].drone)
			{
				destx = clients[i].posxy[0][0] - origx;
				desty = clients[i].posxy[1][0] - origy;

				if((MODULUS(destx) < cvrange->value) && (MODULUS(desty) < cvrange->value))
				{
					SendPacket(buffer, sizeof(buffer), &clients[i]);
				}
			}
		}
	}
}

/*************
SendDronePos

Send drone pos to attached player
*************/

void SendDronePos(client_t *drone, client_t *client)
{
	u_int8_t buffer[46];
	planeposition_t *pos;

	memset(buffer, 0, sizeof(buffer));

	pos = (planeposition_t *)buffer;

	pos->packetid = htons(0x0e00);
	pos->posx = htonl(drone->posxy[0][0]);
	pos->posy = htonl(drone->posxy[1][0]);
	pos->alt = htonl(drone->posalt[0]);
	pos->xspeed = htons(drone->speedxyz[0][0]);
	pos->yspeed = htons(drone->speedxyz[1][0]);
	pos->climbspeed = htons(drone->speedxyz[2][0]);
	pos->sideaccel = htons(drone->accelxyz[0][0]);
	pos->forwardaccel = htons(drone->accelxyz[1][0]);
	pos->climbaccel = htons(drone->accelxyz[2][0]);
	pos->pitchangle = htons(drone->angles[0][0]);
	pos->rollangle = htons(drone->angles[1][0]);
	pos->yawangle = htons(drone->angles[2][0]);
	pos->pitchangspeed = htons(drone->aspeeds[0][0]);
	pos->rollangspeed = htons(drone->aspeeds[1][0]);
	pos->yawangspeed = htons(drone->aspeeds[2][0]);
	pos->timer = htonl(arena->time); // FIXME: drone->timer?
	pos->radar = htons(0x30);
	pos->plane = 0;//htons(drone->plane);

	SendPacket(buffer, sizeof(buffer), client);
}

/*************
SendXBombs

Send a few bombs making a X
*************/

void SendXBombs(client_t *drone)
{
	rocketbomb_t *rocketbomb;
	u_int8_t i;
	u_int8_t buffer[31];

	rocketbomb = (rocketbomb_t *) buffer;

	rocketbomb->packetid = htons(Com_WBhton(0x1900));
	rocketbomb->item = 91 /*1000kg bomb*/;
	rocketbomb->alt = htonl(0xd6);
	rocketbomb->xspeed = htons(0);
	rocketbomb->yspeed = htons(0);
	rocketbomb->zspeed = htons(0xff80);
	rocketbomb->unknown1 = htonl(0x20);
	rocketbomb->shortnick = htonl(drone->shortnick);

	for(i = 0; i < 9; i++)
	{
		rocketbomb->id = htons(0x01F9+i);
		rocketbomb->posx = htonl(drone->posxy[0][0]+800-(i*200));
		rocketbomb->posy = htonl(drone->posxy[1][0]+800-(i*200));
		ProcessPacket(buffer, sizeof(buffer), drone);
		rocketbomb->id = htons(0x01F9+i);
		rocketbomb->posx = htonl(drone->posxy[0][0]+800-(i*200));
		rocketbomb->posy = htonl(drone->posxy[1][0]-800+(i*200));
		ProcessPacket(buffer, sizeof(buffer), drone);
	}
}

/*************
HitStructsNear

Check if there are structs near bomb hit and kill them
*************/

u_int8_t HitStructsNear(int32_t x, int32_t y, u_int8_t type, u_int16_t speed, u_int8_t nuke, client_t *client)
{
	int32_t a, b;
	u_int16_t i, j, k;
	u_int8_t field, city, damaged;
	munition_t *munition, *max, *min;
	int16_t radius;
	int8_t killer = 0;
	
	munition = GetMunition(type);
		
	if(!munition)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Unknown munition ID %d, plane %d", type, client?client->plane:0);
		return 0;
	}

	min = GetMunition(81);
	max = GetMunition(87);

	if(max->he == min->he)
	{
		Com_Printf("WARNING: HitStructsNear(): min->he == max->he\n");
		radius = 0;
	}
	else
		radius = (munition->he - min->he) * (MAX_BOMBRADIUS - MIN_BOMBRADIUS) / (max->he - min->he);

	radius += MIN_BOMBRADIUS;

	if(!nuke)
		if(radius > MAX_BOMBRADIUS)
			radius = MAX_BOMBRADIUS;

	if(gunstats->value)
		PPrintf(client, RADIO_RED, "Radius %d", radius);

	j = 0;
	k = fields->value + cities->value;

	for(field = 0; field < k; field++)
	{
		if(field < fields->value)
		{
			a = x - arena->fields[field].posxyz[0];
			b = y - arena->fields[field].posxyz[1];
		}
		else
		{
			a = x - arena->cities[field - (int16_t)fields->value].posxyz[0];
			b = y - arena->cities[field - (int16_t)fields->value].posxyz[1];
		}

		if((a >= -3600 && a <= 3600) && (b >= -3600 && b <= 3600))
		{
			if(sqrt(Com_Pow(a, 2) + Com_Pow(b, 2)) < 3600)
			{
				if(field < fields->value)
				{
					for(i = 0; i < MAX_BUILDINGS; i++)
					{
						if(!arena->fields[field].buildings[i].field)
						{
							break;
						}
						else if(!arena->fields[field].buildings[i].status)
						{
							a = x - arena->fields[field].buildings[i].posx;
							b = y - arena->fields[field].buildings[i].posy;
							if(sqrt(Com_Pow(a, 2) + Com_Pow(b, 2)) < radius)
							{
								if(gunstats->value)
									PPrintf(client, RADIO_GREEN, "Hit %s Damage %d", GetBuildingType(arena->fields[field].buildings[i].type), munition->he);
									
								Com_Printf("%s %shit %s with %s\n", client?client->longnick:"-HOST-", (client && client->country==arena->fields[field].buildings[i].country)?"friendly ":"", GetBuildingType(arena->fields[field].buildings[i].type), munition->abbrev);

								if(client)
								{
									Com_LogEvent(EVENT_HITSTRUCT, client->id, 0);
									Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
									Com_LogDescription(EVENT_DESC_COUNTRY, arena->fields[field].buildings[i].country, NULL);
									Com_LogDescription(EVENT_DESC_STRUCT, arena->fields[field].buildings[i].type, NULL);
									Com_LogDescription(EVENT_DESC_FIELD, arena->fields[field].buildings[i].field, NULL);
									Com_LogDescription(EVENT_DESC_AMMO, type, NULL);
								}

									
								damaged = AddBuildingDamage(&arena->fields[field].buildings[i], munition->he, munition->ap, client);

								if(arena->fields[field].type >= FIELD_CV && arena->fields[field].type <= FIELD_SUBMARINE && damaged)
								{
									CheckBoatDamage(&arena->fields[field].buildings[i], client);
								}

								if(gunstats->value)
									PPrintf(client, RADIO_GREEN, "Hit %s", GetBuildingType(arena->fields[field].buildings[i].type));
								j++;
							}
						}
					}
				}
				else // destroying cities
				{
					city = field - fields->value;
					
					for(i = 0; i < MAX_BUILDINGS; i++)
					{
						if(!arena->cities[city].buildings[i].field)
						{
							break;
						}
						else if(!arena->cities[city].buildings[i].status)
						{
							a = x - arena->cities[city].buildings[i].posx;
							b = y - arena->cities[city].buildings[i].posy;
							if(sqrt(Com_Pow(a, 2) + Com_Pow(b, 2)) < radius)
							{
								if(gunstats->value)
									PPrintf(client, RADIO_GREEN, "Hit %s Damage %d", GetBuildingType(arena->cities[city].buildings[i].type), munition->he);
									
								Com_Printf("%s %shit %s with %s\n", client?client->longnick:"-HOST-", (client && client->country==arena->cities[city].buildings[i].country)?"friendly ":"", GetBuildingType(arena->cities[city].buildings[i].type), munition->abbrev);
									
								if(client)
								{									
									Com_LogEvent(EVENT_HITSTRUCT, client->id, 0);
									Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
									Com_LogDescription(EVENT_DESC_COUNTRY, arena->cities[city].buildings[i].country, NULL);
									Com_LogDescription(EVENT_DESC_STRUCT, arena->cities[city].buildings[i].type, NULL);
									Com_LogDescription(EVENT_DESC_FIELD, arena->cities[city].buildings[i].field, NULL);
									Com_LogDescription(EVENT_DESC_AMMO, type, NULL);
								}

									
								AddBuildingDamage(&arena->cities[city].buildings[i], munition->he, munition->ap, client);
								j++;
							}
						}
					}
				}
			}
		}
	}

	for(i = 0; i < maxentities->value; i++) // check players near explosion
	{
		if(clients[i].inuse && clients[i].infly)
		{
			if(clients[i].drone & (DRONE_TANK1 | DRONE_TANK2))
				radius = 50;
			else
				radius = 180;


			a = x - clients[i].posxy[0][0];
			b = y - clients[i].posxy[1][0];

			if(a > -200 && a < 200 && b > -200 && b < 200)
			{
				if(sqrt(Com_Pow(a, 2) + Com_Pow(b, 2)) < radius)
				{
					if((clients[i].posalt[0] - GetHeightAt(clients[i].posxy[0][0], clients[i].posxy[1][0])) < radius)
					{
						if(!(clients[i].drone && clients[i].related[0] == client)) // allow to kill own drones (no penalties, no score, etc)
						{
							killer = AddKiller(&clients[i], client);

							if(killer >= 0)
								clients[i].damby[killer] += munition->he;
						}

						AddPlaneDamage(PLACE_CENTERFUSE, munition->he, 0, NULL, NULL, &clients[i]);
					}
				}
			}
		}
	}

	return j;
}

/*************
PFAUDamage

Add damage to hit field if it does
*************/

void PFAUDamage(client_t *fau)
{
	u_int8_t i, j, k;
	int32_t a, b;
	u_int32_t dist;

	k = fields->value + cities->value;

	for(i = 0; i < k; i++)
	{

		if(i < fields->value)
		{
			a = fau->posxy[0][0] - arena->fields[i].posxyz[0];
			b = fau->posxy[1][0] - arena->fields[i].posxyz[1];
		}
		else
		{
			a = fau->posxy[0][0] - arena->cities[i - (int16_t)fields->value].posxyz[0];
			b = fau->posxy[1][0] - arena->cities[i - (int16_t)fields->value].posxyz[1];
		}

		if((a >= -3600 && a <= 3600) && (b >= -3600 && b <= 3600))
		{
			if((dist = sqrt(Com_Pow(a, 2) + Com_Pow(b, 2))) < 3600)
				break;
		}
	}

	if(i < k)
	{
		if(fau->related[0])
		{
			if(i < fields->value)
				PPrintf(fau->related[0], RADIO_YELLOW, "V-1 hit F%d dist %d", i+1, dist);
			else
				PPrintf(fau->related[0], RADIO_YELLOW, "V-1 hit %s (C%d) dist %d", arena->cities[i - (int16_t)fields->value].name, i+1-(int16_t)fields->value, dist);
		}
	}
	else if(fau->related[0])
	{
		PPrintf(fau->related[0], RADIO_YELLOW, "V-1 lost");
	}

	for(j = 0, k = 0; j < 9; j++)
	{
		a = fau->posxy[0][0]+800-(j*200);
		b = fau->posxy[1][0]+800-(j*200);
		k += HitStructsNear(a, b, 87/*2000Kg GP*/, DRONE_FAU_SPEED, 0, fau->related[0]);
		a = fau->posxy[0][0]+800-(j*200);
		b = fau->posxy[1][0]-800+(j*200);
		k += HitStructsNear(a, b, 87/*2000Kg GP*/, DRONE_FAU_SPEED, 0, fau->related[0]);
	}

	if(fau->related[0])
		PPrintf(fau->related[0], RADIO_YELLOW, "Hit %d structures", k);
}

/*************
DroneWings

Add wingmans to a client
*************/

void DroneWings(client_t *client)
{
	AddDrone(DRONE_WINGS1, client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->country, client->plane, client);
	AddDrone(DRONE_WINGS2, client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->country, client->plane, client);
}

/*************
NewDroneName

Get a new name for drone in drone name file
*************/

u_int32_t NewDroneName(client_t *client)
{
	FILE *fp;
	char buffer[8];
	u_int16_t i, j, k, l;
	u_int32_t nick;

	Sys_WaitForLock(FILE_DRONENICKS_LOCK);

	if(Sys_LockFile(FILE_DRONENICKS_LOCK) < 0)
	{
		return client->shortnick;
	}


	if((fp = fopen(FILE_DRONENICKS, "r")) == NULL)
	{
		Com_Printf("WARNING: Couldn't open \"%s\"\n", FILE_DRONENICKS);
		nick = client->shortnick;
	}
	else
	{
		if(fgets(buffer, 8, fp) == NULL)
		{
			Com_Printf("Unexpected end of %s\n", FILE_DRONENICKS);
			fclose(fp);
			Sys_UnlockFile(FILE_DRONENICKS_LOCK);
			return client->shortnick;
		}

		i = Com_Atoi(buffer);

		j = (arena->time % i) + 1; // random nick in list

		k = 0;
		while(k < j)
		{
			if(fgets(buffer, 8, fp) == NULL)
			{
				Com_Printf("Unexpected end of %s\n", FILE_DRONENICKS);
				fclose(fp);
				Sys_UnlockFile(FILE_DRONENICKS_LOCK);
				return client->shortnick;
			}

			k++;
		}

		nick = ascii2wbnick(buffer, 1);

		for(k = 0; k < maxentities->value; k++) // check if nick is already in use
		{
			if(clients[k].inuse)
				if(clients[k].shortnick == nick) // nick in use
					break;
		}

		if(k != maxentities->value) // nick in use
		{
			for(k = j; k < i; k++)
			{
				if(fgets(buffer, 8, fp) == NULL)
				{
					Com_Printf("Unexpected end of %s\n", FILE_DRONENICKS);
					fclose(fp);
					Sys_UnlockFile(FILE_DRONENICKS_LOCK);
					return client->shortnick;
				}

				nick = ascii2wbnick(buffer, 1);

				for(l = 0; l < maxentities->value; l++)
				{
					if(clients[l].shortnick == nick) // nick in use
						break;
				}

				if(l == maxentities->value)
				{
					fclose(fp);
					Sys_UnlockFile(FILE_DRONENICKS_LOCK);
					return nick;
				}

			}

			fclose(fp);

			if((fp = fopen(FILE_DRONENICKS, "r")) == NULL)
			{
				Com_Printf("WARNING: Couldn't open \"%s\"\n", FILE_DRONENICKS);
				Sys_UnlockFile(FILE_DRONENICKS_LOCK);
				return client->shortnick;
			}
			else
			{
				// crop end of ring
				if(fgets(buffer, 8, fp) == NULL)
				{
					Com_Printf("Unexpected end of %s\n", FILE_DRONENICKS);
					fclose(fp);
					Sys_UnlockFile(FILE_DRONENICKS_LOCK);
					return client->shortnick;
				}

				for(k = 0; k < j; k++)
				{
					if(fgets(buffer, 8, fp) == NULL)
					{
						Com_Printf("Unexpected end of %s\n", FILE_DRONENICKS);
						fclose(fp);
						Sys_UnlockFile(FILE_DRONENICKS_LOCK);
						return client->shortnick;
					}

					nick = ascii2wbnick(buffer, 0);//1);

					for(l = 0; l < maxentities->value; l++) // start again from begin
					{
						if(clients[l].shortnick == nick) // nick in use
							break;
					}

					if(l == maxentities->value)
					{
						fclose(fp);
						Sys_UnlockFile(FILE_DRONENICKS_LOCK);
						return nick;
					}
				}

				if(k == j)
				{
					nick = client->shortnick;
				}
			}
		}

		fclose(fp);
	}

	Sys_UnlockFile(FILE_DRONENICKS_LOCK);

	return nick;
}

void LaunchTanks(u_int8_t fieldfrom, u_int8_t fieldto, u_int8_t country, client_t *client)
{
	double angle;
	int32_t x, y;
	client_t *drone;

	x = arena->fields[fieldfrom].posxyz[0] - arena->fields[fieldto].posxyz[0];
	y = arena->fields[fieldto].posxyz[1] - arena->fields[fieldfrom].posxyz[1];

	if(!y)
	{
		if(x > 0)
			angle = 90;
		else
			angle = 270;
	}
	else
		angle = Com_Deg(atan2(x, y));

	x = DRONE_TANK_SPEED * sin(Com_Rad(angle)) * -1;
	y = DRONE_TANK_SPEED * cos(Com_Rad(angle));

	if(angle < 0)
	{
		angle += 360;
	}

	if((angle *= 10) > 901)
	{
		angle -= 3600;
	}

	if((drone = AddDrone(DRONE_TANK1, arena->fields[fieldfrom].posxyz[0], arena->fields[fieldfrom].posxyz[1], arena->fields[fieldfrom].posxyz[2], country, 101/*TANK*/, client)))
	{
		drone->speedxyz[0][0] = x;
		drone->speedxyz[1][0] = y;
		drone->dronefield = fieldto;
		drone->angles[2][0] = angle;
	}
	if((drone = AddDrone(DRONE_TANK2, arena->fields[fieldfrom].posxyz[0], arena->fields[fieldfrom].posxyz[1], arena->fields[fieldfrom].posxyz[2], country, 101/*TANK*/, client)))
	{
		drone->speedxyz[0][0] = x;
		drone->speedxyz[1][0] = y;
		drone->dronefield = fieldto;
		drone->angles[2][0] = angle;
	}
	if((drone = AddDrone(DRONE_AAA, arena->fields[fieldfrom].posxyz[0], arena->fields[fieldfrom].posxyz[1], arena->fields[fieldfrom].posxyz[2], country, 85/*AAA*/, client)))
	{
		drone->speedxyz[0][0] = x;
		drone->speedxyz[1][0] = y;
		drone->dronefield = fieldto;
		drone->angles[2][0] = angle;
		
	}	
	if((drone = AddDrone(DRONE_KATY, arena->fields[fieldfrom].posxyz[0], arena->fields[fieldfrom].posxyz[1], arena->fields[fieldfrom].posxyz[2], country, 85/*JEEP*/, client)))
	{
		drone->speedxyz[0][0] = x;
		drone->speedxyz[1][0] = y;
		drone->dronefield = fieldto;
		drone->angles[2][0] = angle;
	}

	PPrintf(client, RADIO_YELLOW, "Armoured column launched to f%d", fieldto+1);
}
