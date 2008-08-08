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
GetBuilding

Returns a pointer to building if found or NULL
*************/

building_t *GetBuilding(u_int16_t id)
{
	u_int16_t i, j;

	for(i = 0; i < fields->value; i++)
	{
		for(j = 0; j < MAX_BUILDINGS; j++)
		{
			if(!arena->fields[i].buildings[j].field)
			{
				break;
			}
			else
			{
				if(arena->fields[i].buildings[j].id == id)
					return &(arena->fields[i].buildings[j]);
			}
		}
	}

	for(i = 0; i < cities->value; i++)
	{
		for(j = 0; j < MAX_BUILDINGS; j++)
		{
			if(!arena->cities[i].buildings[j].field)
			{
				break;
			}
			else
			{
				if(arena->cities[i].buildings[j].id == id)
					return &(arena->cities[i].buildings[j]);
			}
		}
	}

	return NULL;
}

/*************
GetFieldType

Returns type of a field
*************/

char *GetFieldType(u_int8_t type)
{
	switch(type)
	{
		case FIELD_LITTLE:
			return "Small";
		case FIELD_MEDIUM:
			return "Medium";
		case FIELD_MAIN:
			return "Main";
		case FIELD_CV:
			return "CV";
		case FIELD_CARGO:
			return "Cargo Ships";
		case FIELD_DD:
			return "Destroyers";
		case FIELD_SUBMARINE:
			return "Submarine";
		case FIELD_RADAR:
			return "Radar";
		case FIELD_BRIDGE:
			return "Bridge";
		case FIELD_CITY:
			return "City";
		case FIELD_PORT:
			return "Port";
		case FIELD_CONVOY:
			return "Convoy";
		case FIELD_FACTORY:
			return "Factory";
		case FIELD_REFINERY:
			return "Refinery";
		case FIELD_OPENFIELD:
			return "Open field";
		case FIELD_WB3POST:
			return "Post";
		case FIELD_WB3VILLAGE:
			return "Village";
		case FIELD_WB3TOWN:
			return "Town";
		default:
			return "Unknown";
	}
}

/*************
GetBuildingType

Returns name of a building
*************/

char *GetBuildingType(u_int16_t type)
{
	switch(type)
	{
		case BUILD_50CALACK:
			return "50 CAL Ack";
		case BUILD_20MMACK:
			return "20mm Ack";
		case BUILD_40MMACK:
			return "40mm Ack";
		case BUILD_88MMFLAK:
			return "88mm Flak";
		case BUILD_TOWER:
			return "Tower";
		case BUILD_HANGAR:
			return "Hangar";
		case BUILD_FUEL:
			return "Fuel";
		case BUILD_AMMO:
			return "Ammo Depot";
		case BUILD_RADAR:
			return "Radar";
		case BUILD_WARE:
			return "Warehouse";
		case BUILD_RADIOHUT:
			return "Radio Hut";
		case BUILD_ANTENNA:
			return "Antenna";
		case BUILD_CV:
			return "CV";
		case BUILD_DESTROYER:
			return "Destroyer";
		case BUILD_CRUISER:
			return "Cruiser";
		case BUILD_CARGO:
			return "Cargo Ship";
		case BUILD_SUBMARINE:
			return "Submarine";
		case BUILD_BRIDGE:
			return "Bridge";
		case BUILD_SPECIALBUILD:
			return "Special Building";
		case BUILD_FACTORY:
			return "Factory";
		case BUILD_BARRACKS:
			return "Barracks";
		case BUILD_STATICS:
			return "Static plane";
		case BUILD_REFINERY:
			return "Oil Refinery";
		case BUILD_PLANEFACTORY:
			return "Plane Factory";
		case BUILD_BUILDING:
			return "Building";
		case BUILD_CRANE:
			return "Crane";
		case BUILD_STRATEGIC:
			return "Strategic Target";
		case BUILD_ARTILLERY:
			return "Artillery";
		case BUILD_HUT:
			return "Hut";
		case BUILD_TRUCK:
			return "Truck";
		case BUILD_TREE:
			return "Tree";
		case BUILD_SPAWNPOINT:
			return "Spawn Point";
		case BUILD_HOUSE:
			return "House";
		case BUILD_ROCK:
			return "Rock";
		case BUILD_FENCE:
			return "Fence";
		default:
			return "Unkown Building";
	}
}

/*************
LoadArenaStatus

Load arena status (fields, CV, city, etc)
*************/

void LoadArenaStatus(char *filename, client_t *client, u_int8_t reset)
{
	u_int16_t i, j, k, l;
	char file[128];
	char buffer[16384];
	FILE *fp;
	char *token;

	if(reset)
	{
		sprintf(file, "./arenas/%s/arena", filename);
	}
	else
	{
		strcpy(file, filename);
	}

	strcat(file, ".arn");

	if(!(fp = fopen(file, "r")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: LoadArenaStatus() Cannot open file \"%s\"", file);
		return;
	}
	else
	{
		if(client && !reset)
			Cmd_Undecl(0xffff, client);

		PPrintf(client, RADIO_LIGHTYELLOW, "Loading arena status from \"%s\"", file);

		for(i = 0; i < fields->value; i++)
		{
			memset(buffer, 0, sizeof(buffer));
			if(!fgets(buffer, sizeof(buffer), fp))
			{
				PPrintf(client, RADIO_YELLOW, "WARNING: Unexpected end of %s", file);
				fclose(fp);
				return;
			}
			else
			{
				arena->fields[i].number = i+1;

				arena->fields[i].type = Com_Atoi((char *)strtok(buffer, ";"));
				arena->fields[i].posxyz[0] = Com_Atoi((char *)strtok(NULL, ";"));
				arena->fields[i].posxyz[1] = Com_Atoi((char *)strtok(NULL, ";"));
				arena->fields[i].posxyz[2] = Com_Atoi((char *)strtok(NULL, ";"));
				arena->fields[i].country = Com_Atoi((char *)strtok(NULL, ";"));
				arena->fields[i].abletocapture = Com_Atoi((char *)strtok(NULL, ";"));
				arena->fields[i].closed = Com_Atoi((char *)strtok(NULL, ";"));

				if(!reset)
				{
					arena->fields[i].paras = Com_Atoi((char *)strtok(NULL, ";"));
				}
				else
					arena->fields[i].paras = 0;

				j = Com_Atoi((char *)strtok(NULL, ";"));

				for(k = 0; k < j; k++)
				{
					l = Com_Atoi((char *)strtok(NULL, ";"));

					if(l)
					{
						arena->cities[l - 1].field = &arena->fields[i];
						arena->fields[i].city[k] = &arena->cities[l - 1];
					}
				}

				if(!reset)
				{
//					for(j = 0; j < maxplanes; j++)
//					{
//						arena->fields[i].rps[j] = Com_Atoi((char *)strtok(NULL, ";"));
//					}

					for(j = 0; j < MAX_BUILDINGS; j++)
					{
						if(!(token = strtok(NULL, ";")))
							break;

						arena->fields[i].buildings[j].field = Com_Atoi(token);
						arena->fields[i].buildings[j].country = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].id = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].type = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].fieldtype = arena->fields[i].type;
						arena->fields[i].buildings[j].status = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].timer = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].armor = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].posx = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].posy = Com_Atoi((char *)strtok(NULL, ";"));
						arena->fields[i].buildings[j].posz = Com_Atoi((char *)strtok(NULL, ";"));
					}
				}
				else
				{
					for(j = 0; j < MAX_BUILDINGS; j++)
					{
						if(arena->fields[i].buildings[j].field)
							arena->fields[i].buildings[j].country = arena->fields[i].country;
						else
							break;
					}
				}
			}
		}

		for(i = 0; i < cities->value; i++)
		{
			memset(buffer, 0, sizeof(buffer));
			if(!fgets(buffer, sizeof(buffer), fp))
			{
				PPrintf(client, RADIO_YELLOW, "WARNING: Unexpected end of %s", file);
				fclose(fp);
				return;
			}
			else
			{
				arena->cities[i].number = i+1;
				arena->cities[i].type = Com_Atoi((char *)strtok(buffer, ";"));
				arena->cities[i].posxyz[0] = Com_Atoi((char *)strtok(NULL, ";"));
				arena->cities[i].posxyz[1] = Com_Atoi((char *)strtok(NULL, ";"));
				arena->cities[i].posxyz[2] = Com_Atoi((char *)strtok(NULL, ";"));
				arena->cities[i].country = Com_Atoi((char *)strtok(NULL, ";"));
				arena->cities[i].closed = Com_Atoi((char *)strtok(NULL, ";"));
				token = (char *)strtok(NULL, ";");
				strcpy(arena->cities[i].name, token?token:"unamed");
				arena->cities[i].needtoclose = Com_Atoi((char *)strtok(NULL, ";"));

				if(!reset)
				{
					for(j = 0; j < MAX_BUILDINGS; j++)
					{
						if(!(token = (char *)strtok(NULL, ";")))
							break;

						arena->cities[i].buildings[j].field = Com_Atoi(token);
						arena->cities[i].buildings[j].country = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].id = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].type = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].fieldtype = arena->cities[i].type;
						arena->cities[i].buildings[j].status = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].timer = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].armor = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].posx = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].posy = Com_Atoi((char *)strtok(NULL, ";"));
						arena->cities[i].buildings[j].posz = Com_Atoi((char *)strtok(NULL, ";"));
					}
				}
				else
				{
					for(j = 0; j < MAX_BUILDINGS; j++)
					{
						if(arena->cities[i].buildings[j].field)
							arena->cities[i].buildings[j].country = arena->cities[i].country;
						else
							break;
					}
				}				
			}
		}

		file[strlen(file) - 4] = '\0';
		LoadPlanesPool(file, client);

		for(i = 0; i < maxentities->value; i++)
		{
			if(clients[i].inuse && !clients[i].drone && clients[i].ready)
			{
				clients[i].arenafieldsok = 0; // make send capture fields packet
			}
		}


		if(reset)
		{
			if(rps->value)
			{
				Cmd_Seta("f-1", 0, -1, 0); // set 0 to all planes in all fields
				UpdateRPS();
			}
		}

		CalcFactoryBuildings();

		fclose(fp);
	}
}


/*************
SaveArenaStatus

Save arena status (fields, CV, city, etc)
*************/

void SaveArenaStatus(char *filename, client_t *client)
{
	u_int16_t i, j, k;
	char file[128];
	FILE *fp;

	strcpy(file, filename);
	strcat(file, ".arn");

	if(!(fp = fopen(file, "w")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: SaveArenaStatus() Cannot open file \"%s\"", file);
		return;
	}
	else
	{
		for(i = 0; i < fields->value; i++)
		{
			fprintf(fp, "%u;%d;%d;%d;%u;%u;%u;%u",
		   		arena->fields[i].type,
			    arena->fields[i].posxyz[0],
			    arena->fields[i].posxyz[1],
			    arena->fields[i].posxyz[2],
			    arena->fields[i].country,
			    arena->fields[i].abletocapture,
			    arena->fields[i].closed,
			    arena->fields[i].paras);

			for(j = k = 0; j < MAX_CITYFIELD; j++)
			{
				if(arena->fields[i].city[j])
					k++;
			}

			fprintf(fp, ";%u", k);

			if(k)
			{
				for(j = 0; j < MAX_CITYFIELD; j++)
				{
					if(arena->fields[i].city[j])
						fprintf(fp, ";%u", arena->fields[i].city[j]->number);
				}
			}

//			for(j = 0; j < maxplanes; j++)
//			{
//				fprintf(fp, ";%d", arena->fields[i].rps[j]);
//			}

			for(j = 0; j < MAX_BUILDINGS; j++)
			{
				if(arena->fields[i].buildings[j].field)
					fprintf(fp, ";%u;%u;%u;%u;%u;%d;%u;%d;%d;%d",
			   			arena->fields[i].buildings[j].field,
					    arena->fields[i].buildings[j].country,
					    arena->fields[i].buildings[j].id,
					    arena->fields[i].buildings[j].type,
					    arena->fields[i].buildings[j].status,
					    arena->fields[i].buildings[j].timer,
					    arena->fields[i].buildings[j].armor,
					    arena->fields[i].buildings[j].posx,
					    arena->fields[i].buildings[j].posy,
					    arena->fields[i].buildings[j].posz);
				else
				{
					break;
				}
			}
			fprintf(fp, "\n");
		}

		for(i = 0; i < cities->value; i++)
		{
			fprintf(fp, "%u;%d;%d;%d;%u;%u;%s;%u",
						arena->cities[i].type,
						arena->cities[i].posxyz[0],
						arena->cities[i].posxyz[1],
						arena->cities[i].posxyz[2],
						arena->cities[i].country,
						arena->cities[i].closed,
						arena->cities[i].name[0]?arena->cities[i].name:"unamed",
						arena->cities[i].needtoclose);

			for(j = 0; j < MAX_BUILDINGS; j++)
			{
				if(arena->cities[i].buildings[j].field)
					fprintf(fp, ";%u;%u;%u;%u;%u;%d;%u;%d;%d;%d",
			   		 arena->cities[i].buildings[j].field,
					 arena->cities[i].buildings[j].country,
					 arena->cities[i].buildings[j].id,
					 arena->cities[i].buildings[j].type,
					 arena->cities[i].buildings[j].status,
					 arena->cities[i].buildings[j].timer,
					 arena->cities[i].buildings[j].armor,
					 arena->cities[i].buildings[j].posx,
					 arena->cities[i].buildings[j].posy,
					 arena->cities[i].buildings[j].posz);
				else
				{
					break;
				}
			}
			fprintf(fp, "\n");
		}
	}

	SavePlanesPool(filename, client);

	if(wb3->value)
	{
		sprintf(file, "./arenas/%s/arena.topo", dirname->string);
		SaveEarthMap(file);
	}

	fclose(fp);
}

/*************
LoadPlanesPool

Load planes pool
*************/

void LoadPlanesPool(char *filename, client_t *client)
{
	u_int8_t i, j;
	char file[128];
	char buffer[8192];
	FILE *fp;

	strcpy(file, filename);

	strcat(file, ".pool");

	if(!(fp = fopen(file, "r")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: LoadPlanesPool() Couldn't open file \"%s\"", file);
	}
	else
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Loading planes pool from \"%s\"", file);

		for(i = 0; i < fields->value; i++)
		{
			memset(buffer, 0, sizeof(buffer));
			if(!fgets(buffer, sizeof(buffer), fp))
			{
				PPrintf(client, RADIO_YELLOW, "WARNING: Unexpected end of %s", file);
				fclose(fp);
				return;
			}
			else
			{
				arena->fields[i].rps[0] = Com_Atoi((char *)strtok(buffer, ";"));

				for(j = 1; j < maxplanes; j++)
				{
					arena->fields[i].rps[j] = Com_Atoi((char *)strtok(NULL, ";"));
				}
			}
		}
		fclose(fp);
	}
}


/*************
SavePlanesPool

Save planes pool
*************/

void SavePlanesPool(char *filename, client_t *client)
{
	u_int8_t i, j;
	char file[128];
	FILE *fp;

	strcpy(file, filename);
	strcat(file, ".pool");

	if(!(fp = fopen(file, "w")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: SavePlanesPool() Cannot open file \"%s\"", file);
		return;
	}
	else
	{
		for(i = 0; i < fields->value; i++)
		{
			for(j = 0; j < maxplanes; j++)
			{
				fprintf(fp, "%d;", arena->fields[i].rps[j]);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

/*************
GetBuildingArmor

Get the value of armor for given structure
*************/

u_int32_t GetBuildingArmor(u_int8_t type, client_t *client)
{
	if(type >= BUILD_MAX)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "type unknown %d", type);
		return 0;
	}

	return arena->buildarmor[type].points;
}

/*************
GetBuildingAPstop

Get the value of AP-stop for given structure
*************/

u_int32_t GetBuildingAPstop(u_int8_t type, client_t *client)
{
	if(type >= BUILD_MAX)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "type unknown %d", type);
		return 0;
	}

	return arena->buildarmor[type].apstop;
}

/*************
GetBuildingImunity

Get the value of imunity for given structure
*************/

u_int32_t GetBuildingImunity(u_int8_t type, client_t *client)
{
	if(type >= BUILD_MAX)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "type unknown %d", type);
		return 0;
	}

	return arena->buildarmor[type].imunity;
}

/*************
SendMapDots

Send dots at radar
*************/

void SendMapDots(void)
{
	u_int8_t i, j, k, country;
	radardot1_t *radar1;
	radardot2_t *radar2;
	u_int8_t buffer[59];

	radar1 = (radardot1_t *) buffer;

	memset(buffer, 0, sizeof(buffer));

	for(country = 1; country <= 4; country++)
	{
		for(i = 0, j = 0; i < maxentities->value; i++)
		{
			if(j >= 8 || i == maxentities->value - 1) // send dots
			{
				radar1->packetid = htons(Com_WBhton(0x0005));
				radar1->numdots = j;

				for(k = 0; k < maxentities->value; k++)
				{
					if(clients[k].inuse && !clients[k].drone && clients[k].ready && (clients[k].attr == 1 || ((!clients[k].infly || clients[k].obradar) && clients[k].country == country)))
					{
						if(clients[k].mapdots)
							ClearMapDots(&clients[k]);

						if(j != 0)
						{
							clients[k].mapdots = 1;
							SendPacket(buffer, 3+(7*j), &clients[k]);
						}
					}
				}

				memset(buffer, 0, sizeof(buffer));
				j = 0;
			}

			if(clients[i].inuse && clients[i].ready && clients[i].infly) // mount dot packet
			{
				if((clients[i].country == country) && iff->value)
				{
					radar2 = (radardot2_t *)(buffer+3+(7*j));
					radar2->slot = htons(i);
					radar2->posx = htons((float) clients[i].posxy[0][0] / (312 * 3.28));
					radar2->posy = htons((float) clients[i].posxy[1][0] / (312 * 3.28));
					radar2->country = clients[i].country;
					j++;
				}
				else if(SeeEnemyDot(&clients[i], country))
				{
					radar2 = (radardot2_t *)(buffer+3+(7*j));
					radar2->slot = htons(i);
					radar2->posx = htons((float) clients[i].posxy[0][0] / (312 * 3.28));
					radar2->posy = htons((float) clients[i].posxy[1][0] / (312 * 3.28));
					if(iff->value)
						radar2->country = clients[i].country;
					else
						radar2->country = 0;
					j++;
				}
			}
		}
	}
}

/*************
SeeEnemyDot

Check if enemy dot is in radar range
*************/

u_int8_t SeeEnemyDot(client_t *client, u_int8_t country)
{
	u_int16_t i, j, k;
	u_int32_t range;
	int32_t x, y, z;

	range = country == 1?radarrange1->value:country == 2?radarrange2->value:country == 3?radarrange3->value:country == 4?radarrange4->value:0;

	range /= 2;

	for(i = 0, j = 0; i < fields->value; i++) // check if any Field gets the dot in radar
	{
		if(!j)
		{
			if(arena->fields[i].country == country)
			{
				for(k = 0; k < MAX_BUILDINGS; k++)
				{
					if(!arena->fields[i].buildings[k].field)
					{
						break;
					}
					else if((arena->fields[i].buildings[k].type == BUILD_RADAR)/* || arena->fields[i].type == FIELD_CV || arena->fields[i].type == FIELD_CARGO || arena->fields[i].type == FIELD_DD || arena->fields[i].type == FIELD_SUBMARINE*/)
					{
						if(!arena->fields[i].buildings[k].status/* || arena->fields[i].type == FIELD_CV || arena->fields[i].type == FIELD_CARGO || arena->fields[i].type == FIELD_DD || arena->fields[i].type == FIELD_SUBMARINE*/)
						{
							x = (arena->fields[i].posxyz[0] - client->posxy[0][0]) / 22;
							y = (arena->fields[i].posxyz[1] - client->posxy[1][0]) / 22;
							z = client->posalt[0] - arena->fields[i].posxyz[2];

							if(x > -46340 && x < 46340 && y > -46340 && y < 46340 && z > (arena->fields[i].posxyz[2] + radaralt->value) && z < (arena->fields[i].posxyz[2] + radarheight->value))
							{
								if(IsVisible(client->posxy[0][0], client->posxy[1][0], client->posalt[0] ,arena->fields[i].posxyz[0], arena->fields[i].posxyz[1], arena->fields[i].posxyz[2]))
								{
									if(sqrt(Com_Pow(x, 2) + Com_Pow(y, 2)) < (range/11))// && !(client->atradar & 0x10)) // commented to implement max/min alt
									{
										j = 1;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	if(!j) // if no Field got the dot, check if Cities does
	{
		for(i = 0, j = 0; i < cities->value; i++)
		{
			if(!j)
			{
				if(arena->cities[i].country == country)
				{
					for(k = 0; k < MAX_BUILDINGS; k++)
					{
						if(!arena->cities[i].buildings[k].field)
						{
							break;
						}
						else if((arena->cities[i].buildings[k].type == BUILD_RADAR))
						{
							if(!arena->cities[i].buildings[k].status)
							{
								x = (arena->cities[i].posxyz[0] - client->posxy[0][0]) / 22;
								y = (arena->cities[i].posxyz[1] - client->posxy[1][0]) / 22;
								z = client->posalt[0] - arena->cities[i].posxyz[2];
	
								if(x > -46340 && x < 46340 && y > -46340 && y < 46340 && z > (arena->cities[i].posxyz[2] + radaralt->value) && z < (int32_t)range)
								{
									if(IsVisible(client->posxy[0][0], client->posxy[1][0], client->posalt[0] ,arena->cities[i].posxyz[0], arena->cities[i].posxyz[1], arena->cities[i].posxyz[2]))
									{
										if(sqrt(Com_Pow(x, 2) + Com_Pow(y, 2)) < (range/11))// && !(client->atradar & 0x10)) // commented to implement max/min alt
										{
											j = 1;
										}
									}
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	
	if(!j) // Still not found? Check if any Fleet got it
	{
		range = country == 1?cvradarrange1->value:country == 2?cvradarrange2->value:country == 3?cvradarrange3->value:country == 4?cvradarrange4->value:0;
	
		range /= 2;
		
		for(i = 0, j = 0; i < cvs->value; i++)
		{
			k = fields->value - i - 1;
			
			if(!j)
			{
				if(arena->fields[k].country == country)
				{
					if(arena->fields[k].type >= FIELD_CV || arena->fields[k].type <= FIELD_SUBMARINE)
					{
						x = (arena->fields[k].posxyz[0] - client->posxy[0][0]) / 22;
						y = (arena->fields[k].posxyz[1] - client->posxy[1][0]) / 22;
						z = client->posalt[0] - arena->fields[k].posxyz[2];

						if(x > -46340 && x < 46340 && y > -46340 && y < 46340 && z > (arena->fields[k].posxyz[2] + radaralt->value) && z < (arena->fields[k].posxyz[2] + radarheight->value))
						{
							if(IsVisible(client->posxy[0][0], client->posxy[1][0], client->posalt[0] ,arena->fields[k].posxyz[0], arena->fields[k].posxyz[1], arena->fields[k].posxyz[2]))
							{
								if(sqrt(Com_Pow(x, 2) + Com_Pow(y, 2)) < (range/11))// && !(client->atradar & 0x10)) // commented to implement max/min alt
								{
									j = 1;
								}
							}
						}
					}
				}
			}
		}
	}

	if(!j) // Still not found... our last chance is check if any obRadar got it
	{
		for(i = 0; i < maxentities->value; i++)
		{
			if(clients[i].inuse && !clients[i].drone && clients[i].infly && clients[i].obradar && clients[i].country == country)
			{
				x = (client->posxy[0][0] - clients[i].posxy[0][0]) / 10;
				y = (client->posxy[1][0] - clients[i].posxy[1][0]) / 10;
				z = (client->posalt[0] - clients[i].posalt[0]) / 10;

				if(x > -46340 && x < 46340 && y > -46340 && y < 46340 && z > (clients[i].obradar / -10) && z < (clients[i].obradar / 10))
				{
					if(sqrt(Com_Pow(x, 2) + Com_Pow(y, 2)) < (clients[i].obradar / 10))
						j = 1;
				}
				break;
			}
		}
	}


	return j;
}

/*************
ClearMapDots

Clear dots at radar
*************/

void ClearMapDots(client_t *client)
{
	u_int8_t buffer[3];

	buffer[0] = 0x21;
	buffer[1] = 0x06;
	buffer[2] = 0;

	client->mapdots = 0;

	SendPacket(buffer, 3, client);
}

/*************
SendCVPos

Send Current CV Pos
*************/

void SendCVPos(client_t *client, u_int8_t cvnum)
{
	cvpos_t *cvpos;
	u_int8_t buffer[16];

	cvpos = (cvpos_t *) buffer;

	memset(buffer, 0, sizeof(buffer));

	cvpos->packetid = htons(Com_WBhton(0x0302));
	cvpos->number = htons(cvnum);
	cvpos->posx = htonl(GetCVPos(&(arena->cv[cvnum]), 0));
	cvpos->posy = htonl(GetCVPos(&(arena->cv[cvnum]), 1));

	SendPacket(buffer, sizeof(buffer), client);
}

/*************
ResetCVPos

Reset CV back to starting point (port)
*************/

void ResetCVPos(cv_t *cv)
{
	u_int16_t i;
	
	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(!arena->fields[cv->field].buildings[i].field)
		{
			break;
		}
		else
		{
			arena->fields[cv->field].buildings[i].status = 1;
			arena->fields[cv->field].buildings[i].timer = 0;
		}
	}
	
	cv->timebase = 0;
	cv->threatened = 0;
	cv->outofport = 0;
	cv->wpnum = 0;
	cv->speed = cvspeed->value;
	sprintf(cv->logfile, "%s,cv%u,%s,%u", mapname->string, cv->id, GetCountry(arena->fields[cv->field].country), (u_int32_t)time(NULL));
//	arena->cv[j].stuck = 0;
//	arena->fields[cv->field].posxyz[0] = cv->wp[0][0];
//	arena->fields[cv->field].posxyz[1] = cv->wp[0][1];

	
}

/*************
SetCVSpeed

Set convoy speeds when some boat is damaged or reached waypoint
*************/

void SetCVSpeed(cv_t *cv)
{
	int16_t i;

	if((cv->wp[cv->wpnum][0] == arena->fields[cv->field].posxyz[0]) && (cv->wp[cv->wpnum][1] == arena->fields[cv->field].posxyz[1])) // if recalc cv route in the same pos as current WP (when CV hit, etc)
	{
		cv->wpnum++;

		if(!cv->outofport)
		{
			arena->fields[cv->field].posxyz[0] = cv->wp[0][0];
			arena->fields[cv->field].posxyz[1] = cv->wp[0][1];
		}

		if(cv->wpnum == cv->wptotal) // reset waypoint index
		{
			cv->wpnum = 1;
		}
	}

	SetCVRoute(cv);

	for(i = 0; i < maxentities->value; i++)
	{
		if(clients[i].inuse && !clients[i].drone && clients[i].ready)
		{
			SendCVRoute(&clients[i], cv->id);

			if(!cv->outofport) // send cv pos when it return to base (e.g. capt cv)
			{
				SendCVPos(&clients[i], cv->id);
			}
		}
	}
	
// debug
	
	Com_Printf("DEBUG: CV%u %u;%u;%u;%d;%d;%d;%d\n",
		cv->id,
		cv->wpnum,
		arena->time,
		cv->timebase,
		cv->wp[cv->wpnum][0],
		cv->wp[cv->wpnum][1],
		GetCVPos(cv, 0),
		GetCVPos(cv, 1));
	
//	BPrintf(RADIO_RED, "DEBUG: timebase CV %d - %u", cv->field+1, cv->timebase);
	cv->outofport = 1;
}

/*************
GetCVTimebase

Get current timebase to reach next waypoint
*************/

u_int32_t GetCVTimebase(cv_t *cv)
{
	u_int32_t timebase;
	
	if(cv->wpnum >= cv->wptotal)
	{
		Com_Printf("WARNING: GetCVTimebase() CV%d wpnum >= wptotal\n", cv->id);
		timebase = (u_int32_t) arena->time + 10000; // return 10 seconds ahead
	}
	else
	{
		timebase = (u_int32_t) arena->time + ((sqrt(Com_Pow(cv->wp[cv->wpnum][0] - arena->fields[cv->field].posxyz[0], 2) + Com_Pow(cv->wp[cv->wpnum][1] - arena->fields[cv->field].posxyz[1], 2)) * 1000) / (cv->speed + 0.001));
		
		if(timebase == arena->time)
		{
			Com_Printf("WARNING: GetCVTimebase() CV%d timebase is zero (%u) X' %d X'' %d\n", cv->id, cv->wp[cv->wpnum][0], arena->fields[cv->field].posxyz[0]); // If stills this message, change == to aprox in SetCVSpeed()
			timebase = (u_int32_t) arena->time + 10000; // return 10 seconds ahead
		}
	}
	
	if(timebase < arena->time)
	{
		Com_Printf("DEBUG: error setting timebase t %u; a %u CV%u %d;%d;%d;%d\n", timebase, arena->time, cv->id, cv->wp[cv->wpnum][0], cv->wp[cv->wpnum][1], arena->fields[cv->field].posxyz[0], arena->fields[cv->field].posxyz[1]);
	}
	
	return timebase;
}

/*************
GetCVSpeeds

Get X or Y speed based on current route (current pos and destiny)
*************/

double GetCVSpeeds(cv_t *cv, u_int8_t xy)
{
	double prop;

	prop = (double)cv->speed / sqrt(Com_Pow(cv->wp[cv->wpnum][0] - arena->fields[cv->field].posxyz[0], 2) + Com_Pow(cv->wp[cv->wpnum][1] - arena->fields[cv->field].posxyz[1], 2));

	return (double)(cv->wp[cv->wpnum][xy] - arena->fields[cv->field].posxyz[xy]) * prop;
}

/*************
SetCVRoute

Set CV Route (timebase and XY speeds)
*************/

void SetCVRoute(cv_t *cv)
{
	cv->xyspeed[0] = GetCVSpeeds(cv, 0);
	cv->xyspeed[1] = GetCVSpeeds(cv, 1);
	cv->timebase = GetCVTimebase(cv);
}

/*************
ChangeCVRoute

Change Route of CV, in threathness or by command
*************/

void ChangeCVRoute(cv_t *cv, double angle /*0*/, u_int16_t distance /*10000*/,  client_t *client)
{
	u_int8_t lastwp;
	int8_t angleoffset = 0;

	if(cv->wpnum >= cv->wptotal)
	{
		Com_Printf("WARNING: ChangeCVRoute() wpnum >= wptotal\n");
		cv->wpnum = 1;
	}
	
	cv->threatened = 1;

	if(cv->wpnum == 1)
	{
		lastwp = cv->wptotal - 1;
	}
	else
		lastwp = cv->wpnum - 1;

	if(!client)
	{
		angle = AngleTo(arena->fields[cv->field].posxyz[0], arena->fields[cv->field].posxyz[1], cv->wp[cv->wpnum][0], cv->wp[cv->wpnum][1]);

		if(rand()%100 < 60) // zigzag
		{
			angleoffset = 45 * Com_Pow(-1, cv->zigzag);

			if(cv->zigzag == 1)
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
		
		if(GetHeightAt(arena->fields[cv->field].posxyz[0] - (10000 * sin(Com_Rad(angle + angleoffset))),
			arena->fields[cv->field].posxyz[1] + (10000 * cos(Com_Rad(angle + angleoffset))))) // WP is over land
		{
			angleoffset *= -1;
		}
		
		angle += angleoffset;
		distance = 2000;		
	}

	// defines which waypoint will be changed
	// if dist to next waypoint < 2000, dont backward wp counter
	if(DistBetween(cv->wp[cv->wpnum][0], cv->wp[cv->wpnum][1], 0, arena->fields[cv->field].posxyz[0], arena->fields[cv->field].posxyz[1], 0, 2000) >= 0)
	{
		lastwp = cv->wpnum;
	}
	else
		cv->wpnum = lastwp;

	cv->wp[lastwp][0] = arena->fields[cv->field].posxyz[0] - (distance * sin(Com_Rad(angle)));
	cv->wp[lastwp][1] = arena->fields[cv->field].posxyz[1] + (distance * cos(Com_Rad(angle)));

	if(client)
	{
		PPrintf(client, RADIO_YELLOW, "Waypoint changed to %s", Com_Padloc(cv->wp[lastwp][0], cv->wp[lastwp][1]));
		PPrintf(client, RADIO_YELLOW, "ETA: %s\"", Com_TimeSeconds(distance / cv->speed));
	}	
	
	SetCVSpeed(cv);
	
	// configure to next wpnum be that nearest to cv->wp[lastwp][0]
	// coded at threatened = 0;
}

/*************
GetCVPos

Get current XY CV pos based in estimated time to reach next waypoint and current XY speed
*************/

int32_t GetCVPos(cv_t *cv, u_int8_t xy)
{
	int32_t offset;
	
	if(cv->wpnum >= cv->wptotal)
	{
		Com_Printf("WARNING: GetCVPos() wpnum >= wptotal\n");
		cv->wpnum = 1;
	}
	
	offset = (int32_t)((double)((cv->timebase - arena->time) / 1000) * cv->xyspeed[xy]);
	
	return (int32_t)(cv->wp[cv->wpnum][xy] - offset);
}

/*************
SendCVRoute

Send CV Route to client
*************/

void SendCVRoute(client_t *client, u_int8_t cvnum)
{
	cvroute_t *cvroute;
	u_int8_t buffer[36];

	cvroute = (cvroute_t *) buffer;

	memset(buffer, 0, sizeof(buffer));
	
/*	if(!client->arenafieldsok)
	{
		Com_Printf("DEBUG: %s CV%u %u;%u;%u;%d;%d;%d;%d\n",
			client->longnick,
			cvnum,
			arena->cv[cvnum].wpnum,
			arena->time,
			arena->cv[cvnum].timebase,
			arena->cv[cvnum].wp[arena->cv[cvnum].wpnum][0],
			arena->cv[cvnum].wp[arena->cv[cvnum].wpnum][1],
			GetCVPos(&(arena->cv[cvnum]), 0),
			GetCVPos(&(arena->cv[cvnum]), 1));
	}
*/
	cvroute->packetid = htons(Com_WBhton(0x0303));
	cvroute->number = htons(cvnum);
	cvroute->basetime = htonl(arena->time);
	cvroute->triptime = htonl(arena->cv[cvnum].timebase);
	cvroute->destx = htonl(arena->cv[cvnum].wp[arena->cv[cvnum].wpnum][0]);
	cvroute->desty = htonl(arena->cv[cvnum].wp[arena->cv[cvnum].wpnum][1]);
	cvroute->posx = htonl(GetCVPos(&(arena->cv[cvnum]), 0));
	cvroute->posy = htonl(GetCVPos(&(arena->cv[cvnum]), 1));

	SendPacket(buffer, sizeof(buffer), client);
}

/*************
ReadCVWaypoints

Read CV waypoints from file
*************/

void ReadCVWaypoints(u_int8_t num)
{
	char file[32];
	u_int8_t i;
	FILE *fp;
	char buffer[128];

	sprintf(file, "./arenas/%s/cv%d.rte", dirname->string, num);

	arena->cv[num].wptotal = 0;

	if(!(fp = fopen(file, "r")))
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() Cannot open file \"%s\"", file);
		return;
	}

	for(i = 0; fgets(buffer, sizeof(buffer), fp); i++)
	{
		arena->cv[num].wp[i][0] = Com_Atoi((char *)strtok(buffer, ";"));
		arena->cv[num].wp[i][1] = Com_Atoi((char *)strtok(NULL, ";"));

		arena->cv[num].wptotal++;

		if(arena->cv[num].wptotal >= MAX_WAYPOINTS)
			break;

		memset(buffer, 0, sizeof(buffer));
	}

	if(!arena->cv[num].wptotal)
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() error reading \"%s\"", file);
	}

	fclose(fp);
}


/*************
LogCVsPosition

Log all CVs position
*************/

void LogCVsPosition(void)
{
	u_int8_t i;
	FILE *fp;
	char filename[128];
	
	for(i = 0; i < cvs->value; i++)
	{
		sprintf(filename, "./logs/%s.cvl", arena->cv[i].logfile);	
		
		if(!(fp = fopen(filename, "a")))
		{
			Com_Printf("WARNING: Couldn't append file \"%s\"\n", filename);
		}
		else
		{
			fprintf(fp, "%d;%d;%f;%f;%u;%u;%u\n",
					arena->fields[arena->cv[i].field].posxyz[0],
					arena->fields[arena->cv[i].field].posxyz[1],
					arena->cv[i].xyspeed[0],
					arena->cv[i].xyspeed[1],
					arena->cv[i].threatened,
					arena->fields[arena->cv[i].field].country,
					(u_int32_t)time(NULL));
			fclose(fp);
		}
	}
}

/*************
GetPlaneName

Get plane name from given number
*************/

char *GetPlaneName(u_int16_t plane)
{
	if(plane < maxplanes)
		return arena->planedamage[plane].name;
	else
		return "Invalid Plane";
}

/*************
GetSmallPlaneName

Get plane abreviated name from given number
*************/

char *GetSmallPlaneName(u_int16_t plane)
{
	if(plane < maxplanes)
		return arena->planedamage[plane].abbrev;
	else
		return "Invalid Plane";
}

/*************
UpdateRPS

Update field with new planes
*************/

void UpdateRPS(void)
{
	time_t tdate;
	struct tm timestr;
	struct tm *timeptr;
	u_int8_t i, j;
	u_int32_t basedate, lagdate[4];

	timestr.tm_sec = 0;
	timestr.tm_min = 0;
	timestr.tm_hour = 0;
	timestr.tm_mday = arena->day;
	timestr.tm_mon = arena->month - 1;
	timestr.tm_year = arena->year - 1860;
	timestr.tm_wday = 0;
	timestr.tm_yday = 0;
	timestr.tm_isdst = 0;

	tdate = mktime(&timestr);

	for(i = 0; i < 4; i++)
	{
		tdate -= GetRPSLag(i + 1) * 86400;
		timeptr = localtime(&tdate);
		lagdate[i] = ((timeptr->tm_year + 1860) * 10000)+((timeptr->tm_mon + 1) * 100) + timeptr->tm_mday;
	}

	basedate = (arena->year * 10000)+(arena->month * 100) + arena->day;

	for(i = 0; i < fields->value; i++)
	{
		for(j = 0; j < maxplanes; j++)
		{
			if(arena->rps[j].used)
			{
				if(((arena->fields[i].country == 1 && arena->rps[j].country & 0x01) || (arena->fields[i].country == 2 && arena->rps[j].country & 0x02) || (arena->fields[i].country == 3 && arena->rps[j].country & 0x04) || (arena->fields[i].country == 4 && arena->rps[j].country & 0x08))
				    && ((arena->rps[j].in <= basedate && arena->rps[j].out > basedate) || (!arena->rps[j].in && !arena->rps[j].out)))
				{
					arena->fields[i].rps[j] = arena->rps[j].pool[arena->fields[i].type - 1];
				}
				else if(arcade->value)
				{
					arena->fields[i].rps[j] = 0;
				}
			}
			else
			{
				if(((arena->fields[i].country == 1 && arena->rps[j].country & 0x01) || (arena->fields[i].country == 2 && arena->rps[j].country & 0x02) || (arena->fields[i].country == 3 && arena->rps[j].country & 0x04) || (arena->fields[i].country == 4 && arena->rps[j].country & 0x08))
				    && ((arena->rps[j].in <= lagdate[arena->fields[i].country - 1] && arena->rps[j].out > lagdate[arena->fields[i].country - 1]) || (!arena->rps[j].in && !arena->rps[j].out)))
				{
					arena->rps[j].used = 1;
					arena->fields[i].rps[j] = arena->rps[j].pool[arena->fields[i].type - 1];
				}
				else if(arcade->value)
				{
					arena->fields[i].rps[j] = 0;
				}
			}
		}
	}
	/*

	date = (arena->year * 10000)+(arena->month * 100) + arena->day;
	for(i = 0; i < fields->value; i++)
	{
		for(j = 0; j < maxplanes; j++)
		{
			if(((arena->fields[i].country == 1 && arena->rps[j].country & 0x01) || (arena->fields[i].country == 2 && arena->rps[j].country & 0x02) || (arena->fields[i].country == 3 && arena->rps[j].country & 0x04) || (arena->fields[i].country == 4 && arena->rps[j].country & 0x08))
			    && ((arena->rps[j].in <= date && arena->rps[j].out > date) || (!arena->rps[j].in && !arena->rps[j].out)))
			{
				arena->fields[i].rps[j] = arena->rps[j].pool[arena->fields[i].type - 1];
			}
		}
	}
	*/
}


/*************
SendRPS

Send available planes for actual field
*************/

void SendRPS(client_t *client)
{
	u_int8_t buffer[512];
	u_int8_t i;
	sendrps_t *sendrps;

	memset(buffer, 0, sizeof(buffer));

	sendrps = (sendrps_t *)buffer;
	
	sendrps->packetid = htons(Com_WBhton(0x1D0F));
	sendrps->amount = maxplanes;

	if(wb3->value)
		i = 1;
	else
		i = 0;
	
	for(i = 1; i < maxplanes; i++)
	{
		if(arena->fields[client->field - 1].rps[i] > 0 || arena->fields[client->field - 1].rps[i] == -1 || (i >= 131 && i <=134 && wb3->value))
		{
			if(wb3->value)
				buffer[i+2] = 0;
			else
				buffer[i+3] = 0;
		}
		else
		{
			if(wb3->value)
				buffer[i+2] = 1;
			else
				buffer[i+3] = 1;
		}
	}
	
	SendPacket(buffer, maxplanes+3, client);
}

/*************
WB3SendAcks

Send available acks for actual field
*************/

void WB3SendAcks(client_t *client)
{
	u_int8_t buffer[3000];
	u_int16_t i, j;
	u_int8_t numacks;

	if(!client->field)
		return;

	for (i = 0, j = 0; i < MAX_BUILDINGS; i++)
	{
		if(arena->fields[client->field - 1].buildings[i].field)
		{
			if(arena->fields[client->field - 1].buildings[i].type >= BUILD_50CALACK && 
				arena->fields[client->field - 1].buildings[i].type <= BUILD_88MMFLAK)
			{
				j++;
			}
		}
	}

	if(!(numacks = j))
		return;

	*(u_int16_t *)buffer = htons(Com_WBhton(0x1D12));

	*(buffer + 2) = numacks;
	*(buffer + numacks + 3) = numacks;
	*(buffer + numacks + 4 + (numacks * 4)) = numacks;

	for (i = 0, j = 0; i < MAX_BUILDINGS; i++)
	{
		if(arena->fields[client->field - 1].buildings[i].field)
		{
			if(arena->fields[client->field - 1].buildings[i].type >= BUILD_50CALACK && 
				arena->fields[client->field - 1].buildings[i].type <= BUILD_88MMFLAK)
			{
				*(buffer + 3 + j) = arena->fields[client->field - 1].buildings[i].status?0:1;

				switch(arena->fields[client->field - 1].buildings[i].type)
				{
					case BUILD_50CALACK:
						*(u_int32_t *)(buffer + numacks + 4 + (j * 4)) = htonl(131);
						break;
					case BUILD_20MMACK:
						*(u_int32_t *)(buffer + numacks + 4 + (j * 4)) = htonl(132);
						break;
					case BUILD_40MMACK:
						*(u_int32_t *)(buffer + numacks + 4 + (j * 4)) = htonl(133);
						break;
					case BUILD_88MMFLAK:
						*(u_int32_t *)(buffer + numacks + 4 + (j * 4)) = htonl(134);
						break;
					default:
						*(u_int32_t *)(buffer + numacks + 4 + (j * 4)) = htonl(131);
						break;
				}
				*(u_int32_t *)(buffer + numacks + 5 + (numacks * 4) + (j * 4)) = htonl(arena->fields[client->field - 1].buildings[i].id);
				j++;
			}
		}
	}

	SendPacket(buffer, (numacks * 9) + 5, client);
}

/*************
AddBomb

Adds a new bomb to bombs array
*************/

void AddBomb(u_int16_t id, int32_t destx, int32_t desty, u_int8_t type, int16_t speed, u_int32_t timer, client_t *client)
{
	u_int16_t i = 0;

	if(timer < 12000)
	{
		for(i = 0; i < MAX_BOMBS; i++)
		{
			if(!arena->bombs[i].id)	// free slot
			{
				if(speed < 0)
					speed *= -1;

				arena->bombs[i].id = id;
				arena->bombs[i].destx = destx;
				arena->bombs[i].desty = desty;
				arena->bombs[i].type = type;
				arena->bombs[i].speed = speed;
				arena->bombs[i].timer = timer;
				arena->bombs[i].from = client;
				return;
			}
		}
	}
	else
		Com_Printf("WARNING: AddBomb() timer > 120 sec (%d)\n, timer/100");

	if(i == MAX_BOMBS)
	{
		Com_Printf("WARNING: AddBomb() no slot available\n");
		PPrintf(client, RADIO_LIGHTYELLOW, "AddBomb() Couldn't create a new bomb");
	}
}

/*************
LoadRPS

Load RPS from file
*************/

void LoadRPS(char *path, client_t *client)
{
	u_int8_t i, j;
	char file[128];
	char buffer[4096];
	FILE *fp;

	strcpy(file, path);
	strcat(file, ".rps");

	strcat(file, ".LOCK");

	Sys_WaitForLock(file);

	if(Sys_LockFile(file) < 0)
		return;

	file[strlen(file) - 5] = '\0';

	if(!(fp = fopen(file, "r")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: LoadRPS() Cannot open file \"%s\"", file);
		strcat(file, ".LOCK");
		Sys_UnlockFile(file);
		return;
	}

	for(i = 0; i < fields->value; i++)
	{
		for(j = 0; j < maxplanes; j++)
			arena->fields[i].rps[j] = 0;
	}

	for(i = 0; i < maxplanes; i++)
	{
		memset(buffer, 0, sizeof(buffer));
		if(!fgets(buffer, sizeof(buffer), fp))
		{
			PPrintf(client, RADIO_YELLOW, "WARNING: Unexpected end of %s", file);
			return;
		}
		else
		{
			arena->rps[i].used = 0;
			arena->rps[i].country = Com_Atoi((char *)strtok(buffer, ";"));
			arena->rps[i].in = Com_Atoi((char *)strtok(NULL, ";"));
			arena->rps[i].out = Com_Atoi((char *)strtok(NULL, ";"));
			arena->rps[i].pool[0] = Com_Atoi((char *)strtok(NULL, ";")); // Small
			arena->rps[i].pool[1] = Com_Atoi((char *)strtok(NULL, ";")); // Medium
			arena->rps[i].pool[2] = Com_Atoi((char *)strtok(NULL, ";")); // Big
			arena->rps[i].pool[3] = Com_Atoi((char *)strtok(NULL, ";")); // CV
			arena->rps[i].pool[4] = Com_Atoi((char *)strtok(NULL, ";")); // Cargo
			arena->rps[i].pool[5] = Com_Atoi((char *)strtok(NULL, ";")); // DD
			arena->rps[i].pool[6] = Com_Atoi((char *)strtok(NULL, ";")); // Submarine
			arena->rps[i].pool[7] = Com_Atoi((char *)strtok(NULL, ";")); // Radar
			arena->rps[i].pool[8] = Com_Atoi((char *)strtok(NULL, ";")); // Bridge
			arena->rps[i].pool[9] = Com_Atoi((char *)strtok(NULL, ";")); // City
			arena->rps[i].pool[10] = Com_Atoi((char *)strtok(NULL, ";")); // Port
			arena->rps[i].pool[11] = Com_Atoi((char *)strtok(NULL, ";")); // Convoy
			arena->rps[i].pool[12] = Com_Atoi((char *)strtok(NULL, ";")); // Factory
			arena->rps[i].pool[13] = Com_Atoi((char *)strtok(NULL, ";")); // Refinery
			arena->rps[i].pool[14] = Com_Atoi((char *)strtok(NULL, ";")); // Openfield
			arena->rps[i].pool[15] = Com_Atoi((char *)strtok(NULL, ";")); // Post
			arena->rps[i].pool[16] = Com_Atoi((char *)strtok(NULL, ";")); // Village
			arena->rps[i].pool[17] = Com_Atoi((char *)strtok(NULL, ";")); // Town
		}
	}

	fclose(fp);

	PPrintf(client, RADIO_LIGHTYELLOW, "Loading RPS from \"%s\"", file);

	strcat(file, ".LOCK");
	Sys_UnlockFile(file);
}

/*************
SaveRPS

Save curren RPS in file
FIXME: Function not used for a while
*************/

void SaveRPS(char *path, client_t *client)
{
	char	file[128];
	u_int8_t i;
	FILE	*fp;

	strcpy(file, path);
	strcat(file, ".rps");

	strcat(file, ".LOCK");

	Sys_WaitForLock(file);

	if(Sys_LockFile(file) < 0)
		return;

	file[strlen(file) - 5] = '\0';

	if(!(fp = fopen (file, "w")))
	{
		Com_Printf("WARNING: Error opening %s\n", path);
		strcat(file, ".LOCK");
		Sys_UnlockFile(file);
		return;
	}

	for(i = 0; i < maxplanes; i++)
	{
		fprintf(fp, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
			arena->rps[i].country,
		    arena->rps[i].in,
		    arena->rps[i].out,
		    arena->rps[i].pool[0],
		    arena->rps[i].pool[1],
		    arena->rps[i].pool[2],
		    arena->rps[i].pool[3],
		    arena->rps[i].pool[4],
		    arena->rps[i].pool[5],
		    arena->rps[i].pool[6],
		    arena->rps[i].pool[7],
		    arena->rps[i].pool[8],
		    arena->rps[i].pool[9],
		    arena->rps[i].pool[10],
		    arena->rps[i].pool[11],
		    arena->rps[i].pool[12],
		    arena->rps[i].pool[13],
		    arena->rps[i].pool[14],
		    arena->rps[i].pool[15],
		    arena->rps[i].pool[16],
		    arena->rps[i].pool[17]);
	}

	fclose(fp);

	PPrintf(client, RADIO_LIGHTYELLOW, "RPS saved in \"%s\"", file);

	strcat(file, ".LOCK");
	Sys_UnlockFile(file);
}

/*************
ShowRPS

Show RPS configuration
*************/

void ShowRPS(client_t *client)
{
	u_int8_t i;

	for(i = 1; i < maxplanes; i++)
	{
		if(arena->rps[i].pool[0] + arena->rps[i].pool[1] + arena->rps[i].pool[2] +
			arena->rps[i].pool[3] + arena->rps[i].pool[4] + arena->rps[i].pool[5] + arena->rps[i].pool[6] +
			 arena->rps[i].pool[7] + arena->rps[i].pool[8] + arena->rps[i].pool[9] + arena->rps[i].pool[10] +
			  arena->rps[i].pool[7] + arena->rps[i].pool[8] + arena->rps[i].pool[9] + arena->rps[i].pool[10] +
			   arena->rps[i].pool[11] + arena->rps[i].pool[12] + arena->rps[i].pool[13] + arena->rps[i].pool[14] +
			    arena->rps[i].pool[15] + arena->rps[i].pool[16] + arena->rps[i].pool[17])
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "%-7s:c%d;i%d;o%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d", GetSmallPlaneName(i), arena->rps[i].country, arena->rps[i].in, arena->rps[i].out,
				arena->rps[i].pool[0],
				arena->rps[i].pool[1],
				arena->rps[i].pool[2],
				arena->rps[i].pool[3],
				arena->rps[i].pool[4],
				arena->rps[i].pool[5],
				arena->rps[i].pool[6],
				arena->rps[i].pool[7],
				arena->rps[i].pool[8],
				arena->rps[i].pool[9],
				arena->rps[i].pool[10],
				arena->rps[i].pool[11],
				arena->rps[i].pool[12],
				arena->rps[i].pool[13],
				arena->rps[i].pool[14],
				arena->rps[i].pool[15],
				arena->rps[i].pool[16],
				arena->rps[i].pool[17]);
		}

	}
}

/*************
LoadMapcycle

Load Mapcycle from file
*************/

void LoadMapcycle(char *path, client_t *client)
{
	u_int8_t i;
	char file[128];
	char buffer[4096];
	char *string;
	FILE *fp;

	strcpy(file, path);
	strcat(file, ".mpc");

	strcat(file, ".LOCK");

	Sys_WaitForLock(file);

	if(Sys_LockFile(file) < 0)
		return;

	file[strlen(file) - 5] = '\0';

	if(!(fp = fopen(file, "r")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: LoadMapcycle() Cannot open file \"%s\"", file);
		strcat(file, ".LOCK");
		Sys_UnlockFile(file);
		return;
	}

	for(i = 0; i < MAX_MAPCYCLE; i++)
	{
		arena->mapcycle[i].date = 0;
	}

	for(i = 0; i < MAX_MAPCYCLE; i++)
	{
		memset(buffer, 0, sizeof(buffer));
		if(!fgets(buffer, sizeof(buffer), fp))
		{
			if(!i)
				return;
		}
		else
		{
			string = (char *)strtok(buffer, ";");

			if(string)
			{
				strcpy(arena->mapcycle[i].mapname, string);

				string = (char *)strtok(NULL, ";");

					if(string)
						arena->mapcycle[i].date = Com_Atoi(string);
					else
						PPrintf(client, RADIO_LIGHTYELLOW, "Syntax error in %s", file);

			}
			else
				break;
		}
	}

	fclose(fp);

	PPrintf(client, RADIO_LIGHTYELLOW, "Loading mapcycle from \"%s\"", file);

	strcat(file, ".LOCK");
	Sys_UnlockFile(file);
}

/*************
IsFighter

Check if plane is fighter
*************/

int8_t IsFighter(client_t *client, ...)
{
	va_list argptr;
	u_int32_t plane = 0;
	
	if(!client)
	{
		va_start(argptr, client);
		plane = va_arg(argptr, u_int32_t);
		va_end(argptr);
		
		if(!plane) // possible error
		{
			Com_Printf("WARNING: IsFighter() plane = 0\n");
			return 0;
		}
		else if(plane < maxplanes)
		{
			if(arena->planedamage[plane].type == 1)
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}

	if(client->attached)
	{
		plane = client->attached->plane;
	}
	else
	{
		plane = client->plane;
	}

	if(plane < maxplanes)
	{
		if(arena->planedamage[plane].type == 1)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

// 61:  CHUTE
// 62:  PILOT
// 85:  JEEP
// 101: TANK
// 180: FAU


/*************
IsBomber

Check if plane is Buffer
*************/

int8_t IsBomber(client_t *client, ...)
{
	va_list argptr;
	u_int32_t plane = 0;
	
	if(!client)
	{
		va_start(argptr, client);
		plane = va_arg(argptr, u_int32_t);
		va_end(argptr);
		
		if(!plane) // possible error
		{
			Com_Printf("WARNING: IsBomber() plane = 0\n");
			return 0;
		}
		else if(plane < maxplanes)
		{
			if(arena->planedamage[plane].type == 2)
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}

	if(client->attached)
	{
		plane = client->attached->plane;
	}
	else
	{
		plane = client->plane;
	}

	if(plane < maxplanes)
	{
		if(arena->planedamage[plane].type == 2)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

/*************
IsCargo

Check if plane is a Cargo Plane / Vehicle
*************/

int8_t IsCargo(client_t *client, ...)
{
	va_list argptr;
	u_int32_t plane = 0;
	
	if(!client)
	{
		va_start(argptr, client);
		plane = va_arg(argptr, u_int32_t);
		va_end(argptr);
		
		if(!plane) // possible error
		{
			Com_Printf("WARNING: IsCargo() plane = 0\n");
			return 0;
		}
		else if(plane < maxplanes)
		{
			if(wb3->value)
			{
				switch(plane)
				{
					case 54: // ju52
					case 67: // M5
					case 79: // opel truck
					case 137: // C47
						return 1;
					default:
						return 0;
				}
			}
			else
			{
				switch(plane)
				{
					case 54: // ju52
					case 82: // Li-2
						return 1;
					default:
						return 0;
				}
			}
		}
		else
			return 0;
	}

	if(client->attached)
	{
		plane = client->attached->plane;
	}
	else
	{
		plane = client->plane;
	}

	if(wb3->value)
	{
		switch(plane)
		{
			case 54: // ju52
			case 67: // M5
			case 79: // opel truck
			case 137: // C47
				return 1;
			default:
				return 0;
		}
	}
	else
	{
		switch(plane)
		{
			case 54: // ju52
			case 82: // Li-2
				return 1;
			default:
				return 0;
		}
	}
}

/*************
IsGround

Check if plane is a Ground Vehicle
*************/

int8_t IsGround(client_t *client, ...)
{
	va_list argptr;
	u_int32_t plane = 0;
	
	if(!client)
	{
		va_start(argptr, client);
		plane = va_arg(argptr, u_int32_t);
		va_end(argptr);
		
		if(!plane) // possible error
		{
			Com_Printf("WARNING: IsGround() plane = 0\n");
			return 0;
		}
		else if(plane < maxplanes)
		{
			if(arena->planedamage[plane].type == 3)
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}

	if(client->attached)
	{
		plane = client->attached->plane;
	}
	else
	{
		plane = client->plane;
	}

	if(plane < maxplanes)
	{
		if(arena->planedamage[plane].type == 3)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

/*************
HaveGunner

Check if plane is have gunners
*************/

int8_t HaveGunner(u_int16_t plane)
{
	switch(plane)
	{
		case 15: case 16: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
		case 40: case 41: case 51: case 52: case 53: case 54: case 55: case 57: case 58: case 59:
		case 67: case 75: case 82: case 85: case 92: case 95: case 98: case 100: case 101:
		case 105: case 106: case 112: case 113: case 115:
			return 1;
		default:
			return 0;
	}
}

void LoadAmmo(client_t *client)
{
	u_int8_t i;
	int16_t num_rows;
	
	sprintf(my_query, "SELECT * FROM munitions ORDER BY id");
		
	if(!(client?Com_MySQL_Query(client, &my_sock, my_query):d_mysql_query(&my_sock, my_query)))
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if(num_rows > MAX_MUNTYPE)
					num_rows = MAX_MUNTYPE;

				for(i = 0; i < num_rows; i++)
				{
					if((my_row = mysql_fetch_row(my_result)))
					{
						arena->munition[i].he = Com_Atoi(Com_MyRow("he"));
						arena->munition[i].ap = Com_Atou(Com_MyRow("ap"));
						arena->munition[i].decay = Com_Atoi(Com_MyRow("decay"));
						arena->munition[i].type = Com_Atoi(Com_MyRow("muntype"));
						if(Com_MyRow("name"))
							strcpy(arena->munition[i].name, Com_MyRow("name"));
						if(Com_MyRow("abbrev"))
							strcpy(arena->munition[i].abbrev, Com_MyRow("abbrev"));
					}
					else
					{
						Com_Printf("WARNING: LoadAmmo(): Unexpected end of table 'munition'\n");
					}
				}
				
				maxmuntype = i; // external variable
				
				Com_Printf("Loading Munition Table. %d mun loaded\n", i);
			}
	
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;			
		}		
		else
		{
			Com_Printf("WARNING: CheckUserPasswd(): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}		
	}
	else
	{
		Com_Printf("WARNING: LoadAmmo(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		return;
	}
}

/*************
LoadDamageModel

Load planes/structs model from file
*************/

void LoadDamageModel(client_t *client)
{
	u_int8_t i, j;
	int16_t num_rows;
	int16_t num_fields;
	
	sprintf(my_query, "SELECT * FROM planes ORDER BY id");
	
	if(!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if((num_rows + 1) > MAX_PLANES)
					num_rows = MAX_PLANES;
					
				for(i = 0; i < num_rows; i++)
				{	
					if((my_row = mysql_fetch_row(my_result)))
					{
						strcpy(arena->planedamage[i + 1].name, Com_MyRow("name"));
						strcpy(arena->planedamage[i + 1].abbrev, Com_MyRow("abbrev"));
						arena->planedamage[i + 1].type = Com_Atoi(Com_MyRow("plane_type"));
					}
					else
					{
						Com_Printf("WARNING: LoadDamageModel(planes): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
					}
				}
				
				maxplanes = i + 1;
				
				Com_Printf("Loading Damage Model. %d planes loaded\n", i);
			}
			else
			{
				Com_Printf("WARNING: LoadDamageModel(planes): num_rows = %u\n", num_rows);
			}			
			
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: LoadDamageModel(planes): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}				
	}
	else
	{
		Com_Printf("WARNING: LoadDamageModel(planes): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
	
	sprintf(my_query, "SELECT * FROM dm_hitpoints ORDER BY plane");
	
	if(!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if((num_rows + 1) > maxplanes)
					num_rows = maxplanes;
					
				for(i = 0; i < num_rows; i++)
				{	
					if((my_row = mysql_fetch_row(my_result)))
					{
						num_fields = mysql_num_fields(my_result);
					
						if((num_fields - 1) > MAX_PLACE)
							num_fields = MAX_PLACE + 1;
							
						for(j = 1; j < num_fields; j++)
							arena->planedamage[i + 1].points[j-1] = Com_Atoi(my_row[j]);
						
						arena->planedamage[i + 1].positiveG = Com_Atof(my_row[j++]);
						arena->planedamage[i + 1].negativeG = Com_Atof(my_row[j]);
					}
					else
					{
						Com_Printf("WARNING: LoadDamageModel(hitpoints): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
					}
				}
			}
			else
			{
				Com_Printf("WARNING: LoadDamageModel(hitpoints): num_rows = %u\n", num_rows);
			}			
			
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: LoadDamageModel(hitpoints): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}				
	}
	else
	{
		if(mysql_errno(&my_sock))
			Com_Printf("WARNING: LoadDamageModel(hitpoints): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		else
			return;
	}
	
	sprintf(my_query, "SELECT * FROM dm_apstops ORDER BY plane");
	
	if(!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if((num_rows + 1) > maxplanes)
					num_rows = maxplanes;
					
				for(i = 0; i < num_rows; i++)
				{	
					if((my_row = mysql_fetch_row(my_result)))
					{
						num_fields = mysql_num_fields(my_result);
					
						if((num_fields - 1) > MAX_PLACE)
							num_fields = MAX_PLACE + 1;

						for(j = 1; j < num_fields; j++)
							arena->planedamage[i + 1].apstop[j-1] = Com_Atoi(my_row[j]);
					}
					else
					{
						Com_Printf("WARNING: LoadDamageModel(apstops): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
					}
				}
			}
			else
			{
				Com_Printf("WARNING: LoadDamageModel(apstops): num_rows = %u\n", num_rows);
			}			
			
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: LoadDamageModel(apstops): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}				
	}
	else
	{
		Com_Printf("WARNING: LoadDamageModel(apstops): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
	
	sprintf(my_query, "SELECT * FROM dm_imunities ORDER BY plane");
	
	if(!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if((num_rows + 1) > maxplanes)
					num_rows = maxplanes;
					
				for(i = 0; i < num_rows; i++)
				{	
					if((my_row = mysql_fetch_row(my_result)))
					{
						num_fields = mysql_num_fields(my_result);
					
						if((num_fields - 1) > MAX_PLACE)
							num_fields = MAX_PLACE + 1;
							
						for(j = 1; j < num_fields; j++)
							arena->planedamage[i + 1].imunity[j-1] = Com_Atoi(my_row[j]);
					}
					else
					{
						Com_Printf("WARNING: LoadDamageModel(imunities): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
					}
				}
			}
			else
			{
				Com_Printf("WARNING: LoadDamageModel(imunities): num_rows = %u\n", num_rows);
			}			
			
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: LoadDamageModel(imunities): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}				
	}
	else
	{
		Com_Printf("WARNING: LoadDamageModel(imunities): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}	

	sprintf(my_query, "SELECT * FROM dm_parents ORDER BY plane");
	
	if(!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if((num_rows + 1) > maxplanes)
					num_rows = maxplanes;
				
				for(i = 0; i < num_rows; i++)
				{	
					if((my_row = mysql_fetch_row(my_result)))
					{
						num_fields = mysql_num_fields(my_result);
					
						if((num_fields - 1) > MAX_PLACE)
							num_fields = MAX_PLACE + 1;
							
						for(j = 1; j < num_fields; j++)
							arena->planedamage[i + 1].parent[j-1] = Com_Atoi(my_row[j]);
					}
					else
					{
						Com_Printf("WARNING: LoadDamageModel(parents): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
					}
				}
			}
			else
			{
				Com_Printf("WARNING: LoadDamageModel(parents): num_rows = %u\n", num_rows);
			}			
			
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: LoadDamageModel(parents): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}				
	}
	else
	{
		Com_Printf("WARNING: LoadDamageModel(parents): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	sprintf(my_query, "SELECT * FROM grounddm ORDER BY type");
	
	if(!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if((num_rows = mysql_num_rows(my_result)) > 0)
			{
				if((my_row = mysql_fetch_row(my_result)))
				{
					num_fields = mysql_num_fields(my_result);
					
					if(num_fields > BUILD_MAX)
						num_fields = BUILD_MAX;

					for(i = 1; i < num_fields /*BUILD_MAX*/; i++)
					{
						arena->buildarmor[i].points = Com_Atoi(my_row[i]);
					}
					
					if((my_row = mysql_fetch_row(my_result)))
					{
						if(num_fields > BUILD_MAX)
							num_fields = BUILD_MAX;
						
						for(i = 1; i < num_fields /*BUILD_MAX*/; i++)
						{
							arena->buildarmor[i].apstop = Com_Atoi(my_row[i]);
						}

						if((my_row = mysql_fetch_row(my_result)))
						{
							if(num_fields > BUILD_MAX)
								num_fields = BUILD_MAX;
								
							for(i = 1; i < num_fields /*BUILD_MAX*/; i++)
							{
								arena->buildarmor[i].imunity = Com_Atoi(my_row[i]);
							}
						}
						else
						{
							Com_Printf("WARNING: LoadDamageModel(grounddm-im): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
						}
					}
					else
					{
						Com_Printf("WARNING: LoadDamageModel(grounddm-ap): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
					}
				}
				else
				{
					Com_Printf("WARNING: LoadDamageModel(grounddm-hp): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
				}
			}
			else
			{
				Com_Printf("WARNING: LoadDamageModel(grounddm): num_rows = %u\n", num_rows);
			}			
			
			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: LoadDamageModel(grounddm): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}				
	}
	else
	{
		Com_Printf("WARNING: LoadDamageModel(grounddm): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/*************
SaveDamageModel

Save planes/structs damage model from file
*************/

void SaveDamageModel(client_t *client, char *row)
{
	u_int8_t i, j;
	
	if(row)
	{
		i = Com_Atou(row);
	}
	else
		i = 0;

	if(i && i < maxplanes)
	{
		// hitpoints
		sprintf(my_query, "REPLACE dm_hitpoints VALUES(%d,", i);
		
		for(j = 0; j < 32; j++)
			sprintf(my_query, "%s%d,", my_query, arena->planedamage[i].points[j]);
		
		sprintf(my_query, "%s%.2f,%.2f)", my_query, arena->planedamage[i].positiveG, arena->planedamage[i].negativeG);
		
		if(Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
		{
			if(mysql_errno(&my_sock))
				Com_Printf("WARNING: SaveDamageModel(hitpoint): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			else
				return;
		}
		
		// apstop
		sprintf(my_query, "REPLACE dm_apstops VALUES(%d,", i);
		
		for(j = 0; j < 32; j++)
			sprintf(my_query, "%s%d,", my_query, arena->planedamage[i].apstop[j]);

		my_query[strlen(my_query) - 1] = ')';

		if(d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf("WARNING: SaveDamageModel(apstop): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
		
		// imunity
		sprintf(my_query, "REPLACE dm_imunities VALUES(%d,", i);
		
		for(j = 0; j < 32; j++)
			sprintf(my_query, "%s%d,", my_query, arena->planedamage[i].imunity[j]);

		my_query[strlen(my_query) - 1] = ')';

		if(d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf("WARNING: SaveDamageModel(imunity): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}	
		
		// parents
		sprintf(my_query, "REPLACE dm_parents VALUES(%d,", i);
		
		for(j = 0; j < 32; j++)
			sprintf(my_query, "%s%d,", my_query, arena->planedamage[i].parent[j]);

		my_query[strlen(my_query) - 1] = ')';

		if(d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf("WARNING: SaveDamageModel(parent): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}		
	}
	else
	{
		// hitpoints
		sprintf(my_query, "REPLACE grounddm VALUES(1,");
		
		for(i = 1; i < BUILD_MAX; i++)
			sprintf(my_query, "%s%d,", my_query, arena->buildarmor[i].points);
		
		my_query[strlen(my_query) - 1] = ')';
		
		if(Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
		{
			if(mysql_errno(&my_sock))
				Com_Printf("WARNING: SaveDamageModel(gdmpoints): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			else
				return;
		}
		
		// apstop
		sprintf(my_query, "REPLACE grounddm VALUES(2,");
		
		for(i = 1; i < BUILD_MAX; i++)
			sprintf(my_query, "%s%d,", my_query, arena->buildarmor[i].apstop);
		
		my_query[strlen(my_query) - 1] = ')';
		
		if(d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf("WARNING: SaveDamageModel(gdmapstop): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
		
		// imunity
		sprintf(my_query, "REPLACE grounddm VALUES(3,");
		
		for(i = 1; i < BUILD_MAX; i++)
			sprintf(my_query, "%s%d,", my_query, arena->buildarmor[i].imunity);
		
		my_query[strlen(my_query) - 1] = ')';
		
		if(d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf("WARNING: SaveDamageModel(gdmimunity): couldn't query REPLACE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}		
	}
}

/*************
CheckBoatDamage

Check if boat was damage, set new speed or capture it
*************/

void CheckBoatDamage(building_t *building, client_t *client)
{
	u_int8_t capture = 0;
	int16_t i;
	double speed, tspeed;
	char buffer[16];

	if(!building)
	{
		Com_Printf("WARNING: CheckBoatDamage(): no building\n");
		return;
	}

	if(arena->fields[building->field - 1].cv->speed == 0.01)
		return;

	speed = cvspeed->value;

	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(arena->fields[building->field - 1].buildings[i].field)
		{
			if(arena->fields[building->field - 1].buildings[i].type >= BUILD_CV &&
				arena->fields[building->field - 1].buildings[i].type <= BUILD_SUBMARINE &&
				!arena->fields[building->field - 1].buildings[i].status)
			{
				tspeed = (double)cvspeed->value * arena->fields[building->field - 1].buildings[i].armor / GetBuildingArmor(arena->fields[building->field - 1].buildings[i].type, client);

				if(speed > tspeed)
					speed = tspeed;
			}
		}
		else
			break;
	}

	arena->fields[building->field - 1].cv->speed = speed;

	sprintf(buffer, "f%d", building->field);

	if(building->type == BUILD_CV)
	{
		if(building->status)
		{
			if(client)
			{
				if(cvcapture->value)
				{
					BPrintf(RADIO_YELLOW, "%c CV (F%d) captured by %s", 0x95, building->field, client->longnick);
					capture = 1;
				}
				else
				{
					BPrintf(RADIO_YELLOW, "%c CV (F%d) destroyed by %s", 0x95, building->field, client->longnick);
				}
			}
			else
				BPrintf(RADIO_YELLOW, "%c CV (F%d) destroyed", 0x95, building->field);
			
			Cmd_Seta(buffer, 0, -1, 0); // remove all planes from a sunk convoy

			ResetCVPos(arena->fields[building->field - 1].cv);
			
			if(respawncvs->value)
				arena->fields[building->field - 1].cv->speed = cvspeed->value;
			else
				arena->fields[building->field - 1].cv->speed = 0.01;
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "CV Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
			SetCVSpeed(arena->fields[building->field - 1].cv);
		}
	}
	else if(building->type == BUILD_CARGO)
	{
		if(building->status)
		{
			for(i = 0; i < MAX_BUILDINGS; i++)
			{
				if(!arena->fields[building->field - 1].buildings[i].field)
				{
					break;
				}
				else if(arena->fields[building->field - 1].buildings[i].type == BUILD_CARGO && !arena->fields[building->field - 1].buildings[i].status) // some Cargo still up
				{
					i = -1;
					break;
				}
			}

			if(i >= 0)
			{
				if(client)
				{
					if(cvcapture->value)
					{
						BPrintf(RADIO_YELLOW, "%c Cargo Ships (F%d) captured by %s", 0x95, building->field, client->longnick);
						capture = 1;
					}
					else
					{
						BPrintf(RADIO_YELLOW, "%c Cargo Ships (F%d) destroyed by %s", 0x95, building->field, client->longnick);
					}
				}
				else
					BPrintf(RADIO_YELLOW, "%c Cargo Ships (F%d) destroyed", 0x95, building->field);
				
				Cmd_Seta(buffer, 0, -1, 0); // remove all planes from a sunk convoy

				ResetCVPos(arena->fields[building->field - 1].cv);
				
				if(respawncvs->value)
					arena->fields[building->field - 1].cv->speed = cvspeed->value;
				else
					arena->fields[building->field - 1].cv->speed = 0.01;
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Cargo Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
				SetCVSpeed(arena->fields[building->field - 1].cv);
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Cargo Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
			SetCVSpeed(arena->fields[building->field - 1].cv);
		}
	}
	else if(building->type == BUILD_DESTROYER)
	{
		if(building->fieldtype == FIELD_DD)
		{
			if(building->status)
			{

				for(i = 0; i < MAX_BUILDINGS; i++)
				{
					if(!arena->fields[building->field - 1].buildings[i].field)
					{
						break;
					}
					else if(arena->fields[building->field - 1].buildings[i].type == BUILD_DESTROYER && !arena->fields[building->field - 1].buildings[i].status) // some DD still up
					{
						i = -1;
						break;
					}
				}

				if(i >= 0)
				{
					if(client)
					{
						if(cvcapture->value)
						{
							BPrintf(RADIO_YELLOW, "%c Destroyers (F%d) captured by %s", 0x95, building->field, client->longnick);
							capture = 1;
						}
						else
						{
							BPrintf(RADIO_YELLOW, "%c Destroyers (F%d) destroyed by %s", 0x95, building->field, client->longnick);
						}
					}
					else
						BPrintf(RADIO_YELLOW, "%c Destroyers (F%d) destroyed", 0x95, building->field);
					
					Cmd_Seta(buffer, 0, -1, 0); // remove all planes from a sunk convoy

					ResetCVPos(arena->fields[building->field - 1].cv);
					
					if(respawncvs->value)
						arena->fields[building->field - 1].cv->speed = cvspeed->value;
					else
						arena->fields[building->field - 1].cv->speed = 0.01;
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "DD Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
					SetCVSpeed(arena->fields[building->field - 1].cv);
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "DD Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
				SetCVSpeed(arena->fields[building->field - 1].cv);
			}
		}
		else
		{
			if(building->fieldtype == FIELD_CARGO)
				PPrintf(client, RADIO_YELLOW, "Cargo Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
			else if(building->fieldtype == FIELD_CV)
				PPrintf(client, RADIO_YELLOW, "CV Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);

			SetCVSpeed(arena->fields[building->field - 1].cv);
		}
	}
	else if(building->type == BUILD_SUBMARINE)
	{
		if(building->fieldtype == FIELD_SUBMARINE)
		{
			if(building->status)
			{

				for(i = 0; i < MAX_BUILDINGS; i++)
				{
					if(!arena->fields[building->field - 1].buildings[i].field)
					{
						break;
					}
					else if(arena->fields[building->field - 1].buildings[i].type == BUILD_SUBMARINE && !arena->fields[building->field - 1].buildings[i].status) // some Sub still up
					{
						i = -1;
						break;
					}
				}

				if(i >= 0)
				{
					if(client)
					{
						if(cvcapture->value)
						{
							BPrintf(RADIO_YELLOW, "%c Submarine (F%d) captured by %s", 0x95, building->field, client->longnick);
							capture = 1;
						}
						else
						{
							BPrintf(RADIO_YELLOW, "%c Submarine (F%d) destroyed by %s", 0x95, building->field, client->longnick);
						}
					}
					else
						BPrintf(RADIO_YELLOW, "%c Submarine (F%d) destroyed", 0x95, building->field);
					
					Cmd_Seta(buffer, 0, -1, 0); // remove all planes from a sunk convoy

					ResetCVPos(arena->fields[building->field - 1].cv);
					
					if(respawncvs->value)
						arena->fields[building->field - 1].cv->speed = cvspeed->value;
					else
						arena->fields[building->field - 1].cv->speed = 0.01;
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "Submarine Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
					SetCVSpeed(arena->fields[building->field - 1].cv);
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Submarine Speed %.3f ft/s", arena->fields[building->field - 1].cv->speed);
				SetCVSpeed(arena->fields[building->field - 1].cv);
			}
		}
	}

	if(capture)//building->status && (building->type == BUILD_CV || building->type == BUILD_CARGO || building->type == BUILD_DESTROYER || building->type == BUILD_SUBMARINE))
	{
		if(client)
		{
			Com_LogEvent(EVENT_CAPT, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
			Com_LogDescription(EVENT_DESC_FIELD, building->field, NULL);			
			
			if(client->squadron)
				BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(client->squadron));
			Cmd_Capt(building->field - 1, client->country, NULL);
		}
	}
}

/*************
CaptureField

Captures the Field
*************/

void CaptureField(u_int8_t field, client_t *client)
{
	u_int32_t timer = MAX_UINT32; //ignore compiler warning
	int16_t i;

	BPrintf(RADIO_YELLOW, "%c Field %d captured by %s", 0x95, field, client->longnick);

	if(client->squadron)
		BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(client->squadron));
	
	Com_LogEvent(EVENT_CAPT, client->id, 0);
	Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
	Com_LogDescription(EVENT_DESC_FIELD, field, NULL);

	Cmd_Capt(field - 1, client->country, NULL);
	arena->fields[field - 1].paras = 0;

/*
	for(i = 0; i < MAX_CITYFIELD; i++)
	{
		if(arena->fields[field - 1].city[i] && arena->fields[field - 1].city[i]->needtoclose)
			break;
	}

	if(i == MAX_CITYFIELD)*/
		arena->fields[field - 1].abletocapture = 0;

	//score
	switch(arena->fields[field - 1].type)
	{
		case FIELD_LITTLE:
			client->score.captscore += SCORE_CAPTURE_FS;
			break;
		case FIELD_MEDIUM:
			client->score.captscore += SCORE_CAPTURE_FM;
			break;
		case FIELD_MAIN:
			client->score.captscore += SCORE_CAPTURE_FL;
			break;
		case FIELD_WB3POST:
			client->score.captscore += SCORE_CAPTURE_P;
			break;
		case FIELD_WB3VILLAGE:
			client->score.captscore += SCORE_CAPTURE_V;
			break;
		case FIELD_WB3TOWN:
			client->score.captscore += SCORE_CAPTURE_P;
			break;
		default:
			client->score.captscore += SCORE_CAPTURE;
	}
	
	if(IsBomber(client))
		sprintf(my_query, "UPDATE score_bomber SET fieldscapt = fieldscapt + '1' WHERE player_id = '%u'", client->id);
	else if(wb3->value && IsGround(client)) 
		sprintf(my_query, "UPDATE score_ground SET fieldscapt = fieldscapt + '1' WHERE player_id = '%u'", client->id);

	if(d_mysql_query(&my_sock, my_query))
	{
		PPrintf(client, RADIO_YELLOW, "CaptureField(): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
		Com_Printf("WARNING: CaptureField(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}	
	
	for(i = 0; i < MAX_BUILDINGS; i++) // Get minimum time
	{
		if(!arena->fields[field - 1].buildings[i].field)
		{
			break;
		}
		else
		{
			if(timer > arena->fields[field - 1].buildings[i].timer)
			{
				timer = arena->fields[field - 1].buildings[i].timer;
			}
		}
	}

	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(!arena->fields[field - 1].buildings[i].field)
		{
			break;
		}
		else
		{
			if(arena->fields[field - 1].buildings[i].type <= BUILD_88MMFLAK)
				arena->fields[field - 1].buildings[i].timer = timer;
		}
	}
	
	for(i = 0; i < maxplanes; i++)
	{
		if(arena->fields[field - 1].rps[i] > 0)
			arena->fields[field - 1].rps[i] = 1;
	}
}

/*************
TimetoNextArena

Returns time left in days to next arena
*************/

u_int16_t TimetoNextArena(void)
{
	int32_t difftime;
	time_t current, next;
	struct tm timestr;

	timestr.tm_sec = 0; // arena->frame % (int)(6000/timemult->value); /* seconds after the minute (0 to 61) */
	timestr.tm_min = 0; // arena->minute; /* minutes after the hour (0 to 59) */
	timestr.tm_hour = 0; // arena->hour; /* hours since midnight (0 to 23) */
	timestr.tm_mday = arena->day; /* day of the month (1 to 31) */
	timestr.tm_mon = arena->month - 1; /* months since January (0 to 11) */
	timestr.tm_year = arena->year - 1860; /* years since 1900 */
	timestr.tm_wday = 0; /* days since Sunday (0 to 6 Sunday=0) */
	timestr.tm_yday = 0; /* days since January 1 (0 to 365) */
	timestr.tm_isdst = 0; /* Daylight Savings Time */

	current = mktime(&timestr);

	timestr.tm_sec = 0; /* seconds after the minute (0 to 61) */
	timestr.tm_min = 0; /* minutes after the hour (0 to 59) */
	timestr.tm_hour = 0; /* hours since midnight (0 to 23) */
	timestr.tm_mday = (arena->mapcycle[arena->mapnum].date - ((arena->mapcycle[arena->mapnum].date / 100) * 100)); /* day of the month (1 to 31) */
	timestr.tm_mon = ((arena->mapcycle[arena->mapnum].date - ((arena->mapcycle[arena->mapnum].date / 10000) * 10000)) / 100) - 1; /* months since January (0 to 11) */
	timestr.tm_year = (arena->mapcycle[arena->mapnum].date / 10000) - 1860; /* years since 1900 */
	timestr.tm_wday = 0; /* days since Sunday (0 to 6 Sunday=0) */
	timestr.tm_yday = 0; /* days since January 1 (0 to 365) */
	timestr.tm_isdst = 0; /* Daylight Savings Time */

	next = mktime(&timestr);

	difftime = next - current;

	if(difftime < 0)
	{
		Com_Printf("WARNING: TimetoNextArena() returned invalid time calculation\n");
		return -1;
	}

	return (u_int16_t) FloorDiv(difftime, 86400);
}


/*************
InitArena

Initializes arena array
*************/

void InitArena(void)
{
	arena = (arena_t *) Z_Malloc(sizeof(arena_t));
//	arena->mapname = mapname->string;
//	arena->name = hostname->string;
//	arena->address = hostdomain->string;
	arena->frame = 0;

	arena->hour = 7 - ((int)dayhours->value%10)/2;
	arena->day = currday->value;
	arena->month = currmonth->value;
	arena->year = curryear->value;
	CalcTimemultBasedOnTime();
	arena->fields = (field_t *) Z_Malloc((sizeof(field_t) * fields->value) + 1);
	arena->cities = (city_t *) Z_Malloc((sizeof(city_t) * cities->value) + 1);

	sprintf(my_query, "TRUNCATE online_players");

	if(d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		Com_Printf("WARNING: InitArena(): couldn't query TRUNCATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	//LoadArenaStatus(mapname->string, NULL, 0);
	//LoadArenaStatus("arena", NULL, 0);

/*	if(rps->value)
	{
		Cmd_Seta("f-1", 0, -1, 0); // set 0 to all planes in all fields
		UpdateRPS();
	}*/

//	SetCVSpeed(NULL);
}

/*************
ChangeArena

Changes arena, duh :)
*************/

void ChangeArena(char *map, client_t *client)
{
	char file[256];
	int16_t i;
	u_int8_t j;
	FILE *fp;

	sprintf(file, "./arenas/%s/config.cfg", map);

	// check if .cfg exists
	if((fp = fopen(file, "r")))
	{
		fclose(fp);

		// kick all clients & drones
		for(i = 0; i < maxentities->value; i++)
		{
			if(clients[i].drone)
				RemoveDrone(&clients[i]);

			if(clients[i].socket)
			{
				if(clients[i].inuse && clients[i].infly)
				{
					PEndflightScores(1, 0, 0, &clients[i]); // DEBUG: force client to land or just simulate land scores
				}

				Com_Close(&(clients[i].socket));
			}
			memset(&clients[i], 0, sizeof(client_t));
		}
		UpdateIngameClients(0);
		arena->numplayers = 0;
		arena->numdrones = 0;


		// execute "map".cfg
		Var_Get("fields", fields->string, VAR_ARCHIVE | VAR_ADMIN);
		Var_Get("cities", cities->string, VAR_ARCHIVE | VAR_ADMIN);
		Var_Get("cvs", cvs->string, VAR_ARCHIVE | VAR_ADMIN);
		Var_Get("mapname", mapname->string, VAR_ARCHIVE | VAR_ADMIN);
		Var_Get("dirname", dirname->string, VAR_ARCHIVE | VAR_ADMIN);
		Var_Get("wb3", wb3->string, VAR_ARCHIVE | VAR_ADMIN);
		Var_Get("sqlserver", sqlserver->string, VAR_ADMIN);
		Var_Get("database", database->string, VAR_ADMIN); // VAR_ARCHIVE

		sprintf(file, "./arenas/%s/config", map);
		Cmd_LoadConfig(file, client);

		Var_Get("fields", fields->string, VAR_NOSET | VAR_ARCHIVE);
		Var_Get("cities", cities->string, VAR_NOSET | VAR_ARCHIVE);
		Var_Get("cvs", cvs->string, VAR_NOSET | VAR_ARCHIVE);
		Var_Get("mapname", mapname->string, VAR_NOSET | VAR_ARCHIVE);
		Var_Get("dirname", dirname->string, VAR_NOSET | VAR_ARCHIVE);
		Var_Get("wb3", wb3->string, VAR_ARCHIVE | VAR_NOSET);
		Var_Get("sqlserver", sqlserver->string, VAR_NOSET);
		Var_Get("database", database->string, VAR_NOSET); // VAR_ARCHIVE

		arena->day = currday->value;
		arena->month = currmonth->value;
		arena->year = curryear->value;
		CalcTimemultBasedOnTime();

		free(arena->fields);
		arena->fields = (field_t *) Z_Malloc((sizeof(field_t) * fields->value) + 1);
		free(arena->cities);
		arena->cities = (city_t *) Z_Malloc((sizeof(city_t) * cities->value) + 1);

		sprintf(file, "./arenas/%s/arena", map);
		LoadArenaStatus(file, NULL, 0);
		
		if(wb3->value)
			strcat(file, ".topo");
		else
			strcat(file, ".elv");

		LoadEarthMap(file);

		if(rps->value)
		{
			Cmd_Seta("f-1", 0, -1, 0); // set 0 to all planes in all fields
			sprintf(file, "./arenas/%s/planes", map);
			LoadRPS(file, NULL);
			UpdateRPS();
		}
		
		if(arena->frame > 100)
		{
			SaveArenaStatus("arena", NULL);
		}

		// Set original CV status
		for(i = (fields->value - 1), j = 0; i >= 0 && j < cvs->value; i--)
		{
			if((arena->fields[i].type == FIELD_CV) || (arena->fields[i].type == FIELD_CARGO) || (arena->fields[i].type == FIELD_DD) || (arena->fields[i].type == FIELD_SUBMARINE))
			{
				ReadCVWaypoints(j);

				arena->fields[i].cv = &(arena->cv[j]);
				
				arena->cv[j].id = j;
				arena->cv[j].field = i;
				ResetCVPos(&(arena->cv[j]));

//				SetCVRoute(&(arena->cv[j]));
				j++;
			}
		}
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: ChangeArena() Can't open file %s", file);
	}
}

/*************
CalcTimemultBasedOnTime

Calculate the timemult
*************/

void CalcTimemultBasedOnTime(void)
{
	if(((arena->hour - (7 - ((int)dayhours->value%10)/2)) >= (dayhours->value - 1)) || (arena->hour < ((7 - ((int)dayhours->value%10)/2) + 1))) // night
	{
		arena->multiplier = (u_int8_t)timemult->value * 2;
	}
	else // day
	{
		arena->multiplier = (u_int8_t)timemult->value;
	}

	Cmd_Time((arena->hour*100)+arena->minute, NULL, NULL);
}

/*************
NewWar

Reset/Backup Scores
Reset mapcycle
Clear all .score files
*************/

void NewWar(void)
{

	ResetScores();

	arena->year = inityear->value;
	arena->month = initmonth->value;
	arena->day = initday->value;

	arena->mapnum = 0;
}

/*************
NearestField

Returns nearest field/city array ID of != country
*************/

int32_t NearestField(int32_t posx, int32_t posy, u_int8_t country, u_int8_t city, u_int8_t cvs, u_int32_t *pdist)
{
	int16_t i = 0, j = 0, k = 0, fieldstotal = 0;
	int32_t distx, disty, fieldx, fieldy;
	u_int32_t dist, tdist;

	dist = MAX_UINT32; //ignore compiler warning
	tdist = 0;

	if(city)
		fieldstotal = fields->value + cities->value;
	else
		fieldstotal = fields->value;

	for(i = 0; i < fieldstotal; i++)
	{
		if(i < fields->value)
		{
			if(arena->fields[i].type >= FIELD_CV && arena->fields[i].type <= FIELD_SUBMARINE)
			{
				if(!cvs || (arena->fields[i].cv->speed == 0.01))
				{
					continue;
				}
			}

			fieldx = arena->fields[i].posxyz[0];
			fieldy = arena->fields[i].posxyz[1];
			k = arena->fields[i].country;
		}
		else
		{
			fieldx = arena->cities[i - (int16_t)fields->value].posxyz[0];
			fieldy = arena->cities[i - (int16_t)fields->value].posxyz[1];
			k = arena->cities[i - (int16_t)fields->value].country;
		}

		if(!country || k != country)
		{
			distx = posx - fieldx;
			disty = posy - fieldy;

			if(MODULUS(distx) < 46340 && MODULUS(disty) < 46340)
			{
				tdist = sqrt(Com_Pow(distx, 2) + Com_Pow(disty, 2));

				if(tdist < dist)
				{
					dist = tdist;
					j = i;
				}
			}
		}
	}

	if(tdist)
	{
		if(pdist)
			*pdist = dist;

		return j;
	}
	else
		return -1;
}

/*************
ReducePlanes

Reduce plane pool after hangar destruction
*************/

void ReducePlanes(u_int8_t field)
{
	u_int8_t i;

	for(i = 0; i < maxplanes; i++)
	{
		if(arena->fields[field - 1].rps[i] > 0)
			arena->fields[field - 1].rps[i] *= 0.8;
	}

}

/*************
IncreaseAcksReup

Increase Ack Reup time after warehouse destruction
*************/

void IncreaseAcksReup(u_int8_t field)
{
	u_int16_t i;

	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(arena->fields[field - 1].buildings[i].field)
		{
			if(arena->fields[field - 1].buildings[i].status)
			{
				if(((arena->fields[field - 1].buildings[i].type >= BUILD_50CALACK) && (arena->fields[field - 1].buildings[i].type >= BUILD_88MMFLAK)) ||
					(arena->fields[field - 1].buildings[i].type == BUILD_ARTILLERY))
				{
					arena->fields[field - 1].buildings[i].timer += 60000;

					if(arena->fields[field - 1].buildings[i].timer > (u_int32_t)(rebuildtime->value * 1200))
					{
						arena->fields[field - 1].buildings[i].timer = (rebuildtime->value * 1200);
					}
				}
			}
		}
		else
			break;
	}
}

/*************
IsVitalBuilding

Returns true if building is essential to close a field
*************/

u_int8_t IsVitalBuilding(building_t *building)
{
	if(arcade->value)
	{
		if((building->type == BUILD_50CALACK) ||
			(building->type == BUILD_20MMACK) ||
			(building->type == BUILD_40MMACK) ||
			(building->type == BUILD_88MMFLAK) ||
			(building->type == BUILD_HANGAR) ||
			(building->type == BUILD_FUEL) ||
			(building->type == BUILD_AMMO) ||
			(building->type == BUILD_RADAR) ||
			(building->type == BUILD_WARE) ||
			(building->type == BUILD_RADIOHUT) ||
			(building->type == BUILD_ANTENNA) ||
			(building->type == BUILD_CV) ||
			(building->type == BUILD_DESTROYER) ||
			(building->type == BUILD_CRUISER) ||
			(building->type == BUILD_CARGO) ||
			(building->type == BUILD_SUBMARINE) ||
			(building->type == BUILD_BRIDGE) ||
			(building->type == BUILD_SPECIALBUILD) ||
			(building->type == BUILD_FACTORY) ||
			(building->type == BUILD_BARRACKS) ||
			(building->type == BUILD_STATICS) ||
			(building->type == BUILD_REFINERY) ||
			(building->type == BUILD_PLANEFACTORY) ||
			(building->type == BUILD_BUILDING) ||
			(building->type == BUILD_CRANE) ||
			(building->type == BUILD_STRATEGIC) ||
			(building->type == BUILD_ARTILLERY) ||
			(building->type == BUILD_HUT) ||
			(building->type == BUILD_TRUCK))
			return TRUE;
		else
	/*
	BUILD_TOWER
	BUILD_TREE
	BUILD_SPAWNPOINT
	BUILD_HOUSE
	BUILD_ROCK
	BUILD_FENCE
	*/
			return FALSE;
	}
	else
	{
		if((building->type == BUILD_50CALACK) ||
			(building->type == BUILD_20MMACK) ||
			(building->type == BUILD_40MMACK) ||
			(building->type == BUILD_88MMFLAK) ||
			(building->type == BUILD_HANGAR) ||
			(building->type == BUILD_FUEL) ||
			(building->type == BUILD_AMMO) ||
			(building->type == BUILD_RADAR) ||
			(building->type == BUILD_WARE) ||
			(building->type == BUILD_RADIOHUT) ||
			(building->type == BUILD_ANTENNA) ||
			(building->type == BUILD_CV) ||
			(building->type == BUILD_DESTROYER) ||
			(building->type == BUILD_CRUISER) ||
			(building->type == BUILD_CARGO) ||
			(building->type == BUILD_SUBMARINE) ||
			(building->type == BUILD_BRIDGE) ||
			(building->type == BUILD_SPECIALBUILD) ||
			(building->type == BUILD_FACTORY) ||
			(building->type == BUILD_BARRACKS) ||
			(building->type == BUILD_STATICS) ||
			(building->type == BUILD_REFINERY) ||
			(building->type == BUILD_PLANEFACTORY) ||
			(building->type == BUILD_BUILDING) ||
			(building->type == BUILD_CRANE) ||
			(building->type == BUILD_STRATEGIC) ||
			(building->type == BUILD_ARTILLERY) ||
			(building->type == BUILD_HUT) ||
			(building->type == BUILD_TRUCK))
			return TRUE;
		else
	/*
	BUILD_TOWER
	BUILD_TREE
	BUILD_SPAWNPOINT
	BUILD_HOUSE
	BUILD_ROCK
	BUILD_FENCE
	*/
			return FALSE;
	}
}

/*********************************
These next functions were provided
to calculate arena Z from given
X and Y position
**********************************/

/* size of the map (number of dots) */
#define MWIDTH      512
#define MHEIGHT     512

/* distance between dots (in feets?) */
#define MHDOT       (wb3->value?2640:2181) // 2640 for WB3 (12.8 squares)
/* level of water */
#define MWATERMARK  209
/* center of map (num of dots * dist betw dots / 2) */
#define MXCENTER    558084 //558336?  // original == 558534
#define MYCENTER    556604 //558336?  // original == 556534
/* translating level number to feets */
#define LEVEL2HEIGHT(Level) (mapLevels[(Level)])
/* translating dot indexes to coords */
#define INDEX2X(Index) (wb3->value?(((int)(Index)) * MHDOT):(MXCENTER - ((int)(Index)) * MHDOT))
#define INDEX2Y(Index) (wb3->value?(((int)(Index)) * MHDOT):(MYCENTER - ((int)(Index)) * MHDOT))
//#define WB3INDEX2X(Index) (((int)(Index)) * WB3MHDOT)
//#define WB3INDEX2Y(Index) (((int)(Index)) * WB3MHDOT)
/* and back */
#define X2INDEX(xxxx) (wb3->value?((xxxx) / MHDOT):((MXCENTER - (xxxx)) / MHDOT))
#define Y2INDEX(yyyy) (wb3->value?((yyyy) / MHDOT):((MYCENTER - (yyyy)) / MHDOT))
//#define WB3X2INDEX(xxxx) ((xxxx) / WB3MHDOT) // square 0,0 = X 21120; Y 21120
//#define WB3Y2INDEX(yyyy) ((yyyy) / WB3MHDOT) // 

/* terrain levels */
u_int8_t earthMap[MHEIGHT * MWIDTH];
/* mapping levels to real height */
int32_t mapLevels[256];

/* get client position and map altitude */

u_int8_t Alt2Index(int32_t alt)
{
	int16_t x;
	const double A = 0.5333;
	const double B = 0.4677;
	const double C = -202;

	if(alt > 50000)
		return 254;

	x = ((B * -1) + sqrt(B * B - (4 * A * (C - alt))) / (2 * A) + 0.5);

	if (x < 0)
	{
		x = 0;
	}
	else if (x > 254)
	{
		x = 254;
	}

	return (u_int8_t) x;
}

void WB3Mapper(client_t *client)
{
	int32_t x, y, z;

	x = X2INDEX(client->posxy[0][0]);
	y = Y2INDEX(client->posxy[1][0]);
	z = Alt2Index(client->posalt[0]);

	if (x < 0 || x >= MWIDTH || y < 0 || y >= MHEIGHT)
	{
		Com_Printf("WARNING: WB3Mapper() %s out of scale X %d, Y %d\n", client->longnick, x, y);
		return;
	}

	if(!earthMap[(y * MWIDTH) + x]) // if altitude not defined (remember, vehicles cannot run in water (yet)
	{
		if(client->attr)
			PPrintf(client, RADIO_GREEN, "TOPO: X %d Y %d Z %d", x, y, z);
		earthMap[(y * MWIDTH) + x] = z;
	}
	else
	{
		if(earthMap[(y * MWIDTH) + x] != z)
		{
			earthMap[(y * MWIDTH) + x] = (earthMap[(y * MWIDTH) + x] + z) / 2;
		}
	}
}

/* calculating height at corner */
int32_t CalcHeight(int32_t x, int32_t y)
{
	if (x < 0 || x>= MWIDTH || y < 0 || y >= MHEIGHT)
		return LEVEL2HEIGHT(0);

	if ((x & 1) ^ (y & 1))
		return LEVEL2HEIGHT(earthMap[(y * MWIDTH) + x]);

	if (x & 1)
		return (CalcHeight(x - 1, y) + CalcHeight(x + 1, y)) / 2; // recursive call

	return (CalcHeight(x, y - 1) + CalcHeight(x, y + 1)) / 2; // recursive call
}

/* getting triangle under the dot [x,y] */
void GetTriangleUnder(int32_t x, int32_t y, int32_t *resx, int32_t *resy)
{
	int32_t i1, j1, x1, y1, i2, j2, i3, j3;

	i1 = X2INDEX(x);
	j1 = Y2INDEX(y);
	x1 = INDEX2X(i1);
	y1 = INDEX2Y(j1);

	if((i1 & 1) ^ (j1 & 1)) // if one is par and other impar, then
	{
		if(x1 - x >= y1 - y) // if diff between indexed x and real x >= y counterparts, then
		{
			i2 = i1 + 1;
			j2 = j1;
			i3 = i1 + 1;
			j3 = j1 + 1;
		}
		else
		{
			i2 = i1 + 1;
			j2 = j1 + 1;
			i3 = i1;
			j3 = j1 + 1;
		}
	}
	else
	{
		if(x1 - x + y1 - y <= MHDOT) // if diff between indexed x and real x plus y's <= dist between dots, then
		{
			i2 = i1 + 1;
			j2 = j1;
			i3 = i1;
			j3 = j1 + 1;
		}
		else
		{
			i1 = i1 + 1;
			j1 = j1;
			i2 = i1;
			j2 = j1 + 1;
			i3 = i1 - 1;
			j3 = j1 + 1;
		}
	}

	resx[0] = i1;
	resy[0] = j1;
	resx[1] = i2;
	resy[1] = j2;
	resx[2] = i3;
	resy[2] = j3;
}

/* calculating the z of earth in [x, y] */
u_int32_t GetHeightAt(int32_t x, int32_t y)
{
	int32_t tx[3], ty[3], tz[3];
	u_int32_t dist;
	double dx1, dy1, dz1, dx2, dy2, dz2;
	double a, b, c, z;

	GetTriangleUnder(x, y, tx, ty);

	tz[0] = CalcHeight(tx[0], ty[0]);
	tx[0] = INDEX2X(tx[0]);
	ty[0] = INDEX2Y(ty[0]);
	tz[1] = CalcHeight(tx[1], ty[1]);
	tx[1] = INDEX2X(tx[1]);
	ty[1] = INDEX2Y(ty[1]);
	tz[2] = CalcHeight(tx[2], ty[2]);
	tx[2] = INDEX2X(tx[2]);
	ty[2] = INDEX2Y(ty[2]);

	dx1 = tx[1] - tx[0];
	dy1 = ty[1] - ty[0];
	dz1 = tz[1] - tz[0];
	dx2 = tx[2] - tx[0];
	dy2 = ty[2] - ty[0];
	dz2 = tz[2] - tz[0];

	a = dy1*dz2 - dz1*dy2;
	b = dx1*dz2 - dz1*dx2;
	c = dx1*dy2 - dy1*dx2;

	if(c > -0.5 && c < 0.5)
	{
		dx1 = tx[0] - tx[1];
		dy1 = ty[0] - ty[1];
		dz1 = tz[0] - tz[1];
		dx2 = tx[2] - tx[1];
		dy2 = ty[2] - ty[1];
		dz2 = tz[2] - tz[1];

		a = dy1*dz2 - dz1*dy2;
		b = dx1*dz2 - dz1*dx2;
		c = dx1*dy2 - dy1*dx2;

		if(c == 0)
			z = tz[0];
		else
			z = tz[1] - (a * (x - tx[1]) - b * (y - ty[1])) / c;
	}
	else
	{
		z = tz[0] - (a * (x - tx[0]) - b * (y - ty[0])) / c;
	}

	if(z > 0)
	{
		return (u_int32_t)(z > 0 ? z : 0);
	}
	else
	{
		tx[0] = NearestField(x, y, 0, TRUE, TRUE, &dist);
		
		if(tx[0] < 0)
			z = 0;
		else
			z = arena->fields[tx[0]].posxyz[2];

		return z;
	}
}

/* loading map */
u_int8_t LoadEarthMap(char *FileName)
{
	int32_t i;
	FILE *fp = fopen(FileName, "rb");

	memset(earthMap, 0, sizeof(earthMap));

	if(fp == NULL)
	{
		Com_Printf("WARNING: LoadEarthMap() Cannot open file \"%s\"\n", FileName);
		return 0;
	}

	if(fread(earthMap, MWIDTH, MHEIGHT, fp) != MHEIGHT) // error on read some value from file
	{
		fclose(fp);
		Com_Printf("WARNING: LoadEarthMap() Error reading \"%s\"\n", FileName);
		return 0;
	}
	fclose(fp);

	Com_Printf("Loading elevations from \"%s\"\n", FileName);

	mapLevels[0] = -176 - 26; // ???

	for(i = 1; i < 256; i++)
		mapLevels[i] = mapLevels[i - 1] + i + (i - 1) / 15;

	return 1;
}

/* saving map */
u_int8_t SaveEarthMap(char *FileName)
{
	FILE *fp = fopen(FileName, "wb");

	if(fp == NULL)
	{
		Com_Printf("WARNING: SaveEarthMap() Cannot open file \"%s\"\n", FileName);
		return 0;
	}

	if(fwrite(earthMap, MWIDTH, MHEIGHT, fp) != MHEIGHT) // error on write some value from file
	{
		fclose(fp);
		Com_Printf("WARNING: SaveEarthMap() Error saving \"%s\"\n", FileName);
		return 0;
	}

	Com_Printf("Saving elevations into \"%s\"\n", FileName);

	fclose(fp);
	return 1;
}

/*************
IsVisible

is [x2,y2,z2] visible to [x1,y1,z1]? there no mountains between this dots?
*************/

int32_t IsVisible(int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	double dz = z2 - z1;
	int32_t direction;
	int32_t i[4], j[4];
	double h[4];
	double cx[4], cy[4];
	int32_t desti, destj;
	int32_t step = 0;
	int32_t curx = 0, cury = 0, curz = 0, curh = 0;

	if (dx >= 0 && dy >= 0)
	{
		if(dx > dy)
			direction = 1;
		else
			direction = 2;
	}
	else if(dx < 0 && dy >= 0)
	{
		if(dy > -dx)
			direction = 3;
		else
			direction = 4;
	}
	else if(dx < 0 && dy < 0)
	{
		if(dx < dy)
			direction = 5;
		else
			direction = 6;
	}
	else
	{
		if(dx < -dy)
			direction = 7;
		else
			direction = 8;
	}

	i[0] = X2INDEX(x1);
	j[0] = Y2INDEX(y1);
	h[0] = CalcHeight(i[0], j[0]);

	cx[0] = INDEX2X(i[0]);
	cy[0] = INDEX2Y(j[0]);

	i[1] = i[0] + 1;
	j[1] = j[0];
	h[1] = CalcHeight(i[1], j[1]);

	cx[1] = INDEX2X(i[1]);
	cy[1] = INDEX2Y(j[1]);

	i[2] = i[0] + 1;
	j[2] = j[0] + 1;
	h[2] = CalcHeight(i[2], j[2]);

	cx[2] = INDEX2X(i[2]);
	cy[2] = INDEX2Y(j[2]);

	i[3] = i[0];
	j[3] = j[0] + 1;
	h[3] = CalcHeight(i[3], j[3]);

	cx[3] = INDEX2X(i[3]);
	cy[3] = INDEX2Y(j[3]);

	desti = X2INDEX(x2);
	destj = Y2INDEX(y2);

	while(i[0] != desti && j[0] != destj)
	{
		switch(direction)
		{
			case 1:
				if(y1 + dy * (cx[0] - x1) / dx <= cy[0])
					step = 1;
				else
					step = 2;
				break;
			case 2:
				if(x1 + dx * (cy[0] - y1) / dy <= cx[0])
					step = 2;
				else
					step = 1;
				break;
			case 3:
				if(x1 + dx * (cy[1] - y1) / dy >= cx[1])
					step = 2;
				else
					step = 3;
				break;
			case 4:
				if(y1 + dy * (cx[1] - x1) / dx <= cy[1])
					step = 3;
				else
					step = 2;
				break;
			case 5:
				if (y1 + dy * (cx[2] - x1) / dx >= cy[2])
					step = 3;
				else
					step = 4;
				break;
			case 6:
				if(x1 + dx * (cy[2] - y1) / dy >= cx[2])
					step = 4;
				else
					step = 3;
				break;
			case 7:
				if(x1 + dx * (cy[3] - y1) / dy <= cx[3])
					step = 4;
				else
					step = 1;
				break;
			case 8:
				if(y1 + dy * (cx[3] - x1) / dx >= cy[3])
					step = 1;
				else
					step = 4;
				break;
		}
		switch(step)
		{
			case 1:
				cury = y1 + (int32_t)(dy * (cx[0] - x1) / dx);
				curz = z1 + (int32_t)(dz * (cx[0] - x1) / dx);
				curh = (int32_t)h[3] + (int32_t)(h[0] - h[3]) * (int32_t)(cury - cy[3]) / (int32_t)(cy[0] - cy[3]);
				break;
			case 2:
				curx = x1 + (int32_t)(dx * (cy[0] - y1) / dy);
				curz = z1 + (int32_t)(dz * (cy[0] - y1) / dy);
				curh = (int32_t)h[1] + (int32_t)(h[0] - h[1]) * (int32_t)(curx - cx[1]) / (int32_t)(cx[0] - cx[1]);
				break;
			case 3:
				cury = y1 + (int32_t)(dy * (cx[1] - x1) / dx);
				curz = z1 + (int32_t)(dz * (cx[1] - x1) / dx);
				curh = (int32_t)h[2] + (int32_t)(h[1] - h[2]) * (int32_t)(cury - cy[2]) / (int32_t)(cy[1] - cy[2]);
				break;
			case 4:
				curx = x1 + (int32_t)(dx * (cy[3] - y1) / dy);
				curz = z1 + (int32_t)(dz * (cy[3] - y1) / dy);
				curh = (int32_t)h[3] + (int32_t)(h[3] - h[2]) * (int32_t)(curx - cx[2]) / (int32_t)(cx[3] - cx[2]);
				break;
		}

		if(curz <= curh)
			return 0;

		switch(step)
		{
			case 1:
				i[1] = i[0];
				h[1] = h[0];
				cx[1] = cx[0];
				i[2] = i[3];
				h[2] = h[3];
				cx[2] = cx[3];
				i[0]--;
				h[0] = CalcHeight(i[0], j[0]);
				cx[0] = INDEX2X(i[0]);
				i[3]--;
				h[3] = CalcHeight(i[3], j[3]);
				cx[3] = INDEX2X(i[3]);

				if(i[0] < 0)
					return 1;
				break;
			case 2:
				j[3] = j[0];
				h[3] = h[0];
				cy[3] = cy[0];
				j[2] = j[1];
				h[2] = h[1];
				cy[2] = cy[1];
				j[0]--;
				h[0] = CalcHeight(i[0], j[0]);
				cy[0] = INDEX2Y(j[0]);
				j[1]--;
				h[1] = CalcHeight(i[1], j[1]);
				cy[1] = INDEX2Y(j[1]);

				if(j[0] < 0)
					return 1;
				break;
			case 3:
				i[0] = i[1];
				h[0] = h[1];
				cx[0] = cx[1];
				i[3] = i[2];
				h[3] = h[2];
				cx[3] = cx[2];
				i[1]++;
				h[1] = CalcHeight(i[1], j[1]);
				cx[1] = INDEX2X(i[1]);
				i[2]++;
				h[2] = CalcHeight(i[2], j[2]);
				cx[2] = INDEX2X(i[2]);

				if(i[2] >= MWIDTH)
					return 1;
				break;
			case 4:
				j[0] = j[3];
				h[0] = h[3];
				cy[0] = cy[3];
				j[1] = j[2];
				h[1] = h[2];
				cy[1] = cy[2];
				j[3]++;
				h[3] = CalcHeight(i[3], j[3]);
				cy[3] = INDEX2Y(j[3]);
				j[2]++;
				h[2] = CalcHeight(i[2], j[2]);
				cy[2] = INDEX2Y(j[2]);

				if(j[2] >= MHEIGHT)
					return 1;
				break;
		}
	}
	return 1;
}

/*************
NoopArenalist

Updates the list of arenas in main server
*************/
/*

Packet layout:

	l - packet size
	s40 - mapname
	b   - \0
	s40 - hostdomain
	b   - \0
	s40 - hostname
	b   - \0
	l - numplayers
	l - maxclients
	----
	l - shortnick
	b - country
	----
*/

void NoopArenalist(void)
{
	char buffer[MAX_UDPDATA];
	u_int16_t offset;
	u_int16_t numoffset, nplayers;
	u_int8_t i;
	struct sockaddr_in Target;
	struct hostent *he;
	int UdpSock;
	playerslist_t *Plist;

	UdpSock = socket(AF_INET, SOCK_DGRAM, 0);

	if(UdpSock < 0)
	{
		Com_Printf("WARNING: NoopArenalist(): Couldn't create UDP socket\n");
		return;
	}

	offset = 4;

	strcpy(buffer + offset, mapname->string);

	offset += strlen(mapname->string) + 1;

	strcpy(buffer + offset, hostdomain->string);

	offset += strlen(hostdomain->string) + 1;

	strcpy(buffer + offset, hostname->string);

	offset += strlen(hostname->string) + 1;
	
	numoffset = offset;

	offset += 4;

	*(long *)(buffer + offset) = htonl(maxclients->value);

	offset += 4;

	Plist = (playerslist_t *)(buffer+offset);

	for(i = 0, nplayers = 0; i < maxentities->value; i++)
	{
		if(clients[i].inuse && clients[i].ready && !clients[i].drone)
		{
			if(!(clients[i].attr == 1 && hideadmin->value))
			{
				Plist->nick = clients[i].shortnick;
				Plist->country = clients[i].country;

				nplayers++;
				offset += 5;

				Plist = (playerslist_t *) (buffer + offset);
			}
		}
	}
	
	*(long *)(buffer + numoffset) = htonl(nplayers);

	*(long *) buffer = htonl(offset);

	Target.sin_family = AF_INET;
	Target.sin_port = htons(19243);
	inet_aton("127.0.0.1", &Target.sin_addr); // Target.sin_addr.s_addr = inet_addr("127.0.0.1");
	sendto(UdpSock, buffer, offset, 0, (struct sockaddr *) &Target, sizeof(Target)); // send arena to itself

	if(arenalist->value)
	{
		inet_aton("89.108.72.190", &Target.sin_addr); // Target.sin_addr.s_addr = inet_addr("83.149.244.5");
		sendto(UdpSock, buffer, offset, 0, (struct sockaddr *) &Target, sizeof(Target)); // send to russian arena

		if(strlen(nooparena->string) > 4)
		{
			he = gethostbyname(nooparena->string);
			memcpy (&(Target.sin_addr.s_addr), he->h_addr, he->h_length);
			sendto(UdpSock, buffer, offset, 0, (struct sockaddr *) &Target, sizeof(Target)); // send arena to central arena
		}
	}

	Com_Close(&UdpSock);
}

/*************
SetBFieldType

Set buildings field type
*************/

void SetBFieldType(building_t *buildings, u_int16_t type)
{
	u_int16_t i;

	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(buildings[i].field)
		{
			buildings[i].fieldtype = type;
		}
		else
			break;
	}
}

/*************
CalcFactoryBuildings

Calculate "city" buildings number and buildings up for each country
*************/

void CalcFactoryBuildings(void)
{
	u_int8_t i;

	for(i = 0; i < 4; i++)
	{
		factorybuildings[i] = 0;
		factorybuildingsup[i] = 0;
	}

	for(i = 0; i < cities->value; i++)
	{
		CrediteFactoryBuildings(&arena->cities[i]);
	}
}

/*************
DebiteFactoryBuildings

Debite buildings from country
*************/

void DebiteFactoryBuildings(city_t *city)
{
	u_int16_t i;

	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(!city->buildings[i].field)
		{
			break;
		}
		else if(city->buildings[i].country)
		{
			factorybuildings[city->buildings[i].country - 1]--;

			if(!city->buildings[i].status)
			{
				factorybuildingsup[city->buildings[i].country - 1]--;
			}
		}
	}
}


/*************
CrediteFactoryBuildings

Credite buildings from country
*************/

void CrediteFactoryBuildings(city_t *city)
{
	u_int16_t i;

	for(i = 0; i < MAX_BUILDINGS; i++)
	{
		if(!city->buildings[i].field)
		{
			break;
		}
		else if(city->buildings[i].country)
		{
			factorybuildings[city->buildings[i].country - 1]++;

			if(!city->buildings[i].status)
			{
				factorybuildingsup[city->buildings[i].country - 1]++;
			}
		}
	}
}
