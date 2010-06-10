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

/**
 InitClients

 Create a zeroed clients array with maxentities->value size
 */

void InitClients(void)
{
	clients = (client_t *) Z_Malloc((sizeof(client_t) * maxentities->value) + 1);
}

/**
 AddClient

 Add a new client in clients array
 */

void AddClient(int socket, struct sockaddr_in *cli_addr)
{
	int i;

	for (i = 0; i < maxentities->value; i++)
	{
		if (!clients[i].inuse)
		{
			clients[i].inuse = 1;
			clients[i].drone = 0;
			clients[i].socket = socket;
			strncpy(clients[i].ip, (char *)inet_ntoa(cli_addr->sin_addr), 16);
			clients[i].ip[15] = '\0';
			clients[i].ctrid = GeoIP_country_id_by_addr(gi, clients[i].ip);
			clients[i].ready = 0;
			clients[i].key = (rand() ^ arena->time);
			clients[i].login = 5;

			clients[i].lives = -1;
			clients[i].attr = 0;
			if (rand()%2)
				clients[i].country = 1;
			else
				clients[i].country = 3;
			clients[i].field = 1;
			clients[i].plane = 1;
			clients[i].fuel = 50;
			clients[i].rain = 1;
			clients[i].ord = 0;
			clients[i].conv = 300;
			clients[i].squadron = 0;
			clients[i].squad_flag = 0;
			clients[i].rank = 1500;
			clients[i].ranking = 0;
			clients[i].killstod = 0;
			clients[i].structstod = 0;
			clients[i].lastscore = 0;
			//			clients[i].flighttime = 0;
			clients[i].streakscore = 0;

			clients[i].frame = arena->frame;
			clients[i].offset = 0;
			clients[i].timer = arena->time;
			break;
		}
	}

	if (debug->value)
	{
		Com_Printf(VERBOSE_DEBUG, "numplayers %d\n", arena->numplayers);
	}
	Com_Printf(VERBOSE_ALWAYS, "Incomming connection from %s - %s\n", clients[i].ip, GeoIP_country_name_by_addr(gi, clients[i].ip));
}

/**
 RemoveClient

 Reset client structure
 */

void RemoveClient(client_t *client)
{
	u_int16_t i, j;

	if (!client || !client->inuse)
		return;

	if (client->socket != 0)
		Com_Close(&(client->socket));
	else
		Com_Printf(VERBOSE_WARNING, "Com_Close tried to close socket ZERO\n");

	client->inuse = 0; // obsolete?

	if (!client->login)
	{
		if (arena->numplayers > 0)
		{
			if (!(client->attr == 1 && hideadmin->value))
				arena->numplayers--;
		}
		else
		{
			if (arena->numplayers == 0)
				Com_Printf(VERBOSE_DEBUG, "WARNING: RemoveClient() numplayers == 0\n");
			else
				Com_Printf(VERBOSE_DEBUG, "WARNING: RemoveClient() numplayers < 0\n");
		}

		for (i = 0; i < MAX_RELATED; i++)
		{
			if (client->related[i])
			{
				if (client->related[i]->drone)
				{
					RemoveDrone(client->related[i]);
				}
				else
				{
					for (j = 0; j < MAX_RELATED; j++)
					{
						if (client->related[i]->related[j] == client)
						{
							client->related[i]->related[j] = 0;
							break;
						}
					}
				}
			}
		}
	}

	for (i = 0; i < maxentities->value; i++) // remove invite pointer
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			if (clients[i].invite == client)
				clients[i].invite = NULL;
		}
	}

	for (i = 0; i < MAX_BOMBS; i++)
	{
		if (arena->bombs[i].from == client)
		{
			arena->bombs[i].from = NULL;
		}
	}

	if (strlen(client->longnick))
	{
		if (client->ready)
		{
			if (!(client->attr == 1 && hideadmin->value))
				BPrintf(RADIO_YELLOW, "%s became off-line", client->longnick);

			Com_LogEvent(EVENT_LOGOUT, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLIP, 0, client->ip);
			Com_LogDescription(EVENT_DESC_PLCTRID, client->ctrid, 0);

			sprintf(my_query, "DELETE FROM online_players WHERE player_id = '%u'", client->id);

			if (d_mysql_query(&my_sock, my_query)) // query succeeded
			{
				Com_Printf(VERBOSE_WARNING, "LOGOUT: couldn't query DELETE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}

		Com_Printf(VERBOSE_ALWAYS, "%s removed (%d players)\n", client->longnick, arena->numplayers);
	}
	else
	{
		Com_Printf(VERBOSE_ALWAYS, "%s removed (%d players)\n", client->ip, arena->numplayers);
	}

	//	slot = client->slot;

	memset(client, 0, sizeof(client_t));

	//	client->slot = slot;

	UpdateIngameClients(0);
}

/**
 DebugClient

 Save Debug information and kick player
 */

void DebugClient(char *file, u_int32_t line, u_int8_t kick, client_t *client)
{
	char filename[128];
	time_t ltime;
	u_int16_t len, i;
	FILE *fp;

	if(!client)
	{
		DebugArena(file, line);
		return;
	}

	Sys_PrintTrace();

	memset(filename, 0, sizeof(filename));

	time(&ltime);

	snprintf(filename, sizeof(filename), "./debug/%u%s.txt.LOCK", (u_int32_t)ltime, client->longnick);

	Sys_WaitForLock(filename);

	if (Sys_LockFile(filename) < 0)
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't open file \"%s\"\n", filename);
		return;
	}

	filename[strlen(filename) - 5] = '\0';

	if ((fp = fopen(filename, "wb")) == NULL)
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't open file \"%s\"\n", filename);
	}
	else
	{
		fprintf(fp, "%sZ - Debug file\n\n", asc2time(gmtime(&ltime)));

		fprintf(fp, "Error at %s, line %d\n\n", file, line);

		fprintf(fp, "Current map: %s\n\n", mapname->string);

		//		fprintf(fp, "Registers:\n");
		//		fprintf(fp, "ebp=%08lx ebx=%10d edi=%10d esi=%10d\n", debug_buffer[0], debug_buffer[1], debug_buffer[2], debug_buffer[3]);
		//		fprintf(fp, "esp=%10d ret=%10d exc=%10d con=%10d\n\n", debug_buffer[4], debug_buffer[5], debug_buffer[6], debug_buffer[7]);

		fprintf(fp, "client_s:\n");
		fprintf(fp, "inuse             = %10u  ready         = %10u  hdserial       = %10u\n", client->inuse, client->ready, client->hdserial);
		fprintf(fp, "socket            = %10d  frame         = %10u  attr           = %10u\n", client->socket, client->frame, client->attr);
		fprintf(fp, "drone             = %10u  threatened    = %10u  droneformation = %10u\n", client->drone, client->threatened, client->droneformation);
		fprintf(fp, "droneformchanged  = %10d  dronedistance = %10.2f  dronefield     = %10u\n", client->droneformchanged, client->dronedistance, client->dronefield);
		fprintf(fp, "aim[0]            = %10d  aim[1]        = %10d  aim[2]         = %10d\n", client->aim[0], client->aim[1], client->aim[2]);
		fprintf(fp, "dronetimer        = %10u  login         = %10u  loginkey       = %10d\n", client->dronetimer, client->login, client->loginkey);
		fprintf(fp, "key               = %10u  awaytimer     = %10u  timeout        = %10u\n", client->key, client->awaytimer, client->timeout);
		fprintf(fp, "shortnick         = %10u  longnick      = %10s  chute          = %10u\n", client->shortnick, client->longnick, client->chute);
		fprintf(fp, "arenabuildsok     = %10u  arenafieldsok = %10u  contrail       = %10u\n", client->arenabuildsok, client->arenafieldsok, client->contrail);
		fprintf(fp, "posxy[0]          = %10d  posxy[1]      = %10d  posalt         = %10d\n", client->posxy[0][0], client->posxy[1][0], client->posalt[0]);
		fprintf(fp, "status_damage           = %10u  status_status       = %10u  reldist        = %10d\n", client->status_damage, client->status_status, client->reldist);
		fprintf(fp, "speedxyz[0]       = %10d  speedxyz[1]   = %10d  speedxyz[2]    = %10d\n", client->speedxyz[0][0], client->speedxyz[1][0], client->speedxyz[2][0]);
		fprintf(fp, "accelxyz[0]       = %10d  accelxyz[1]   = %10d  accelxyz[2]    = %10d\n", client->accelxyz[0][0], client->accelxyz[1][0], client->accelxyz[2][0]);
		fprintf(fp, "angles[0]         = %10d  angles[1]     = %10d  angles[2]      = %10d\n", client->angles[0][0], client->angles[1][0], client->angles[2][0]);
		fprintf(fp, "aspeeds[0]        = %10d  aspeeds[1]    = %10d  aspeeds[2]     = %10d\n", client->aspeeds[0][0], client->aspeeds[1][0], client->aspeeds[2][0]);
		fprintf(fp, "field             = %10d  infly         = %10u  plane          = %10u\n", client->field, client->infly, client->plane);
		fprintf(fp, "fuel              = %10u  conv          = %10u  ord            = %10u\n", client->fuel, client->conv, client->ord);
		fprintf(fp, "country           = %10u  countrytime   = %10u  mapdots        = %10u\n", client->country, client->countrytime, client->mapdots);
		fprintf(fp, "easymode          = %10u  obradar       = %10d  mortars        = %10u\n", client->easymode, client->obradar, client->mortars);
		fprintf(fp, "squadron          = %10u  hls           = %10u  wings          = %10u\n", client->squadron, client->hls, client->wings);
		fprintf(fp, "flypenalty        = %10u\n", client->flypenalty);
		fprintf(fp, "flypenaltyfield   = %10u  hmackgear     = %10u  hmackpenalty   = %10u\n", client->flypenaltyfield, client->hmackgear, client->hmackpenalty);
		fprintf(fp, "hmackpenaltyfield = %10u  nummedals     = %10u  ranking        = %10u\n", client->hmackpenaltyfield, client->nummedals, client->ranking);
		fprintf(fp, "killssortie       = %10u  killstod      = %10u  structstod     = %10u\n", client->killssortie, client->killstod, client->structstod);
		fprintf(fp, "tklimit           = %10u  fileframe     = %10u  timer          = %10u\n", client->tklimit, client->fileframe, client->clienttimer);
		fprintf(fp, "fueltimer         = %10u  oiltimer      = %10u  oildamaged     = %10u\n", client->fueltimer, client->oiltimer, client->oildamaged);
		fprintf(fp, "radio[0]          = %10u  radio[1]      = %10u  radio[2]       = %10u\n", client->radio[0], client->radio[1], client->radio[2]);
		fprintf(fp, "radio[3]          = %10u  cancollide    = %10d\n\n", client->radio[3], client->cancollide);

		fprintf(fp, "IP = %s\n", client->ip);
		fprintf(fp, "file = %s\n\n", client->file);

		fprintf(fp, "Last Packet:\n\n");

		if (!client->login) // wbpacket
		{
			len = client->packetlen;
		}
		else
		{
			len = MAX_RECVDATA; // dump all packet
		}

		for (i = 0; i < len; i++)
		{
			fprintf(fp, "%02X ", mainbuffer[i]);
		}

		fprintf(fp, "\n");

		fclose(fp);
	}

	Sys_UnlockFile(strcat(filename, ".LOCK"));

	Com_Printf(VERBOSE_WARNING, "Bugged player (%s) - Error at %s, line %d\n", strlen(client->longnick) ? client->longnick : client->ip, file, line);

	if (kick)
		client->bugged = 1;
}

/**
 ProcessClient

 Check if client needs processing and do it
 */

int ProcessClient(client_t *client)
{
	u_int8_t i;
	client_t *nearplane;
	client_t *near_en;
	int32_t x, y, z;

	if (client->disconnect)
	{
		Com_Printf(VERBOSE_ALWAYS, "%s has requested to disconnect\n", client->longnick);
		return -1;
	}

	if (client->bugged)
	{
		BPrintf(RADIO_YELLOW, "%s bugged, and will be kicked", client->longnick);
		return -1;
	}

	client->timeout++;

	if (client->timeout >= MAX_TIMEOUT)
	{
		Com_Printf(VERBOSE_ALWAYS, "%s timed out\n", client->longnick);
		return -1;
	}

	if (client->login)
	{
		if (!setjmp(debug_buffer))
		{
			return ProcessLogin(client);
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, client);
			return -1;
		}
	}
	else
	{
			if(client->arenafieldsok && client->arenabuildsok < (fields->value + cities->value) && !((arena->frame - client->frame) % 100))
			{
				SendFieldStatus(client->arenabuildsok, client);

				if(!client->arenabuildsok)
				{
					if(client->countrytime <= time(NULL)) // if can chose side, send player to HQ
						Cmd_Move("hq", 0, client);
					else
						Cmd_Move(NULL, client->country, client);
					Cmd_Time((arena->hour*100)+arena->minute, NULL, client);

					if(batchfile->value)
					Cmd_LoadBatch(client);
				}
				client->arenabuildsok++;
			}

			if(client->ready && !client->arenafieldsok)
			{
				//	for(i = 0; i < fields->value; i++)
				//		Cmd_Capt(i, arena->fields[i].country, client);

				SendFieldsCountries(client);

/** WB2 CV
				for(i = 0; i < cvs->value; i++)
				{
					SendCVRoute(client, i);
					SendCVPos(client, i);
				}
*/

				/*if(client->hdserial || wb3->value)
				{ TODO New THL */
					// CheckBanned(client); TODO New THL
					ScoresCreate(client);
				//} TODO New THL
				client->arenafieldsok = 1;
			}

			if(client->ready)
			{
				//			if(client->countrytime)  // old countrytime system, can delete this lines
				//				client->countrytime--;

				if(client->flypenalty)
				client->flypenalty--;

				if(client->hmackpenalty)
				client->hmackpenalty--;

				if(client->msgtimer)
				client->msgtimer--;

				if(client->fueltimer)
				{
					client->fueltimer++;
				}

				if(client->oildamaged)
				{
					if(client->oiltimer)
					{
						if(!(client->oiltimer % 200))
						{
							Com_Printf(VERBOSE_ALWAYS, "%s %s is in fire at %s\n",
									client-> longnick,
									GetHitSite(client->oildamaged),
									Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
							PPrintf(client, RADIO_WHITE, "Your %s is in Fire! Jump out!", GetHitSite(client->oildamaged));
						}
						client->oiltimer--;
					}
					else
					{
						Com_Printf(VERBOSE_ALWAYS, "%s %s exploded at %s\n",
								client-> longnick,
								GetHitSite(client->oildamaged),
								Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

						PPrintf(client, RADIO_WHITE, "Your %s exploded!", GetHitSite(client->oildamaged - 4));
						{
							PPrintf(client, RADIO_GOLD, "DEBUG oildamaged %d", client->oildamaged - 4);
							AddPlaneDamage(client->oildamaged - 4, 0xffff, 0, NULL, NULL, client);
						}
						/*
						 switch(client->oilamaged)
						 {
						 case PLACE_CENTERFUEL:
						 AddPlaneDamage(PLACE_REARFUSE, 0xffffffff, client);
						 break;
						 case PLACE_LFUEL:
						 AddPlaneDamage(PLACE_LWING, 0xffffffff, client);
						 break;
						 case PLACE_RFUEL:
						 AddPlaneDamage(PLACE_RWING, 0xffffffff, client);
						 break;
						 }*/
						client->oildamaged = 0;
					}
				}

				// Forced config

				if(!((arena->frame - client->frame) % 30000))
				{
					if(!metar->value)
					{
						if((weather->value == 2) && !client->rain)
							WB3DotCommand(client, ".weather 0"); // cloudy
						else
							WB3DotCommand(client, ".weather %u", (u_int8_t)weather->value);
					}
					WB3DotCommand(client, ".clutterdistance 8500");
				}

				//				if(IsGround(client) && client->infly && !((arena->frame - client->frame) % 1000))
				//				{
				//					WB3Mapper(client);
				//				}

				if(!((arena->frame - client->frame) % 30000))
				{
					UpdateClientFile(client); // update client database every 5 minutes
				}

				if(client->cancollide && !((arena->frame - client->frame) % 6000)) // 60 seconds
				{
					PPrintf(client, RADIO_YELLOW, "Connection quality: %s", client->connection == 0?"Stable":client->connection == 1?"Fair":client->connection == 2?"Unstable":"Poor");
				}

				if(!((arena->frame - client->frame) % 180000)) // 30 minutes
				{
					if(client->tklimit)
					client->tklimit = 0;
				}

				// gunstat
				if(client->gunstat && client->infly && !((arena->frame - client->frame) % 300)) // 3 seconds
				{
					for(i = 0; i < 6; i++)
					{
						if(client->hitsstat[i])
						{
							PPrintf(client, RADIO_GREEN, "Hit %d bullets [%s]", client->hitsstat[i],
								i == 0?"7mm":
								i == 1?"13mm":
								i == 2?"20mm":
								i == 3?"30-37mm":
								i == 4?"40-57mm":"75-88mm");
							client->hitsstat[i] = 0;
						}
					}

					for(i = 0; i < 6; i++)
					{
						if(client->hitstakenstat[i])
						{
							PPrintf(client, RADIO_PURPLE, "Got hit %d bullets [%s]", client->hitstakenstat[i],
								i == 0?"7mm":
								i == 1?"13mm":
								i == 2?"20mm":
								i == 3?"30-37mm":
								i == 4?"40-57mm":"75-88mm");
							client->hitstakenstat[i] = 0;
						}
					}
				}

				if(predictpos->value && client->infly)
				{
					if((arena->time - client->postimer)> 600) // if client didnt send position packet in 500ms
					{
						Com_Printf(VERBOSE_DEBUG, "arena->time %u\n", arena->time);
						Com_Printf(VERBOSE_DEBUG, "arena - postimer %u\n", arena->time - client->postimer);
						BackupPosition(client, TRUE);
					}
				}

				if(!((arena->frame - client->frame) % 50)) // 500ms
				{
					SendPlayersNear(client);

					if(client->deck)
						SendDeckUpdates(client);
				}

				if(!((arena->frame - client->frame) % 10)) // 100ms
				{
					if((client->lograwdata || lograwposition->value) && !((arena->frame - client->frame) % 50) && client->infly)
					{
						LogRAWPosition(TRUE, client);
					}

					if(client->infly && !((arena->frame - client->frame) % 1000))
					{
						LogPosition(client);
					}

					if(client->cancollide > 0 && emulatecollision->value && !arcade->value)
					{
						nearplane = NearPlane(client, 99, 150);

						if(nearplane && (nearplane->related[0] != client) /*avoid collide with own drone*/)
						{
							x = client->posxy[0][0] - nearplane->posxy[0][0];
							y = client->posxy[1][0] - nearplane->posxy[1][0];
							z = sqrt(Com_Pow(x, 2) + Com_Pow(y, 2));
							x = client->posalt[0] - nearplane->posalt[0];

							if((y = sqrt(Com_Pow(x, 2) + Com_Pow(z, 2))) < 10) // 21

							{
								if(!nearplane->drone)
									PPrintf(nearplane, RADIO_DARKGREEN, "%s collided with you!!!", client->longnick);
								Com_Printf(VERBOSE_ALWAYS, "%s collided emulated with %s\n", client, nearplane->longnick);


								PPrintf(client, RADIO_DARKGREEN, "%s collided with you!!!", nearplane->longnick);
								Com_Printf(VERBOSE_ALWAYS, "%s collided emulated with %s\n", nearplane->longnick, client);

								client->damaged = 1;
								nearplane->damaged = 1;

								if(rand()%2)
								{
									SendForceStatus(STATUS_LWING, nearplane->status_status, nearplane); // lwing
									SendForceStatus(STATUS_RWING, client->status_status, client); // rwing
								}
								else if(rand()%2)
								{
									SendForceStatus(STATUS_RWING, nearplane->status_status, nearplane); // rwing
									SendForceStatus(STATUS_LWING, client->status_status, client); // lwing
								}
								else
								{
									SendForceStatus(STATUS_REARFUSE, nearplane->status_status, nearplane); // rfuse

									if(rand()%2)
									SendForceStatus(STATUS_LWING, client->status_status, client);
									else if(rand()%2)
									SendForceStatus(STATUS_RWING, client->status_status, client);
									else
									SendForceStatus(STATUS_REARFUSE, client->status_status, client);
								}

								near_en = NearPlane(client, client->country, 2000);

								if(near_en)
								{
									if(near_en->drone)
									{
										if(near_en->drone & (DRONE_WINGS1 | DRONE_WINGS2 | DRONE_FAU))
										{
											near_en = near_en->related[0];
										}
										else
										near_en = NULL;
									}
								}

								if(near_en)
								{
									client->hitby[0].dbid = near_en->id;
									client->hitby[0].damage = (double)MAX_UINT32; // TODO: Score: collision: change this

									if(nearplane == near_en)
									near_en = client;

									nearplane->hitby[0].dbid = near_en->id;
									nearplane->hitby[0].damage = (double)MAX_UINT32; // TODO: Score: collision: change this
								}

								client->cancollide = -1;
								nearplane->cancollide = -1;
							}
						}
					}
				}
			}

			if(!client->infly && !client->attr)
			{
				client->awaytimer++;

				if(timeout->value)
				{
					if(client->awaytimer> (u_int32_t)(timeout->value * 100))
					{
						BPrintf(RADIO_YELLOW, "%s is away and will be kicked", client->longnick);
						return -1;
					}
				}
			}

			if(!((arena->frame - client->frame) % 1000)) // 10 secs
			{
				client->warptimer = client->warp;
			}

			if(client->warptimer)
			{
				client->warptimer--;
				return 0;
			}

			if(!setjmp(debug_buffer))
			{
				//			while((x = GetPacket(client)) > 0); // line commented to prevent .masterview flood
				for(z = y = 0; y < 5; y++)
				{
					x = GetPacket(client);

					z += x;

					if(x <= 0)
					{
						if(x < 0)
						{
							if(client && strlen(client->longnick))
							{
								BPrintf(RADIO_YELLOW, "%s broke connection", client->longnick);
								Com_Printf(VERBOSE_ALWAYS, "%s broke connection\n", client->longnick);
							}
							z = -1;
						}

						break;
					}
				}

				return z;
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, client);
				return 0;
			}
		}
	}

/**
 BackupPosition

 Backup client position, leaving [0] free to predict or to store by PPlanePosition
 */

void BackupPosition(client_t *client, u_int8_t predict)
{
	u_int8_t i;
	int32_t temp = 0;

	if (predict)
	{
		temp = PredictorCorrector32(client->posxy[0], (u_int8_t)predictpos->value);
	}
	for (i = (MAX_PREDICT - 1); i > 0; i--)
	{
		client->posxy[0][i] = client->posxy[0][i - 1];
	}
	if (predict)
	{
		client->posxy[0][0] = temp;
	}

	if (predict)
		temp = PredictorCorrector32(client->posxy[1], (u_int8_t)predictpos->value);
	for (i = (MAX_PREDICT - 1); i > 0; i--)
	{
		client->posxy[1][i] = client->posxy[1][i - 1];
	}
	if (predict)
	{
		client->posxy[1][0] = temp;
	}

	if (predict)
		temp = PredictorCorrector32(client->posalt, (u_int8_t)predictpos->value);
	for (i = (MAX_PREDICT - 1); i > 0; i--)
	{
		client->posalt[i] = client->posalt[i - 1];
	}
	if (predict)
	{
		client->posalt[0] = temp;
	}
	/*
	 if(predict)
	 {
	 temp = PredictorCorrector16(client->angles[0], (u_int8_t)predictpos->value);
	 }
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->angles[0][i] = client->angles[0][i - 1];
	 }
	 if(predict)
	 {
	 client->angles[0][0] = (int16_t)temp;
	 }
	 if(predict)
	 temp = PredictorCorrector16(client->angles[1], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->angles[1][i] = client->angles[1][i - 1];
	 }
	 if(predict)
	 {
	 client->angles[1][0] = temp;
	 }
	 if(predict)
	 temp = PredictorCorrector16(client->angles[2], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->angles[2][i] = client->angles[2][i - 1];
	 }
	 if(predict)
	 {
	 client->angles[2][0] = temp;
	 }

	 if(predict)
	 temp = PredictorCorrector16(client->accelxyz[0], predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->accelxyz[0][i] = client->accelxyz[0][i - 1];
	 }
	 if(predict)
	 {
	 client->accelxyz[0][0] = temp;
	 }
	 if(predict)
	 temp = PredictorCorrector16(client->accelxyz[1], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->accelxyz[1][i] = client->accelxyz[1][i - 1];
	 }
	 if(predict)
	 {
	 client->accelxyz[1][0] = temp;
	 }
	 if(predict)
	 temp = PredictorCorrector16(client->accelxyz[2], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->accelxyz[2][i] = client->accelxyz[2][i - 1];
	 }
	 if(predict)
	 {
	 client->accelxyz[2][0] = temp;
	 }

	 if(predict)
	 temp = PredictorCorrector16(client->aspeeds[0], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->aspeeds[0][i] = client->aspeeds[0][i - 1];
	 }
	 if(predict)
	 {
	 client->aspeeds[0][0] = temp;
	 }
	 if(predict)
	 temp = PredictorCorrector16(client->aspeeds[1], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->aspeeds[1][i] = client->aspeeds[1][i - 1];
	 }
	 if(predict)
	 {
	 client->aspeeds[1][0] = temp;
	 }
	 if(predict)
	 temp = PredictorCorrector16(client->aspeeds[2], (u_int8_t)predictpos->value);
	 for(i = (MAX_PREDICT - 1); i > 0; i--)
	 {
	 client->aspeeds[2][i + 1] = client->aspeeds[2][i];
	 }
	 if(predict)
	 {
	 client->aspeeds[2][0] = temp;
	 }
	 */
	if (predict)
		temp = PredictorCorrector16(client->speedxyz[0], (u_int8_t)predictpos->value);
	for (i = (MAX_PREDICT - 1); i > 0; i--)
	{
		client->speedxyz[0][i] = client->speedxyz[0][i - 1];
	}
	if (predict)
	{
		client->speedxyz[0][0] = temp;
	}
	if (predict)
		temp = PredictorCorrector16(client->speedxyz[1], (u_int8_t)predictpos->value);
	for (i = (MAX_PREDICT - 1); i > 0; i--)
	{
		client->speedxyz[1][i] = client->speedxyz[1][i - 1];
	}
	if (predict)
	{
		client->speedxyz[1][0] = temp;
	}
	if (predict)
		temp = PredictorCorrector16(client->speedxyz[2], (u_int8_t)predictpos->value);
	for (i = (MAX_PREDICT - 1); i > 0; i--)
	{
		client->speedxyz[2][i] = client->speedxyz[2][i - 1];
	}
	if (predict)
	{
		client->speedxyz[2][0] = temp;
	}

	if (predict)
	{
		client->offset = client->postimer - arena->time;
		client->clienttimer -= client->offset;
		client->postimer = arena->time;
		client->predict++;
	}
}

/**
 ProcessLogin

 Run login sequence
 */
int32_t ProcessLogin(client_t *client)
{
	int32_t n;

	memset(mainbuffer, 0, sizeof(mainbuffer));

	switch (client->login)
	{
		case 5:
			n = LoginKey(client); // Sending crypt key
			if (n == 32)
			{
				client->login--;
			}
			else
			{
				return -1;
			}
			break;
		case 4:
			n = Com_Recv(client->socket, mainbuffer, 2); // Receive client acknowledgement
			if (n > 0)
			{
				if (mainbuffer[1] == 0x00 && n == 2)
				{
					if (mainbuffer[0] == 0x01)
						client->login--;
					else
					{
						if (client->key)
							Com_Printf(VERBOSE_ALWAYS, "%s Ping\n", client->ip);
						client->key = 0;
						client->login++;
					}
				}
				else
				{
					ProtocolError(client); // send protocol error message
					return -1;
				}
			}
			else if (n < 0)
			{
				return n;
			}
			break;
		case 3:
			n = Com_Recv(client->socket, mainbuffer, 96); // Receive client login/passwd
			if (n > 0)
			{
				if (n == 96)
				{
					if (CheckUserPasswd(client, mainbuffer)) // Check user/passwd OBS: include login answers in function
						return -1;
					else
						client->login--;
				}
				else
				{
					ProtocolError(client);
					return -1;
				}
			}
			else if (n < 0)
			{
				return n;
			}
			break;
		case 2:
			n = Com_Recv(client->socket, mainbuffer, 32);
			if (n > 0)
			{
				if (n == 32)
				{
					if ((n = LoginTypeRequest(mainbuffer, client)))
					{
						// Requesting Arenalist
						u_int8_t temp[2] =
						{ 0x00, 0x00 };
						n = Com_Send(client, temp, 2);

						if (n < 0 || !(SendArenaNames(client) > 0))
						{
							return -1;
						}
						else
						{
							if (strlen(client->longnick))
								Com_Printf(VERBOSE_ALWAYS, "%s connected\n", client->longnick);
							else
								Com_Printf(VERBOSE_ALWAYS, "%s connected\n", client->ip);
							client->login--;
						}
					}
					else
					{
						// Entering the game
						u_int8_t temp[2] =
						{ 0x00, 0x00 };
						n = Com_Send(client, temp, 2);

						if (n != 2)
						{
							return -1;
						}
						else
						{
							if (SendCopyright(client) < 0) // sends 0x0C00 packet
								return -1;
							UpdateIngameClients(0);
						}

						if (client->attr != 1)
						{
							arena->numplayers++;
						}
						else if (hideadmin->value != 1)
						{
							arena->numplayers++;
						}

						//if(!(client->attr == 1 && hideadmin->value)) // searching for bug...
						//	arena->numplayers++;

						if (strlen(client->longnick))
							Com_Printf(VERBOSE_ALWAYS, "%s entered the game (%d players)\n", client->longnick, arena->numplayers);
						else
							Com_Printf(VERBOSE_ALWAYS, "Creating a new nick for %s (%d players)\n", client->ip, arena->numplayers);

						client->login = 0;
					}
				}
				else
				{
					ProtocolError(client);
					return -1;
				}
			}
			else if (n < 0)
				return n;
			break;
		case 1:
			SendPlayersNames(client);
			return -1;
			break;
		default:
			Com_Printf(VERBOSE_WARNING, "login unk\n");
			return -1;
	}

	return 0;
}

/**
 CalcLoginKey

 Calculates login key from a 256 bits array
 */

int CalcLoginKey(u_int8_t *Data, int Len)
{
	int Key = 0;
	int i;

	for (i = 0; i < Len; i++)
	{
		unsigned char Val = Data[i];
		unsigned char Val1 = (Val >> 6) & 2;
		unsigned char Val2 = Val & 1;

		Val1 += Val2 + 1;
		Val = (Val >> Val1) & 1;
		Key |= ((unsigned int)(unsigned char)Val) << i;
	}
	return Key;
}

/**
 LoginKey

 Send 32 bytes key to client
 */

u_int8_t LoginKey(client_t *client)
{
	u_int8_t i;
	u_int8_t buffer[32];

	for (i = 0; i < 32; i++)
	{
		buffer[i] = rand()%0x100;
	}

	client->loginkey = CalcLoginKey(buffer, 32);

	return Com_Send(client, buffer, 32);
}

void DecryptOctet(u_int8_t Octet[8], unsigned long Key)
{
	unsigned long l1 = htonl(*(unsigned long *)(Octet));
	unsigned long l2 = htonl(*(unsigned long *)(Octet + 4));

	int i;

	for (i=0; i<32; i++)
		Key -= 0x61C88647;

	for (i=0; i<32; i++)
	{
		l2 -= (l1 + ((l1 >> 5) ^ (l1 << 4))) ^ Key;
		l1 -= (l2 + ((l2 >> 5) ^ (l2 << 4))) ^ Key;
		Key += 0x61C88647;
	}

	*(unsigned long *)(Octet) = htonl(l1);
	*(unsigned long *)(Octet + 4) = htonl(l2);
}

/**
 DecryptBlock

 Used to uncrypt login array
 */

void DecryptBlock(u_int8_t *Buf, int Len, long Key)
{
	int i;

	for (i = 0; i < Len; i += 8)
		DecryptOctet(Buf + i, Key);
}

/**
 CheckUserPasswd

 Check user and password and se player's nick
 */

int8_t CheckUserPasswd(client_t *client, u_int8_t *userpass) // userpass is supposed to have 96 bytes
{
	u_int8_t buffer[2] =
	{ 0x00, 0x00 };
	char *user;
	char *pwd;
//	u_int32_t cpwd;
	int8_t j = -1;

	DecryptBlock(userpass, 96, client->loginkey);

	userpass[31] = userpass[63] = '\0';

	user = (char*)userpass;
	pwd = (char*)userpass+32;

	if (!Com_CheckWBUsername(user))
	{
//		cpwd = crc32(pwd, strlen(pwd));

		if (debug->value)
			Com_Printf(VERBOSE_DEBUG, "User %s, Passwd %s\n", user, pwd);

		sprintf(my_query, "SELECT * FROM players LEFT JOIN score_common ON players.id = score_common.player_id WHERE players.loginuser = '%s' and players.password = MD5('%s') AND players.ipaddr = '%s' AND TIMESTAMPDIFF(SECOND, players.lastseen, UTC_TIMESTAMP()) < 1800", user, pwd, client->ip);

		if (!d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
			{
				if (mysql_num_rows(my_result) > 0)
				{
					if ((my_row = mysql_fetch_row(my_result)))
					{
						if (!Com_Atou(Com_MyRow("banned"))) // client banned
						{
							if (!whitelist->value || Com_Atou(Com_MyRow("white")))
							{
								strcpy(client->loginuser, user);
								GetClientInfo(client);

								j = 0;
							}
							else
							{
								buffer[0] = 0x05;
							}
						}
						else
						{
							buffer[0] = 0x05;
						}
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "CheckUserPasswd(): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						buffer[0] = 0xc0;
					}
				}
				else // password didnt match, or player doesnt exist
				{
//					sprintf(my_query, "INSERT INTO players (loginuser, password) VALUES ('%s', '%u')", user, cpwd);
//
//					if (!d_mysql_query(&my_sock, my_query))
//					{
//						strcpy(client->loginuser, user);
//
//						j = 0;
//					}
//					else // Could't insert because user already exist
//					{
//						if (mysql_errno(&my_sock) != 1062)
//						{
//							Com_Printf(VERBOSE_WARNING, "CheckUserPasswd(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
//							buffer[0] = 0x04;
//						}
//						else
//						{
							buffer[0] = 0xc0; // Access denied
//						}
//					}
				}

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "CheckUserPasswd(): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				buffer[0] = 0x04;
			}
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "CheckUserPasswd(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			buffer[0] = 0xc0;
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "CheckUserPasswd(): user contains non-valid chars (loginkey = %u)\n", client->loginkey);
		Com_Printfhex(userpass, 96);

		buffer[0] = 0x05;
	}

	if ((Com_Send(client, buffer, 2)) != 2)
	{
		return -1;
	}
	else
	{
		return j;
	}
}

/**
 LoginTypeRequest

 Checks if client are requesting login list or trying to login arena
 */

int8_t LoginTypeRequest(u_int8_t *buffer, client_t *client)
{
	// u_int8_t i;

	DecryptBlock(buffer, 32, client->loginkey);

	Com_Printf(VERBOSE_DEBUG, "arenalist: %s\n", buffer);

	if (!Com_Strcmp((char*)buffer, "arnalst2") || !Com_Strcmp((char*)buffer, "arnalst3") /*wb3*/) // retrieving arenalist
	{
		return 1;
	}
	else// if(!Com_Strcmp((char*)buffer, dirname->string) || !Com_Strcmp((char*)buffer, mapname->string))// login in arena
	{
		if(!Com_Strcmp((char*)buffer, "thchat"))
		{
			client->loginkey = 1;
		}
		else if(!Com_Strcmp((char*)buffer, "thclient"))
		{
			client->loginkey = 2;
		}
		else
		{
			client->loginkey = 3;
		}

		return 0;
	}

	//	else
	//		return -1;
}

/**
 FindDBClient

 Find a client in clients array using DB ID
 */

client_t *FindDBClient(u_int32_t dbid)
{
	u_int8_t i;

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse /*&& !clients[i].drone*/&& (clients[i].id == dbid))
		{
			return &clients[i];
		}
	}
	return NULL;
}

/**
 FindSClient

 Find a client in clients array using shortnick
 */

client_t *FindSClient(u_int32_t shortnick)
{
	u_int8_t i;

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse /*&& !clients[i].drone*/&&(clients[i].shortnick == shortnick))
		{
			return &clients[i];
		}
	}
	return NULL;
}

/**
 FindLClient

 Find a client in clients array using longnick
 */

client_t *FindLClient(char *longnick)
{
	u_int8_t i;

	if (!longnick)
		return NULL;

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse /*&& !clients[i].drone */&& !Com_Strcmp(clients[i].longnick, longnick))
		{
			return &clients[i];
		}
	}
	return NULL;
}

/**
 PPrintf

 Send a message to a player
 */

int PPrintf(client_t *client, u_int16_t radio, char *fmt, ...)
{
	va_list argptr;
	u_int8_t buffer[MAX_RADIOMSG]; // 75 is max size of a radio message
	char message[MAX_PRINTMSG];
	u_int8_t n=0;
	radiomessage_t *radiomessage;

	va_start(argptr, fmt);
	vsprintf(message, fmt, argptr);
	va_end(argptr);

	if (!client)
	{
		Com_Printf(VERBOSE_ALWAYS, "%s\n", message);
	}
	else if (client->ready)
	{
		if(client->drone)
		{
			if(!client->related[0])
				return -1;
			else
				client = client->related[0];
		}

		do
		{
			memset(buffer, 0, sizeof(buffer));
			radiomessage = (radiomessage_t *) (buffer);

			radiomessage->packetid = htons(Com_WBhton(0x1200));

			if(radio < 1000)
			{
				radiomessage->msgto = htonl(radio);
				radiomessage->msgfrom = htonl(ascii2wbnick("-HOST-", 1));
			}
			else
			{
				switch(radio)
				{
					case RADIO_WEATHER:
						radiomessage->msgto = htonl(RADIO_YELLOW);
						radiomessage->msgfrom = htonl(ascii2wbnick("-WTHR-", 1));
						break;
					default:
						radiomessage->msgto = htonl(radio);
						radiomessage->msgfrom = htonl(ascii2wbnick("-HOST-", 1));
						break;
				}
			}

			radiomessage->msgsize = strlen(message+(64*n)) > 64 ? 64 : strlen(message+(64*n));
			memcpy(&(radiomessage->message), message+(64*n), radiomessage->msgsize);

			SendPacket(buffer, radiomessage->msgsize + 11, client);

			if (strlen(message+(64*n)) > 64)
				n++;
			else
				n=0;

		} while (n && client->inuse);
	}
	else
	{
		return -1;
	}

	return 0;

}

/**
 CPrintf

 Send a country message
 */

void CPrintf(u_int8_t country, u_int16_t radio, char *fmt, ...)
{
	va_list argptr;
	char msg[MAX_PRINTMSG];
	u_int8_t i;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	memset(arena->thaisent, 0, sizeof(arena->thaisent));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone && clients[i].ready && clients[i].country == country)
		{
			if(clients[i].thai) // CPrintf
			{
				if(arena->thaisent[clients[i].thai].b)
					continue;
				else
					arena->thaisent[clients[i].thai].b = 1;
			}

			PPrintf(&clients[i], radio, msg);
		}
	}

	Com_Printf(VERBOSE_CHAT, "-HOST-:(%d)%s\n", radio, msg);
}

/**
 BPrintf

 Send a broadcast message
 */

void BPrintf(u_int16_t radio, char *fmt, ...)
{
	va_list argptr;
	char msg[MAX_PRINTMSG];
	u_int8_t i;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	memset(arena->thaisent, 0, sizeof(arena->thaisent));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone && clients[i].ready)
		{
			if(clients[i].thai) // BPrintf
			{
				if(arena->thaisent[clients[i].thai].b)
					continue;
				else
					arena->thaisent[clients[i].thai].b = 1;
			}

			PPrintf(&clients[i], radio, msg);
		}
	}

	Com_Printf(VERBOSE_CHAT, "-HOST-:(%d)%s\n", radio, msg);
}

/**
 CheckBanned

 Check if client has an hdserial banned. If does, ban client
 */

u_int8_t CheckBanned(client_t *client) // twin of CheckTK
{
	u_int8_t banned = 0;

	sprintf(my_query,
			"SELECT hdserials.hdserial FROM players, players_hdserials, hdserials WHERE players.id = '%u' AND players.id = players_hdserials.player_id AND hdserials.id = players_hdserials.hdserial_id AND hdserials.banned = '1'",
			client->id);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if (mysql_num_rows(my_result) > 0)
			{
				if ((my_row = mysql_fetch_row(my_result)))
				{
					banned = 1;

					sprintf(my_query, "UPDATE players SET banned = '1' WHERE id = '%u'", client->id);

					if (!d_mysql_query(&my_sock, my_query)) // query succeeded
					{
						PPrintf(client, RADIO_YELLOW, "You are banned");
						Com_Printf(VERBOSE_ATTENTION, "%s was banned for use banned hdserial\n", client->longnick);
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "CheckBanned(ban): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "CheckBanned(check): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "CheckBanned(check): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "CheckBanned(check): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return banned;
}

/**
 CheckTK

 Check if client was banned
 */

u_int8_t CheckTK(client_t *client) // twin of CheckBanned
{
	u_int8_t teamkiller = 0;

	if (wb3->value)
		sprintf(my_query,
				"SELECT ipaddress.ipaddr FROM players, players_ipaddress, ipaddress WHERE players.id = '%u' AND players.id = players_ipaddress.player_id AND ipaddress.id = players_ipaddress.ipaddress_id AND ipaddress.teamkiller = '1'",
				client->id);
	else
		sprintf(my_query,
				"SELECT hdserials.hdserial FROM players, players_hdserials, hdserials WHERE players.id = '%u' AND players.id = players_hdserials.player_id AND hdserials.id = players_hdserials.hdserial_id AND hdserials.teamkiller = '1'",
				client->id);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if (mysql_num_rows(my_result) > 0)
			{
				if ((my_row = mysql_fetch_row(my_result)))
				{
					teamkiller = 1;

					sprintf(my_query, "UPDATE players SET teamkiller = '1' WHERE id = '%u'", client->id);

					if (!d_mysql_query(&my_sock, my_query)) // query succeeded
					{
						PPrintf(client, RADIO_YELLOW, "You are disgraced as a team killer");
						Com_Printf(VERBOSE_ATTENTION, "%s was set TK for use TKed hdserial\n", client->longnick);
						client->tkstatus = 1;
						client->wings = 0;
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "CheckTK(flag): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "CheckTK(check): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "CheckTK(check): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "CheckTK(check): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return teamkiller;
}

/**
 GetClientInfo

 Get client info from file
 */

u_int8_t GetClientInfo(client_t *client)
{
	if (my_result && my_row)
	{
		client->id = Com_Atou(Com_MyRow("id"));
		client->tkstatus = Com_Atou(Com_MyRow("teamkiller"));
		client->lives = Com_Atoi(Com_MyRow("lives"));
		client->attr = Com_Atou(Com_MyRow("attr"));
		if (Com_MyRow("longnick")) // check if user has already a nick
		{
			strcpy(client->longnick, Com_MyRow("longnick"));
			client->shortnick = ascii2wbnick(client->longnick, client->attr);
		}
		client->country = Com_Atou(Com_MyRow("country"));
		client->countrytime = Com_Atou(Com_MyRow("countrytime"));
		client->plane = Com_Atou(Com_MyRow("plane_id"));
		client->fuel = Com_Atou(Com_MyRow("fuel"));
		client->ord = Com_Atou(Com_MyRow("ord"));
		client->conv = Com_Atou(Com_MyRow("conv"));
		client->easymode = Com_Atou(Com_MyRow("easymode"));
		client->radio[0] = Com_Atou(Com_MyRow("radio"));
		client->squadron = Com_Atou(Com_MyRow("squad_owner"));
		client->squad_flag = Com_Atou(Com_MyRow("squad_flag"));
		client->rank = Com_Atou(Com_MyRow("rank")); // Elo rating

		// from score_common
		client->ranking = Com_Atou(Com_MyRow("ranking"));
		client->killstod = Com_Atou(Com_MyRow("killstod"));
		client->structstod = Com_Atou(Com_MyRow("structstod"));
		client->lastscore = Com_Atof(Com_MyRow("lastscore"));
		//		client->flighttime = Com_Atou(Com_MyRow("flighttime"));
		client->streakscore = Com_Atof(Com_MyRow("streakscore"));
	}
	else
	{
		return 1;
	}

	return 0;
}

/**
 UpdateClientFile

 Update client file, creates one if doesn't exist and get user info from file if in login process
 */

void UpdateClientFile(client_t *client)
{
	if (!client)
		return;

	if (!strlen(client->loginuser))
	{
		Com_Printf(VERBOSE_WARNING, "UpdateClientFile() tried to print a (null) loginuser\n");
		return;
	}

	sprintf(my_query, "UPDATE players SET countrytime = '%u', country = '%d', plane_id = '%u', fuel = '%d', ord = '%d', conv = '%d', easymode = '%d', radio = '%d', rank = '%d' WHERE id = '%u' LIMIT 1",
			client->countrytime, client->country, client->plane, client->fuel, client->ord, client->conv, client->easymode, client->radio[0], client->rank, // Elo rating
			client->id);

	if (d_mysql_query(&my_sock, my_query))
	{
		Com_Printf(VERBOSE_WARNING, "UpdateClientFile(1): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	sprintf(my_query, "UPDATE players SET countrytime = '%u', country = '%d' WHERE parent = '%u'",
			client->countrytime, client->country, client->id);

	if (d_mysql_query(&my_sock, my_query))
	{
		Com_Printf(VERBOSE_WARNING, "UpdateClientFile(2): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/**
 AddKiller

 Add a new killer assist and return its location or just return its location if already exists (added clean disconnected players)
 */

int8_t AddKiller(client_t *victim, client_t *client)
{
	u_int8_t i;
	int8_t found, empty;

	found = empty = -1;

	if(client && victim)
	{
		for (i = 0; i < MAX_HITBY; i++)
		{
			if (victim->hitby[i].dbid)
			{
				if (victim->hitby[i].dbid == client->id)
				{
					found = i;
					break;
				}
			}
			else
			{
				empty = i;
			}
		}

		if (!(found < 0)) // if found, update killer plane
		{
			victim->hitby[found].plane = client->attached ? client->attached->plane : client->plane;
			victim->hitby[found].country = client->country;
			victim->hitby[found].squadron = client->squadron;
		}
		else if (!(empty < 0)) // if not found, add to array if slot available
		{
			victim->hitby[empty].dbid = client->id;
			strncpy(victim->hitby[empty].longnick, client->longnick, 6);
			victim->hitby[empty].plane = client->attached ? client->attached->plane : client->plane;
			victim->hitby[empty].country = client->country;
			victim->hitby[empty].squadron = client->squadron;
			victim->hitby[empty].damage = 0;
			found = empty;
		}
	}

	return found;
}

/**
 ClearKillers

 Clear killers list
 */

void ClearKillers(client_t *client)
{
	u_int8_t i;

	if (client)
	{
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "Remove all killers from list\n");

		for(i = 0; i < MAX_HITBY; i++)
		{
			if(client->hitby[i].dbid)
			{
				Com_Printf(VERBOSE_DEBUG, "DAMAGE: Removing %s\n", client->hitby[i].longnick);
			}
			client->hitby[i].country = 0;
			client->hitby[i].damage = 0.0;
			client->hitby[i].dbid = 0;
			client->hitby[i].longnick[0] = 0;
			client->hitby[i].plane = 0;
			client->hitby[i].squadron = 0;
		}
	}
}

/**
 ClearKillers

 Clear killers list
 */

void ClearBombers(u_int8_t field)
{
	u_int8_t i;

	if (field < fields->value)
	{
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "Remove all bombers from list\n");

		for(i = 0; i < MAX_HITBY; i++)
		{
			arena->fields[field].hitby[i].country = 0;
			arena->fields[field].hitby[i].damage = 0;
			arena->fields[field].hitby[i].dbid = 0;
			arena->fields[field].hitby[i].longnick[0] = 0;
			arena->fields[field].hitby[i].plane = 0;
			arena->fields[field].hitby[i].squadron = 0;
		}
	}
}

/**
 CalcEloRating

 Calc rank based in Elo Rating
 */

void CalcEloRating(client_t *winner, client_t *looser, u_int8_t flags)
{
	double Ea, Eb, K;

	if(!winner->rank)
		winner->rank = 1500;
	if(!looser->rank)
		looser->rank = 1500;

	if (!winner->drone && !looser->drone)
	{
		if ((flags & ELO_WINNER) && (winner != looser) /*ack dont have rank*/)
		{
			Ea = 1 / (1 + powf(10, (double)(looser->rank - winner->rank) / 400));

			if (winner->rank > 2400)
				K = 16;
			else if (winner->rank < 2100)
				K = 32;
			else
				K = 24;

			winner->rank += (int16_t)floorf(K * (1 - Ea) + 0.5);
		}

		if ((flags & ELO_LOOSER) && (winner != looser) /*ack dont have rank*/)
		{
			Eb = 1 / (1 + powf(10, (double)(winner->rank - looser->rank) / 400));

			if (looser->rank > 2400)
				K = 16;
			else if (looser->rank < 2100)
				K = 32;
			else
				K = 24;

			looser->rank += (int16_t)floorf(K * (0 - Eb) + 0.5);
		}
	}
}

/**
 NearPlane

 Returns nearest != country plane in visible list
 */

client_t *NearPlane(client_t *client, u_int8_t country, int32_t limit)
{
	u_int8_t i, j;
	u_int32_t dist;

	dist = MAX_UINT32; // ignore compiler warning

	for (i = 0, j = MAX_SCREEN; i < MAX_SCREEN; i++)
	{
		if (client->visible[i].client && !client->visible[i].client->drone && (client->visible[i].client->country != country))
		{
			client->visible[i].client->reldist = DistBetween(client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->visible[i].client->posxy[0][0],
					client->visible[i].client->posxy[1][0], client->visible[i].client->posalt[0], limit);

			if (client->drone && client->visible[i].client->reldist >= 0)
				return client->visible[i].client;

			if ((u_int32_t)client->visible[i].client->reldist < dist)
			{
				dist = client->visible[i].client->reldist;
				j = i;
			}
		}
	}

	if (j < MAX_SCREEN)
		return client->visible[j].client;
	else
		return NULL;
}

/**
 ForceEndFlight

 Force client to return to tower
 DEBUG: check this in WB3
 */

void ForceEndFlight(u_int8_t remdron, client_t *client)
{
	u_int8_t buffer[10];
	u_int8_t i;
	char field[8];

	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0x08;
	buffer[1] = 0x0A;
	buffer[3] = ENDFLIGHT_LANDED;
	buffer[5] = client->field;

	snprintf(field, sizeof(field), "f%d", client->field);

	SendPacket(buffer, sizeof(buffer), client);
	//Cmd_Move(field, client->country, client);

	if (client->attached)
	{
		if (client->attached->view == client)
		{
			client->attached->view = NULL;
		}
	}

	if (client->view) // ends view flight
	{
		if (client->view->attached == client)
			ForceEndFlight(TRUE, client->view);
	}

	if (remdron)
	{
		for (i = 0; i < MAX_RELATED; i++)
		{
			if (client->related[i] && client->related[i]->drone)
			{
				//				if(client->related[i]->drone & (DRONE_HMACK | DRONE_HTANK))
				RemoveDrone(client->related[i]);
			}
		}
	}

	client->status_damage = client->status_status = client->infly = client->chute = client->obradar = client->mortars = client->cancollide = client->fueltimer = 0;

	ClearKillers(client);

	client->speedxyz[0][0] = client->speedxyz[1][0] = client->speedxyz[2][0] = 0;

	client->view = client->shanghai = client->attached = NULL;
}

/**
 ReloadWeapon

 Reload client weapons
 */

void ReloadWeapon(u_int16_t weapon, u_int16_t value, client_t *client)
{
	u_int8_t buffer[8];
	reloadweapon_t *reload;

	memset(buffer, 0, sizeof(buffer));

	reload = (reloadweapon_t *)buffer;

	reload->packetid = htons(Com_WBhton(0x1D09));
	reload->unknown1 = htons(weapon);
	reload->amount = htons(value);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 GetShipByNum

 Return Ship client_t from cvnum
 */

ship_t *GetShipByNum(u_int8_t cvnum)
{
	u_int8_t i, j;
	ship_t *ship;

	for(i = 0, j = 0; i < cvs->value; i++)
	{
		for(ship = arena->cvs[i].ships; ship; ship = ship->next)
		{
			if(j == cvnum)
				return ship;
			j++;
		}
	}

	return NULL;
}

/**
 WB3AiMount

 Request to mount CV Deck
 */

void WB3AiMount(u_int8_t *buffer, client_t *client)
{
	wb3aimount_t *aimount;
	ship_t *ship;

	aimount = (wb3aimount_t *) buffer;

	PPrintf(client, RADIO_YELLOW, "unk1: %d cvnum: %d inout: %d", ntohs(aimount->unk1)/*cvdot->unk2*/, aimount->cvnum, aimount->inout);

	ship = GetShipByNum(aimount->cvnum);

	if(!ship || !ship->drone)
		return;

	if(aimount->inout == 1)
	{
		client->field = arena->cvs[ship->group].field+1;
		UpdateIngameClients(0);
		SendArenaRules(client);
		WB3SendGruntConfig(client);
		WB3ArenaConfig2(client);
		WB3SendAcks(client);
		SendRPS(client);
		//client->field = 1;
		AddRemoveCVScreen(ship, client, FALSE, ntohs(aimount->unk1), aimount->cvnum);
	}
	else if(aimount->inout == 2)
	{
		AddRemoveCVScreen(ship, client, TRUE, ntohs(aimount->unk1), aimount->cvnum);
	}
}

/**
 WB3ClientSkin

 Set client skin
 */

void WB3ClientSkin(u_int8_t *buffer, client_t *client)
{
	u_int8_t i, j;
	wb3planeskin_t *clientskin;
	char *ps;

	if(!skins->value)
	{
		return;
	}

	clientskin = (wb3planeskin_t *) buffer;

	if(clientskin->msgsize < 64)
	{
		ps = &(clientskin->msg);

		ps[clientskin->msgsize] = '\0';

		if (strlen(thskins->string) > 2)
		{
			i = 0;

			if(!strstr(ps, "sqd"))
			{
				i = 1;

				if(strstr(ps, "2ppv"))
				{
					i = 2;
				}
				else if(strstr(ps, "3ppv"))
				{
					i = 3;
				}
				else if(strstr(ps, "4ppv"))
				{
					i = 4;
				}
			}

			if(i)
			{
				ps = CreateSkin(client, i);
			}
		}

		strcpy(client->skin, ps);

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				for (j = 0; j < MAX_SCREEN; j++)
				{
					if (!clients[i].visible[j].client)
						continue;

					if (client == clients[i].visible[j].client)
					{
						Com_Printf(VERBOSE_DEBUG, "WB3OverrideSkin() at WB3ClientSkin()\n");
						WB3OverrideSkin(j, &clients[i]);
						break;
					}
				}
			}
		}
	}
	else
	{
		memset(client->skin, 0, sizeof(client->skin));
	}

	Com_Printf(VERBOSE_DEBUG, "%s skin set to \"%s\"\n", client->longnick, client->skin);
}

/**
 CreateSkin

 Generate a padronized skin
 */

char *CreateSkin(client_t *client, u_int8_t number)
{
	static char buffer[64];

	switch (client->country)
	{
		case COUNTRY_RED:
			snprintf(buffer, sizeof(buffer), "ppv\\%s\\%sr%dppv.vfc@%sr%d.ppv", GetPlaneDir(client->plane), thskins->string, number, thskins->string, number);
			break;
		case COUNTRY_GOLD:
			snprintf(buffer, sizeof(buffer), "ppv\\%s\\%sg%dppv.vfc@%sg%d.ppv", GetPlaneDir(client->plane), thskins->string, number, thskins->string, number);
			break;
		default:
			buffer[0] = '\0';
	}

	return buffer;
}

/**
 WB3OverrideSkin

 Send skin to client
 */

void WB3OverrideSkin(u_int8_t slot, client_t *client)
{
	wb3overrideskin_t *overrideskin;
	u_int8_t size;
	u_int8_t buffer[256];

	memset(buffer, 0, sizeof(buffer));

	overrideskin = (wb3overrideskin_t *) buffer;

	if(client->visible[slot].client)
	{
		size = strlen(client->visible[slot].client->skin);

		if(size && size < 64)
		{
			overrideskin->packetid = htons(Com_WBhton(0x002D));
			overrideskin->slot = slot;
			overrideskin->msgsize = size;
			memcpy(&(overrideskin->msg), client->visible[slot].client->skin, size);

			Com_Printf(VERBOSE_DEBUG, "%s[%d] sent skin to %s \"%s\"\n", client->visible[slot].client->longnick, slot, client->longnick, client->visible[slot].client->skin);

			SendPacket(buffer, size + 4, client);
		}
	}
}

/**
 ClientHDSerial

 Get player's hdserial, add it to hdserials table and players_hdserials table
 */

void ClientHDSerial(u_int8_t *buffer, client_t *client)
{
	hdserial_t *hdserial;
	u_int32_t hd_id;

	hdserial = (hdserial_t *) buffer;

	client->hdserial = (u_int32_t)ntohl(hdserial->serial);

	if (!client->hdserial)
		return;

	sprintf(my_query, "INSERT INTO hdserials (hdserial) VALUES ('%u')", client->hdserial);

	if (d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf(VERBOSE_WARNING, "ClientHDSerial(id): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			return;
		}
	}

	hd_id = (u_int32_t)mysql_insert_id(&my_sock);

	//	sprintf(my_query, "SELECT id FROM hdserials WHERE hdserial = '%u'", client->hdserial);

	sprintf(my_query, "INSERT INTO players_hdserials (player_id, hdserial_id) VALUES ('%u', '%u')", client->id, hd_id);

	if (d_mysql_query(&my_sock, my_query))
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf(VERBOSE_WARNING, "ClientHDSerial(insert): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
}

/**
 ClientIpaddr

 Get player's ipaddr, add it to ipaddr table and players_ipaddress table
 */

void ClientIpaddr(client_t *client)
{
	u_int32_t ipaddr_id;

	Com_Printf(VERBOSE_DEBUG, "Registering IP in DB\n");

	if (!client || !strlen(client->ip))
		return;

	sprintf(my_query, "INSERT INTO ipaddress (ipaddr) VALUES ('%s')", client->ip);

	if (d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf(VERBOSE_WARNING, "ClientIpaddr(id): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			return;
		}
	}

	Com_Printf(VERBOSE_DEBUG, "Inserted in ip table (%s)\n", client->ip);

	ipaddr_id = (u_int32_t)mysql_insert_id(&my_sock);

	if (!ipaddr_id)
		Com_Printf(VERBOSE_DEBUG, "ClientIpaddr(ipaddr_id) = 0, IP %s\n", client->ip);

	//	sprintf(my_query, "SELECT id FROM ipaddress WHERE ipaddr = '%s'", client->ip);

	sprintf(my_query, "INSERT INTO players_ipaddress (player_id, ipaddress_id) VALUES ('%u', '%u')", client->id, ipaddr_id);

	if (d_mysql_query(&my_sock, my_query))
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf(VERBOSE_WARNING, "ClientIpaddr(insert): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}

	Com_Printf(VERBOSE_DEBUG, "Inserted in ip-client table (%s - %s) (%d - %d)\n", client->ip, client->longnick, client->id, ipaddr_id);
}

/**
 LogRAWPosition

 Log client RAW position to later analysis
 */

void LogRAWPosition(u_int8_t server, client_t *client)
{
	char file[128];
	FILE *fp;

	if (server)
		snprintf(file, sizeof(file), "./logs/players/%s.srv", client->longnick);
	else
		snprintf(file, sizeof(file), "./logs/players/%s.cli", client->longnick);

	if (!(fp = fopen(file, "a")))
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't append file \"%s\"\n", file);
	}
	else
	{
		fprintf(fp, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%u;%d;%u;%u\n", client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->angles[0][0], client->angles[1][0],
				client->angles[2][0], client->speedxyz[0][0], client->speedxyz[1][0], client->speedxyz[2][0], client->accelxyz[0][0], client->accelxyz[1][0], client->accelxyz[2][0],
				client->aspeeds[0][0], client->aspeeds[1][0], client->aspeeds[2][0], client->clienttimer, client->offset, arena->time, Sys_Milliseconds());
		fclose(fp);
	}
}

/**
 LogPosition

 Log client position to later analysis
 */

void LogPosition(client_t *client)
{
	FILE *fp;
	char filename[128];

	snprintf(filename, sizeof(filename), "./logs/players/%s.pos", client->logfile);

	if (!(fp = fopen(filename, "a")))
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't append file \"%s\"\n", filename);
	}
	else
	{
		fprintf(fp, "%d;%d;%d;%.0f;%.0f;%.0f;%u\n", client->posxy[0][0], client->posxy[1][0], client->posalt[0], FloorDiv(client->angles[0][0], 10), FloorDiv(client->angles[1][0], 10),
				WBtoHdg(client->angles[2][0]), (u_int32_t)time(NULL));
		fclose(fp);
	}
}

/**
 HardHit
 
 Make a hardhit query (add hits in DB)
 */

void HardHit(u_int8_t munition, u_int8_t penalty, client_t *client)
{
	if (munition >= maxmuntype)
	{
		Com_Printf(VERBOSE_WARNING, "HardHit(): Munition ID overflow %d. maxmuntype=%d\n", munition, maxmuntype);
		return;
	}

	ScoresEvent(SCORE_HARDHIT, client, munition);
}
