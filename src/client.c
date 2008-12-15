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
 InitClients

 Create a zeroed clients array with maxentities->value size
 *************/

void InitClients(void)
{
	clients = (client_t *) Z_Malloc((sizeof(client_t) * maxentities->value) + 1);
}

/*************
 AddClient

 Add a new client in clients array
 *************/

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
			clients[i].offset = clients[i].timer - arena->time;
			clients[i].timer = arena->time;
			break;
		}
	}

	if (debug->value)
	{
		Com_Printf("DEBUG: numplayers %d\n", arena->numplayers);
	}
	Com_Printf("Incomming connection from %s - %s\n", clients[i].ip, GeoIP_country_name_by_addr(gi, clients[i].ip));
}

/*************
 RemoveClient

 Reset client structure
 *************/

void RemoveClient(client_t *client)
{
	u_int8_t i, j;

	if (!client || !client->inuse)
		return;

	if (client->socket != 0)
		Com_Close(&(client->socket));
	else
		Com_Printf("WARNING: Com_Close tried to close socket ZERO\n");

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
				Com_Printf("DEBUG: WARNING: RemoveClient() numplayers == 0\n");
			else
				Com_Printf("DEBUG: WARNING: RemoveClient() numplayers < 0\n");
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

	if (strlen(client->longnick))
	{
		if (client->ready)
		{
			if (!(client->attr == 1 && hideadmin->value))
				BPrintf(RADIO_GRAY, "%s became off-line", client->longnick);

			Com_LogEvent(EVENT_LOGOUT, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLIP, 0, client->ip);
			Com_LogDescription(EVENT_DESC_PLCTRID, client->ctrid, 0);

			sprintf(my_query, "DELETE FROM online_players WHERE player_id = '%u'", client->id);

			if (d_mysql_query(&my_sock, my_query)) // query succeeded
			{
				Com_Printf("WARNING: LOGOUT: couldn't query DELETE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}

		Com_Printf("%s removed (%d players)\n", client->longnick, arena->numplayers);
	}
	else
	{
		Com_Printf("%s removed (%d players)\n", client->ip, arena->numplayers);
	}

	//	slot = client->slot;

	memset(client, 0, sizeof(client_t));

	//	client->slot = slot;

	UpdateIngameClients(0);
}

/*************
 DebugClient

 Save Debug information and kick player
 *************/

void DebugClient(char *file, u_int32_t line, u_int8_t kick, client_t *client)
{
	char filename[128];
	time_t ltime;
	u_int16_t len, i;
	FILE *fp;

	memset(filename, 0, sizeof(filename));

	time(&ltime);

	sprintf(filename, "./debug/%u%s.txt.LOCK", (u_int32_t)ltime, client->longnick);

	Sys_WaitForLock(filename);

	if (Sys_LockFile(filename) < 0)
	{
		Com_Printf("WARNING: Couldn't open file \"%s\"\n", filename);
		return;
	}

	filename[strlen(filename) - 5] = '\0';

	if ((fp = fopen(filename, "wb")) == NULL)
	{
		Com_Printf("WARNING: Couldn't open file \"%s\"\n", filename);
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
		fprintf(fp, "status1           = %10u  status2       = %10u  reldist        = %10d\n", client->status1, client->status2, client->reldist);
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
		fprintf(fp, "tklimit           = %10u  fileframe     = %10u  timer          = %10u\n", client->tklimit, client->fileframe, client->timer);
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

	Com_Printf("WARNING: Bugged player (%s) - Error at %s, line %d\n", strlen(client->longnick) ? client->longnick : client->ip, file, line);

	if (kick)
		client->bugged = 1;
}

/*************
 ProcessClient

 Check if client needs processing and do it
 *************/

int ProcessClient(client_t *client)
{
	u_int8_t i;
	client_t *nearplane;
	client_t *near_en;
	int32_t x, y, z;

	if (client->disconnect)
	{
		Com_Printf("%s has requested to disconnect\n", client->longnick);
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
		Com_Printf("%s timed out\n", client->longnick);
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

				for(i = 0; i < cvs->value; i++)
				{
					SendCVRoute(client, i);
					SendCVPos(client, i);
				}

				if(client->hdserial || wb3->value)
				{
					CheckBanned(client);
					CreateScores(client);
				}
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
							Com_Printf("%s %s is in fire at %s\n",
									client-> longnick,
									GetHitSite(client->oildamaged),
									Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
							PPrintf(client, RADIO_WHITE, "Your %s is in Fire! Jump out!", GetHitSite(client->oildamaged));
						}
						client->oiltimer--;
					}
					else
					{
						Com_Printf("%s %s exploded at %s\n",
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

				if(wb3->value)
				{
					if(!((arena->frame - client->frame) % 30000))
					{
						if(!metar->value)
						WB3DotCommand(client, ".weather %u", (u_int8_t)weather->value);
						WB3DotCommand(client, ".clutterdistance 8500");
					}

					//				if(IsGround(client) && client->infly && !((arena->frame - client->frame) % 1000))
					//				{
					//					WB3Mapper(client);
					//				}
				}

				if(!((arena->frame - client->frame) % 30000))
				{
					UpdateClientFile(client); // update client database every 5 minutes
				}

				if(!((arena->frame - client->frame) % 180000)) // 30 minutes

				{
					if(client->tklimit)
					client->tklimit = 0;
				}

				if(predictpos->value && client->infly)
				{
					if((arena->time - client->postimer)> 600) // if client didnt send position packet in 500ms

					{
						Com_Printf("DEBUG: arena->time %u\n", arena->time);
						Com_Printf("DEBUG: arena - postimer %u\n", arena->time - client->postimer);
						BackupPosition(client, TRUE);
					}
				}

				if(!((arena->frame - client->frame) % 10))
				{
					SendPlayersNear(client);

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
								Com_Printf("%s collided emulated with %s\n", client, nearplane->longnick);
								
								
								PPrintf(client, RADIO_DARKGREEN, "%s collided with you!!!", nearplane->longnick);
								Com_Printf("%s collided emulated with %s\n", nearplane->longnick, client);

								client->damaged = 1;
								
								if(rand()%2)
								{
									SendForceStatus(STATUS_LWING, 0, nearplane); // lwing
									SendForceStatus(STATUS_RWING, 0, client); // rwing
								}
								else if(rand()%2)
								{
									SendForceStatus(STATUS_RWING, 0, nearplane); // rwing
									SendForceStatus(STATUS_LWING, 0, client); // lwing
								}
								else
								{
									SendForceStatus(STATUS_REARFUSE, 0, nearplane); // rfuse

									if(rand()%2)
									SendForceStatus(STATUS_LWING, 0, client);
									else if(rand()%2)
									SendForceStatus(STATUS_RWING, 0, client);
									else
									SendForceStatus(STATUS_REARFUSE, 0, client);
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
									client->hitby[0] = near_en;
									client->damby[0] = MAX_UINT32; // ignore compiler warning

									if(nearplane == near_en)
									near_en = client;

									nearplane->hitby[0] = near_en;
									nearplane->damby[0] = MAX_UINT32; // ignore compiler warning
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
						BPrintf(RADIO_GRAY, "%s is away and will be kicked", client->longnick);
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
						if(x < 0 && strlen(client->longnick))
						{
							BPrintf(RADIO_YELLOW, "%s broke connection", client->longnick);
							Com_Printf("%s broke connection\n", client->longnick);
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

	/*************
	 BackupPosition

	 Backup client position, leaving [0] free to predict or to store by PPlanePosition
	 *************/

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
		client->timer -= client->offset;
		client->postimer = arena->time;
		client->predict++;
	}
}

/*************
 ProcessLogin

 Run login sequence
 *************/
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
							Com_Printf("%s Ping\n", client->ip);
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
								Com_Printf("%s connected\n", client->longnick);
							else
								Com_Printf("%s connected\n", client->ip);
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
							Com_Printf("%s entered the game (%d players)\n", client->longnick, arena->numplayers);
						else
							Com_Printf("Creating a new nick for %s (%d players)\n", client->ip, arena->numplayers);

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
			if (!wb3->value)
			{
				n = Com_Recv(client->socket, mainbuffer, 1); // Receive client acknowledgement
				if (n > 0)
				{
					SendPlayersNames(client);
					return -1;
				}
				else if (n < 0)
				{
					return n;
				}
			}
			else
			{
				SendPlayersNames(client);
				return -1;
			}
			break;
		default:
			Com_Printf("login unk\n");
			return -1;
	}

	return 0;
}

/*************
 CalcLoginKey

 Calculates login key from a 256 bits array
 *************/

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

/*************
 LoginKey

 Send 32 bytes key to client
 *************/

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

/*************
 DecryptBlock

 Used to uncrypt login array
 *************/

void DecryptBlock(u_int8_t *Buf, int Len, long Key)
{
	int i;

	for (i = 0; i < Len; i += 8)
		DecryptOctet(Buf + i, Key);
}

/*************
 CheckUserPasswd

 Check user and password and se player's nick
 *************/

int8_t CheckUserPasswd(client_t *client, u_int8_t *userpass) // userpass is supposed to have 96 bytes
{
	u_int8_t buffer[2] =
	{ 0x00, 0x00 };
	char *user;
	char *pwd;
	u_int32_t cpwd;
	int8_t j = -1;

	DecryptBlock(userpass, 96, client->loginkey);

	userpass[31] = userpass[63] = '\0';

	user = (char*)userpass;
	pwd = (char*)userpass+32;

	if (!Com_CheckAphaNum(user))
	{
		cpwd = crc32(pwd, strlen(pwd));

		if (debug->value)
			Com_Printf("DEBUG: User %s, Passwd %s\n", user, pwd);

		sprintf(my_query, "SELECT * FROM players LEFT JOIN score_common ON players.id = score_common.player_id WHERE players.loginuser = '%s' and players.password = '%u'", user, cpwd);

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
						Com_Printf("WARNING: CheckUserPasswd(): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						buffer[0] = 0xc0;
					}
				}
				else // password didnt match, or player doesnt exist
				{
					sprintf(my_query, "INSERT INTO players (loginuser, password) VALUES ('%s', '%u')", user, cpwd);

					if (!d_mysql_query(&my_sock, my_query))
					{
						strcpy(client->loginuser, user);

						j = 0;
					}
					else
					{
						if (mysql_errno(&my_sock) != 1062)
						{
							Com_Printf("WARNING: CheckUserPasswd(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							buffer[0] = 0x04;
						}
						else
						{
							buffer[0] = 0xc0;
						}
					}
				}

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;
			}
			else
			{
				Com_Printf("WARNING: CheckUserPasswd(): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				buffer[0] = 0x04;
			}
		}
		else
		{
			Com_Printf("WARNING: CheckUserPasswd(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			buffer[0] = 0xc0;
		}
	}
	else
	{
		Com_Printf("WARNING: CheckUserPasswd(): user contains non-valid chars (loginkey = %u)\n", client->loginkey);
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

/*************
 LoginTypeRequest

 Checks if client are requesting login list or trying to login arena
 *************/

int8_t LoginTypeRequest(u_int8_t *buffer, client_t *client)
{
	u_int8_t i;

	DecryptBlock(buffer, 32, client->loginkey);

	Com_Printf("DEBUG: arenalist: %s\n", buffer);

	if (!Com_Strcmp((char*)buffer, "arnalst2") || !Com_Strcmp((char*)buffer, "arnalst3") /*wb3*/) // retrieving arenalist
	{
		return 1;
	}
	else// if(!Com_Strcmp((char*)buffer, dirname->string) || !Com_Strcmp((char*)buffer, mapname->string))// login in arena
	{
		client->loginkey = 1;

		for (i = 31; i > 25; i--)
		{
			if (buffer[i])
			{
				client->loginkey = 0;
				break;
			}
		}
		return 0;
	}
	//	else
	//		return -1;
}

/*************
 FindSClient

 Find a client in clients array using shortnick
 *************/

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

/*************
 FindLClient

 Find a client in clients array using longnick
 *************/

client_t *FindLClient(char *longnick)
{
	u_int8_t i;

	if (!longnick)
		return NULL;

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone && !Com_Strcmp(clients[i].longnick, longnick))
		{
			return &clients[i];
		}
	}
	return NULL;
}

/*************
 PPrintf

 Send a message to a player
 *************/

int PPrintf(client_t *client, u_int8_t radio, char *fmt, ...)
{
	va_list argptr;
	u_int8_t buffer[MAX_RADIOMSG]; // 74 is max size of a radio message
	char message[MAX_PRINTMSG];
	u_int8_t n=0;
	radiomessage_t *radiomessage;

	va_start(argptr, fmt);
	vsprintf(message, fmt, argptr);
	va_end(argptr);

	if (!client)
	{
		Com_Printf("%s\n", message);
	}
	else if (client->ready && !client->drone)
	{
		do
		{
			memset(buffer, 0, sizeof(buffer));
			radiomessage = (radiomessage_t *) (buffer);

			radiomessage->packetid = htons(Com_WBhton(0x1200));
			radiomessage->msgto = htonl(radio);
			radiomessage->msgfrom = htonl(ascii2wbnick("-HOST-", 1));
			radiomessage->msgsize = strlen(message+(63*n)) > 63 ? 63 : strlen(message+(63*n));
			memcpy(&(radiomessage->message), message+(63*n), radiomessage->msgsize);

			SendPacket(buffer, radiomessage->msgsize + 11, client);

			if (strlen(message+(63*n)) > 63)
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

/*************
 CPrintf

 Send a country message
 *************/

void CPrintf(u_int8_t country, u_int8_t radio, char *fmt, ...)
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

	Com_Printf("-HOST-:(%d)%s\n", radio, msg);
}

/*************
 BPrintf

 Send a broadcast message
 *************/

void BPrintf(u_int8_t radio, char *fmt, ...)
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

	Com_Printf("-HOST-:(%d)%s\n", radio, msg);
}

/*************
 CheckBanned

 Check if client has an hdserial banned. If does, ban client
 *************/

u_int8_t CheckBanned(client_t *client) // twin of CheckTK
{
	u_int8_t banned = 0;

	sprintf(
			my_query,
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
						Com_Printf("%s was banned for use banned hdserial\n", client->longnick);
					}
					else
					{
						Com_Printf("WARNING: CheckBanned(ban): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
				else
				{
					Com_Printf("WARNING: CheckBanned(check): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: CheckBanned(check): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf("WARNING: CheckBanned(check): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return banned;
}

/*************
 CheckTK

 Check if client was banned
 *************/

u_int8_t CheckTK(client_t *client) // twin of CheckBanned
{
	u_int8_t teamkiller = 0;

	if (wb3->value)
		sprintf(
				my_query,
				"SELECT ipaddress.ipaddr FROM players, players_ipaddress, ipaddress WHERE players.id = '%u' AND players.id = players_ipaddress.player_id AND ipaddress.id = players_ipaddress.ipaddress_id AND ipaddress.teamkiller = '1'",
				client->id);
	else
		sprintf(
				my_query,
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
						Com_Printf("%s was set TK for use TKed hdserial\n", client->longnick);
						client->tkstatus = 1;
						client->wings = 0;
					}
					else
					{
						Com_Printf("WARNING: CheckTK(flag): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
				else
				{
					Com_Printf("WARNING: CheckTK(check): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: CheckTK(check): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf("WARNING: CheckTK(check): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return teamkiller;
}

/*************
 GetClientInfo

 Get client info from file
 *************/

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

/*************
 UpdateClientFile

 Update client file, creates one if doesn't exist and get user info from file if in login process
 *************/

void UpdateClientFile(client_t *client)
{
	if (!client)
		return;

	if (!strlen(client->loginuser))
	{
		Com_Printf("WARNING: UpdateClientFile() tried to print a (null) loginuser\n");
		return;
	}

	sprintf(
			my_query,
			"UPDATE players SET countrytime = '%u', country = '%d', plane_id = '%u', fuel = '%d', ord = '%d', conv = '%d', easymode = '%d', radio = '%d', rank = '%d', lastseen = FROM_UNIXTIME(%u), ipaddr = '%s' WHERE loginuser = '%s' LIMIT 1",
			client->countrytime, client->country, client->plane, client->fuel, client->ord, client->conv, client->easymode, client->radio[0], client->rank, // Elo rating
			(u_int32_t)time(NULL), client->ip, client->loginuser);

	if (d_mysql_query(&my_sock, my_query))
	{
		Com_Printf("WARNING: UpdateClientFile(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/*************
 AddKiller

 Add a new killer assist and return its location or just return its location if already exists (added clean disconnected players)
 *************/

int8_t AddKiller(client_t *victim, client_t *client)
{
	u_int8_t i;
	int8_t found, empty;

	found = empty = -1;

	for (i = 0; i < MAX_HITBY; i++)
	{
		if (victim->hitby[i])
		{
			if (victim->hitby[i] == client)
				found = i;

			if (!victim->hitby[i]->inuse) // player had disconnected
			{
				if (found < 0)
				{
					victim->hitby[i] = client;
					victim->planeby[i] = client->attached ? client->attached->plane : client->plane;
					found = i;
				}
				else
				{
					victim->hitby[i] = NULL; // remove disconnected players
					empty = i;
				}
			}
		}
		else
		{
			empty = i;
		}
	}

	if (found < 0 && !(empty < 0))
	{
		victim->hitby[empty] = client;
		victim->planeby[empty] = client->attached ? client->attached->plane : client->plane;
		found = empty;
	}
	return found;
}

/*************
 ClearKillers

 Clear killers list
 *************/

void ClearKillers(client_t *client)
{
	u_int8_t i;

	if (client)
	{
		for (i = 0; i < MAX_HITBY; i++)
		{
			client->hitby[i] = NULL;
			client->damby[i] = 0;
			client->planeby[i] = 0;
		}
	}
}

/*************
 CheckKiller

 Check who killed client

 DEBUG: CHANGE IsFighter and IsBomber for a plane number function
 *************/

void CheckKiller(client_t *client)
{
	int8_t i, j;
	char buffer[128];
	u_int32_t damage;
	client_t *nearplane= NULL;

	if (!client->infly) // if not infly, cant be killed
	{
		ClearKillers(client);
		return;
	}

	// DEBUG
	Com_Printf("DEBUG: Dump %s (%s) data:\n", client->longnick, GetSmallPlaneName(client->plane));
	for (i = 0, j = 0; i < MAX_HITBY; i++)
	{
		if (client->hitby[i])
		{
			if (client->hitby[i]->inuse)
				Com_Printf("DEBUG: %u - hitby = %s, damby = %u, planeby = %s (%s)\n", i, client->hitby[i]->longnick, client->damby[i], GetSmallPlaneName(client->planeby[i]),
						GetSmallPlaneName(client->hitby[i]->plane));
			else
				Com_Printf("DEBUG: %u - hitby = %s (offline), damby = %u, planeby = %s (%s)\n", i, client->hitby[i]->longnick, client->damby[i], GetSmallPlaneName(client->planeby[i]),
						GetSmallPlaneName(client->hitby[i]->plane));
		}
		else
		{
			Com_Printf("DEBUG: %u - hitby = (null), damby = %u, planeby = %s\n", i, client->damby[i], GetSmallPlaneName(client->planeby[i]));
		}
	}
	//
	Com_Printf("DEBUG: Check if any logged player had hit client\n");
	for (i = 0, j = 0; i < MAX_HITBY; i++) // check if any logged player had hit client
	{
		if (client->hitby[i])
		{
			if (client->hitby[i]->inuse && client->damby[i])
			{
				Com_Printf("DEBUG: hit found (%s)\n", client->hitby[i]->longnick);
				j = 1;
				break;
			}
		}
	}

	if (!j) // if no one in hit list, get nearest enemy plane within 2000 for maneuver kill
	{
		Com_Printf("DEBUG: hit not found\n");
		Com_Printf("DEBUG: Check nearest player (2000 feets radius)\n");
		if ((client->posalt[0] - GetHeightAt(client->posxy[0][0], client->posxy[1][0])) <= 164) // explosions above 50mts are not maneuver kill
		{
			nearplane = NearPlane(client, client->country, 2000);

			if (nearplane)
			{
				Com_Printf("DEBUG: Found nearest player (%s)\n", nearplane->longnick);
				j = 1;
				client->status1 = 0;
				client->cancollide = 0;
				client->hitby[0] = nearplane;
				client->damby[0] = MAX_UINT32; // ignore compiler warning
				client->planeby[0] = nearplane->plane;
				Com_Printf("DEBUG: Set hitby[0] to %s, damby[0] to %u and planeby[0] to %s\n", client->hitby[0]->longnick, client->damby[0], GetSmallPlaneName(client->planeby[0]));
			}
		}
	}

	if (j) // if anyone at list (hit or nearest enemy)
	{
		Com_Printf("DEBUG: Now check who did more damage\n");
		for (i = 0, j = 0, damage = 0; i < MAX_HITBY; i++) // check who inflicted more damage
		{
			if (client->hitby[i]) // if client in list
			{
				if (client->hitby[i]->inuse) // if still connected
				{
					Com_Printf("DEBUG: %s - %u\n", client->hitby[i]->longnick, client->damby[i]);
					if (client->damby[i] > damage) // if damage > current damage
					{
						damage = client->damby[i]; // set current damage as attacker damage
						client->damby[i] = 0; // clear damby value
						j = i; // set j as index of max damage
					}
				}
				else // else, clear from list
				{
					Com_Printf("DEBUG: Clearing offline player from array[%u]\n", i);
					client->hitby[i] = NULL;
					client->damby[i] = 0;
				}
			}
		}

		Com_Printf("DEBUG: Server has chosen %s as killer!!!\n", client->hitby[j]->longnick);

		Com_LogEvent(EVENT_KILL, client->hitby[j] == client?0:client->hitby[j]->id, client->id);
		if (client->hitby[j] != client)
		{
			Com_LogDescription(EVENT_DESC_PLPLANE, client->planeby[j], NULL);

			if (IsFighter(NULL, client->planeby[j]))
				Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
			else if (IsBomber(NULL, client->planeby[j]))
				Com_LogDescription(EVENT_DESC_PLPLTYPE, 2, NULL);
			else if (IsGround(NULL, client->planeby[j]))
				Com_LogDescription(EVENT_DESC_PLPLTYPE, 3, NULL);
			else
			{
				Com_Printf("WARNING: Plane not classified (N%d)\n", client->plane);
				Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
			}

			Com_LogDescription(EVENT_DESC_PLCTRY, client->hitby[j]->country, NULL);
			Com_LogDescription(EVENT_DESC_PLORD, client->hitby[j]->ord, NULL);
		}
		Com_LogDescription(EVENT_DESC_VCPLANE, client->plane, NULL);

		if (IsFighter(client))
			Com_LogDescription(EVENT_DESC_VCPLTYPE, 1, NULL);
		else if (IsBomber(client))
			Com_LogDescription(EVENT_DESC_VCPLTYPE, 2, NULL);
		else if (IsGround(client))
			Com_LogDescription(EVENT_DESC_VCPLTYPE, 3, NULL);
		else
		{
			Com_Printf("WARNING: Plane not classified (N%d)\n", client->plane);
			Com_LogDescription(EVENT_DESC_VCPLTYPE, 3, NULL);
		}

		Com_LogDescription(EVENT_DESC_VCCTRY, client->country, NULL);
		Com_LogDescription(EVENT_DESC_VCORD, client->ord, NULL);

		/*************************************************
		 Insert values in score_kills table
		 
		 Variables: player_id, player_plane, player_pltype, player_country
		 victim_id, victim_plane, victim_pltype, victim_country
		 *************************************************/

		sprintf(my_query, "INSERT INTO score_kills VALUES(");

		if (client->hitby[j] == client)
		{
			strcat(my_query, "'', '0', '0', '0', '0',");
		}
		else
		{
			sprintf(my_query, "%s'', '%u', '%u', '%u', '%u',", my_query, client->hitby[j]->id, client->planeby[j],
					IsFighter(NULL, client->planeby[j]) ? 1 : IsBomber(NULL, client->planeby[j]) ? 2 : 3, client->hitby[j]->country);
		}

		sprintf(my_query, "%s '%u', '%u', '%u', '%u')", my_query, client->id, client->plane, IsFighter(client) ? 1 : IsBomber(client) ? 2 : 3, client->country);

		if (d_mysql_query(&my_sock, my_query))
		{
			Com_Printf("WARNING: CheckKiller(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}

		if ((client->hitby[j]->country == client->country) && (client->hitby[j] != client)) // not ack, TK
		{
			sprintf(buffer, "TeamKill of %s(%s) by %s(%s)", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j]->longnick, GetSmallPlaneName(client->planeby[j]));

			// in TK, winner is the killed pilot.
			if(IsFighter(client->hitby[j]) && IsFighter(client))
				CalcEloRating(client /*winner*/, client->hitby[j] /*looser*/, ELO_LOOSER);

			if (!client->tkstatus) // if victim is not TK, add penalty to killer
			{
				if (teamkiller->value)
					client->hitby[j]->tklimit++;
			}

			if (client->hitby[j]->tklimit > 5)
			{
				if (!client->hitby[j]->tkstatus)
					Cmd_TK(client->hitby[j]->longnick, TRUE, NULL);
				else
					; // FIXME: BAN CLIENT UNTIL END OF TOD
			}

			if (client->hitby[j]->squadron)
				sprintf(buffer, "%s from %s", buffer, Com_SquadronName(client->hitby[j]->squadron));

			if (printkills->value)
			{
				BPrintf(RADIO_GREEN, buffer);
			}

			Com_Printf("%s at %s\n", buffer, Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

			if (!client->hitby[j]->drone)
			{
				if (!client->tkstatus)
					sprintf(my_query, "UPDATE score_penalty SET");
				else
				{
					if (IsFighter(NULL, client->planeby[j]))
						sprintf(my_query, "UPDATE score_fighter SET");
					else if (IsBomber(NULL, client->planeby[j]))
						sprintf(my_query, "UPDATE score_bomber SET");
					else if (IsGround(NULL, client->planeby[j]))
						sprintf(my_query, "UPDATE score_ground SET");
					else
					{
						Com_Printf("WARNING: Plane not classified (N%d)\n", client->plane);
						sprintf(my_query, "UPDATE score_fighter SET");
					}
				}
			}
		}
		else// if((client->hitby[j]->country != client->country) || (client->hitby[j] == client)) // enemy kill
		{
			if ((client->status1 & STATUS_PILOT) && client->chute)
				sprintf(buffer, "Chutekill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j] == client ? "Ack Weenies" : client->hitby[j]->longnick);
			else if (client->cancollide < 0)
				sprintf(buffer, "Collision kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j] == client ? "Ack Weenies" : client->hitby[j]->longnick);
			else if ((client->status1 & 0x18163F0F) || client->chute || client->drone)
				sprintf(buffer, "Kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j] == client ? "Ack Weenies" : client->hitby[j]->longnick);
			else
			{
				if (!nearplane)
					nearplane = NearPlane(client, client->country, 2000);

				if (!nearplane)
					client->hitby[j] = client; // so Ack weenies wont receive "(plane) from squadron"...

				sprintf(buffer, "Maneuver kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), nearplane ? nearplane->longnick : "Ack Weenies");
			}

			if(IsFighter(client->hitby[j]) && IsFighter(client))
				CalcEloRating(client->hitby[j] /*winner*/, client /*looser*/, ELO_BOTH);

			if (client->hitby[j] != client) // not ack kill
			{
				sprintf(buffer, "%s(%s)", buffer, GetSmallPlaneName(client->planeby[j]));

				if (!client->hitby[j]->drone)
				{
					if (IsFighter(NULL, client->planeby[j]))
						sprintf(my_query, "UPDATE score_fighter SET");
					else if (IsBomber(NULL, client->planeby[j]))
						sprintf(my_query, "UPDATE score_bomber SET");
					else if (IsGround(NULL, client->planeby[j]))
						sprintf(my_query, "UPDATE score_ground SET");
					else
					{
						Com_Printf("WARNING: Plane not classified (N%d)\n", client->plane);
						sprintf(my_query, "UPDATE score_fighter SET");
					}
				}
			}

			if (printkills->value)
			{
				BPrintf(RADIO_YELLOW, buffer);
			}

			Com_Printf("%s at %s\n", buffer, Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
		}

		//score
		if (client->hitby[j] != client && !client->hitby[j]->drone) // not ack kill neither drone
		{
			switch (client->plane)
			{
				case 61:
					strcat(my_query, " killcommandos = killcommandos + '1'");

					if (client->hitby[j]->country != client->country)
					{
						client->hitby[j]->score.groundscore += SCORE_COMMANDO;
					}
					else
					{
						client->hitby[j]->score.penaltyscore += 2 * SCORE_COMMANDO;
					}
					break;
				case PLANE_FAU:
					strcat(my_query, " killfau = killfau + '1'");

					if (client->hitby[j]->country != client->country)
					{
						client->hitby[j]->score.airscore += SCORE_FAU;
					}
					else
					{
						client->hitby[j]->score.penaltyscore += 2 * SCORE_FAU;
					}
					break;
				case 85:
					if (wb3->value)
					{
						; // let go to default:
					}
					else
					{
						if (client->drone & (DRONE_HMACK | DRONE_AAA))
						{
							strcat(my_query, " killhmack = killhmack + '1'");

							if (client->hitby[j]->country != client->country)
							{
								client->hitby[j]->score.groundscore += SCORE_HMACK;
							}
							else
							{
								client->hitby[j]->score.penaltyscore += 2 * SCORE_HMACK;
							}
						}
						else if (client->drone & DRONE_KATY)
						{
							strcat(my_query, " killkaty = killkaty + '1'");

							if (client->hitby[j]->country != client->country)
							{
								client->hitby[j]->score.groundscore += SCORE_KATY;
							}
							else
							{
								client->hitby[j]->score.penaltyscore += 2 * SCORE_KATY;
							}
						}
						break;
					}
				case 101:
					if (wb3->value)
					{
						; // let go to default:
					}
					else
					{
						strcat(my_query, " killtank = killtank + '1'");

						if (client->hitby[j]->country != client->country)
						{
							client->hitby[j]->score.groundscore += SCORE_TANK;
						}
						else
						{
							client->hitby[j]->score.penaltyscore += 2 * SCORE_TANK;
						}
						break;
					}
				default:
					if (client->infly)
					{
						if (wb3->value && IsGround(client))
						{
							strcat(my_query, " killtank = killtank + '1'");

							client->hitby[j]->killssortie++;
							client->hitby[j]->killstod++;

							if (client->hitby[j]->country != client->country)
							{
								strcat(my_query, ", curr_streak = curr_streak + '1', long_streak = IF(curr_streak > long_streak, curr_streak, long_streak)");
								client->hitby[j]->score.groundscore += SCORE_TANK;
							}
							else
							{
								client->hitby[j]->score.penaltyscore += 2 * SCORE_TANK;
							}
						}
						else
						{
							strcat(my_query, " kills = kills + '1'");

							client->hitby[j]->killssortie++;
							client->hitby[j]->killstod++;

							if (client->hitby[j]->country != client->country)
							{
								strcat(my_query, ", curr_streak = curr_streak + '1', long_streak = IF(curr_streak > long_streak, curr_streak, long_streak)");
								client->hitby[j]->score.airscore += SCORE_KILL;
							}
							else
							{
								client->hitby[j]->score.penaltyscore += 2 * SCORE_KILL;
							}
						}
					}
			}

			sprintf(my_query, "%s WHERE player_id = '%u'", my_query, client->hitby[j]->id);

			if (d_mysql_query(&my_sock, my_query))
			{
				PPrintf(client, RADIO_YELLOW, "CheckKiller(): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
				Com_Printf("WARNING: CheckKiller(): couldn't query UPDATE error %d: %s query %s\n", mysql_errno(&my_sock), mysql_error(&my_sock), my_query);
			}
		}

		if ((client->hitby[j] != client) && (client->hitby[j]->country != client->country))
		{
			if (client->hitby[j]->squadron)
			{
				if (printkills->value)
					BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(client->hitby[j]->squadron));
				else
					Com_Printf("`-> from %s\n", Com_SquadronName(client->hitby[j]->squadron));
			}
		}

		client->hitby[j] = NULL; // clear killer from list

		char query_bomber[512];
		char query_ground[512];

		j = 0;

		sprintf(my_query, "UPDATE score_fighter SET assists = assists + '1' WHERE player_id IN(");
		sprintf(query_bomber, "UPDATE score_bomber SET assists = assists + '1' WHERE player_id IN(");
		sprintf(query_ground, "UPDATE score_ground SET assists = assists + '1' WHERE player_id IN(");

		for (i = 0; i < MAX_HITBY; i++) // check who remain in list and give piece score
		{
			if (client->hitby[i] && client->damby[i] && !client->hitby[i]->drone && client->hitby[i]->infly)
			{
				if ((client->hitby[i] != client) && (client->damby[i] <= damage)) // if not ack damage (dont give piece do acks please, they don't deserves hehehe)
				{
					if (client->hitby[i]->country != client->country) // if enemy
					{
						if (printkills->value)
						{
							PPrintf(client->hitby[i], RADIO_YELLOW, "You've got a piece of %s", client->longnick);
						}

						Com_Printf("%s got a piece of %s\n", client->hitby[i]->longnick, client->longnick);

						client->hitby[i]->score.airscore += SCORE_ASSIST;

						if (IsFighter(NULL, client->planeby[i]))
						{
							sprintf(my_query, "%s'%u',", my_query, client->hitby[i]->id);
							j |= 1;
						}
						else if (IsBomber(NULL, client->planeby[i]))
						{
							sprintf(query_bomber, "%s'%u',", query_bomber, client->hitby[i]->id);
							j |= 2;
						}
						else if (IsGround(NULL, client->planeby[i]))
						{
							sprintf(query_ground, "%s'%u',", query_ground, client->hitby[i]->id);
							j |= 4;
						}
						else
						{
							Com_Printf("WARNING: Plane not classified (N%d)\n", client->plane);
							sprintf(my_query, "%s'%u',", my_query, client->hitby[i]->id);
							j |= 1;
						}
					}
					else // friendly kill... tsc, tsc, tsc...
					{
						if (printkills->value)
						{
							PPrintf(client->hitby[i], RADIO_YELLOW, "You've got a piece of your friend %s", client->longnick);
						}
						else
						{
							Com_Printf("%s got a piece of his friend %s\n", client->hitby[i]->longnick, client->longnick);
						}

						client->hitby[i]->score.penaltyscore += SCORE_ASSIST;
					}
				}
			}

			client->hitby[i] = NULL; // remove everyone from list
			client->damby[i] = 0; // and reset damby
		}

		if (j & 1)
		{
			i = strlen(my_query);

			if (i < sizeof(my_query))
			{
				my_query[i - 1] = ')';

				if (d_mysql_query(&my_sock, my_query))
				{
					Com_Printf("WARNING: CheckKiller(assists): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		if (j & 2)
		{
			i = strlen(query_bomber);

			if (i < sizeof(query_bomber))
			{
				query_bomber[i - 1] = ')';

				if (d_mysql_query(&my_sock, query_bomber))
				{
					Com_Printf("WARNING: CheckKiller(assists2): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		if (j & 4)
		{
			i = strlen(query_ground);

			if (i < sizeof(query_ground))
			{
				query_ground[i - 1] = ')';

				if (d_mysql_query(&my_sock, query_ground))
				{
					Com_Printf("WARNING: CheckKiller(assists3): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
	}

	ClearKillers(client);
}

/*************
 CalcEloRating

 Calc rank based in Elo Rating
 *************/

void CalcEloRating(client_t *winner, client_t *looser, u_int8_t flags)
{
	float Ea, Eb, K;

	if (!winner->drone && !looser->drone)
	{
		if ((flags & ELO_WINNER) && (winner != looser) /*ack dont have rank*/)
		{
			Ea = 1 / (1 + powf(10, (float)(looser->rank - winner->rank) / 400));

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
			Eb = 1 / (1 + powf(10, (float)(winner->rank - looser->rank) / 400));

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

/*************
 NearPlane

 Returns nearest != country plane in visible list
 *************/

client_t *NearPlane(client_t *client, u_int8_t country, int32_t limit)
{
	u_int8_t i, j;
	u_int32_t dist;

	dist = MAX_UINT32; // ignore compiler warning

	for (i = 0, j = MAX_SCREEN; i < MAX_SCREEN; i++)
	{
		if (client->visible[i].client && (client->visible[i].client->country != country))
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

/*************
 CheckMedals

 Check if client receive a new medal
 *************/

u_int8_t CheckMedals(client_t *client)
{
	u_int8_t i, new = 0;
	int16_t value;

	//kills in a sortie

	value = client->killssortie;

	if (value >= 3)
	{
		if (value >= 6)
		{
			if (value >= 9)
			{
				if (value >= 12)
				{
					if (value >= 15)
					{
						if (value >= 20)
						{
							new += AddMedal(DEED_KILLSSORTIE, MEDAL_GRANDSTAR, value, client);
						}
						else
							new += AddMedal(DEED_KILLSSORTIE, MEDAL_GRANDCROSS, value, client);
					}
					else
						new += AddMedal(DEED_KILLSSORTIE, MEDAL_ORDERFALCON, value, client);
				}
				else
					new += AddMedal(DEED_KILLSSORTIE, MEDAL_SILVERSTAR, value, client);
			}
			else
				new += AddMedal(DEED_KILLSSORTIE, MEDAL_COMBATCROSS, value, client);
		}
		else
			new += AddMedal(DEED_KILLSSORTIE, MEDAL_COMBATMEDAL, value, client);
	}

	// kills TOD

	value = client->killstod;

	if (value >= 15)
	{
		if (value >= 30)
		{
			if (value >= 60)
			{
				if (value >= 120)
				{
					if (value >= 250)
					{
						if (value >= 500)
						{
							new += AddMedal(DEED_KILLSTOD, MEDAL_GRANDSTAR, value, client);
						}
						else
							new += AddMedal(DEED_KILLSTOD, MEDAL_GRANDCROSS, value, client);
					}
					else
						new += AddMedal(DEED_KILLSTOD, MEDAL_SILVERSTAR, value, client);
				}
				else
					new += AddMedal(DEED_KILLSTOD, MEDAL_ORDERFALCON, value, client);
			}
			else
				new += AddMedal(DEED_KILLSTOD, MEDAL_COMBATCROSS, value, client);
		}
		else
			new += AddMedal(DEED_KILLSTOD, MEDAL_COMBATMEDAL, value, client);
	}

	// Struct TOD

	value = client->structstod;

	if (value >= 10)
	{
		if (value >= 25)
		{
			if (value >= 50)
			{
				if (value >= 100)
				{
					if (value >= 200)
					{
						if (value >= 400)
						{
							new += AddMedal(DEED_STRUCTSTOD, MEDAL_GRANDSTAR, value, client);
						}
						else
							new += AddMedal(DEED_STRUCTSTOD, MEDAL_GRANDCROSS, value, client);
					}
					else
						new += AddMedal(DEED_STRUCTSTOD, MEDAL_SILVERSTAR, value, client);
				}
				else
					new += AddMedal(DEED_STRUCTSTOD, MEDAL_ORDERFALCON, value, client);
			}
			else
				new += AddMedal(DEED_STRUCTSTOD, MEDAL_COMBATCROSS, value, client);
		}
		else
			new += AddMedal(DEED_STRUCTSTOD, MEDAL_COMBATMEDAL, value, client);
	}

	// streak kills
	sprintf(
			my_query,
			"SELECT t1.curr_streak as fighter_streak,	t2.curr_streak as bomber_streak, (t1.kills + t2.kills + t3.kills + t1.killfau) as kills, (t1.acks + t1.buildings + t1.ships + t1.cvs + t2.acks + t2.buildings + t2.ships + t2.cvs + t3.acks + t3.buildings + t3.ships + t3.cvs) as buildings, (t2.fieldscapt + t3.fieldscapt) as fieldscapt FROM score_fighter as t1, score_bomber as t2, score_ground as t3 WHERE t1.player_id = '%u' AND t1.player_id = t2.player_id AND t2.player_id = t3.player_id",
			client->id);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if ((my_row = mysql_fetch_row(my_result)))
			{
				value = Com_Atou(Com_MyRow("fighter_streak"));

				if (value >= 5)
				{
					if (value >= 10)
					{
						if (value >= 15)
						{
							if (value >= 20)
							{
								if (value >= 30)
								{
									if (value >= 50)
									{
										new += AddMedal(DEED_STREAKKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += AddMedal(DEED_STREAKKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += AddMedal(DEED_STREAKKILLS, MEDAL_ORDERFALCON, value, client);
							}
							else
								new += AddMedal(DEED_STREAKKILLS, MEDAL_SILVERSTAR, value, client);
						}
						else
							new += AddMedal(DEED_STREAKKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += AddMedal(DEED_STREAKKILLS, MEDAL_COMBATMEDAL, value, client);
				}

				// bomber streak kills

				value = Com_Atou(Com_MyRow("bomber_streak"));

				if (value >= 5)
				{
					if (value >= 10)
					{
						if (value >= 15)
						{
							if (value >= 20)
							{
								if (value >= 25)
								{
									if (value >= 30)
									{
										new += AddMedal(DEED_BSTREAKKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += AddMedal(DEED_BSTREAKKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += AddMedal(DEED_BSTREAKKILLS, MEDAL_ORDERFALCON, value, client);
							}
							else
								new += AddMedal(DEED_BSTREAKKILLS, MEDAL_SILVERSTAR, value, client);
						}
						else
							new += AddMedal(DEED_BSTREAKKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += AddMedal(DEED_BSTREAKKILLS, MEDAL_COMBATMEDAL, value, client);
				}

				// Career Kills

				value = Com_Atou(Com_MyRow("kills"));

				if (value >= 35)
				{
					if (value >= 70)
					{
						if (value >= 140)
						{
							if (value >= 280)
							{
								if (value >= 560)
								{
									if (value >= 1120)
									{
										new += AddMedal(DEED_CAREERKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += AddMedal(DEED_CAREERKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += AddMedal(DEED_CAREERKILLS, MEDAL_SILVERSTAR, value, client);
							}
							else
								new += AddMedal(DEED_CAREERKILLS, MEDAL_ORDERFALCON, value, client);
						}
						else
							new += AddMedal(DEED_CAREERKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += AddMedal(DEED_CAREERKILLS, MEDAL_COMBATMEDAL, value, client);
				}

				// Career Struct

				value = Com_Atou(Com_MyRow("buildings"));

				if (value >= 30)
				{
					if (value >= 60)
					{
						if (value >= 120)
						{
							if (value >= 250)
							{
								if (value >= 500)
								{
									if (value >= 1000)
									{
										new += AddMedal(DEED_CAREERSTRUCTS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += AddMedal(DEED_CAREERSTRUCTS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += AddMedal(DEED_CAREERSTRUCTS, MEDAL_SILVERSTAR, value, client);
							}
							else
								new += AddMedal(DEED_CAREERSTRUCTS, MEDAL_ORDERFALCON, value, client);
						}
						else
							new += AddMedal(DEED_CAREERSTRUCTS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += AddMedal(DEED_CAREERSTRUCTS, MEDAL_COMBATMEDAL, value, client);
				}

				// Fields Captured

				value = Com_Atou(Com_MyRow("fieldscapt"));

				if (value >= 5)
				{
					if (value >= 10)
					{
						if (value >= 15)
						{
							if (value >= 20)
							{
								if (value >= 25)
								{
									if (value >= 30)
									{
										new += AddMedal(DEED_CAREERFIELDS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += AddMedal(DEED_CAREERFIELDS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += AddMedal(DEED_CAREERFIELDS, MEDAL_SILVERSTAR, value, client);
							}
							else
								new += AddMedal(DEED_CAREERFIELDS, MEDAL_ORDERFALCON, value, client);
						}
						else
							new += AddMedal(DEED_CAREERFIELDS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += AddMedal(DEED_CAREERFIELDS, MEDAL_COMBATMEDAL, value, client);
				}

			}
			else
			{
				Com_Printf("WARNING: CheckMedals(): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: CheckMedals(): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf("WARNING: UNNAMED: couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	sprintf(my_query, "SELECT COUNT(*) FROM medals WHERE player_id = '%u'", client->id);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if ((my_row = mysql_fetch_row(my_result)))
			{
				i = Com_Atou(my_row[0]);

				if (i < 4)
				{
					client->ranking = 0;
				}
				else if (i < 7)
				{
					client->ranking = 1;
				}
				else if (i < 10)
				{
					client->ranking = 2;
				}
				else if (i < 13)
				{
					client->ranking = 3;
				}
				else
				{
					client->ranking = 4;
				}

				sprintf(my_query, "UPDATE score_common SET ranking = '%u' WHERE player_id = '%u'", client->ranking, client->id);

				if (d_mysql_query(&my_sock, my_query)) // query succeeded
				{
					Com_Printf("WARNING: CheckMedals(rank): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				Com_Printf("WARNING: CheckMedals(rank): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf("WARNING: CheckMedals(rank): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf("WARNING: CheckMedals(rank): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return new;
}

/*************
 new = AddMedal

 Add a new medal to client if needed
 *************/

u_int8_t AddMedal(u_int8_t deed, u_int8_t medal, u_int16_t value, client_t *client)
{
	sprintf(my_query, "INSERT INTO medals SET player_id = '%u', what = '%u', why = '%u', date_time = FROM_UNIXTIME(%u), howmuch = '%u'", client->id, medal, deed, (u_int32_t)time(NULL), value);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		Com_LogEvent(EVENT_MEDAL, client->id, 0);
		Com_LogDescription(EVENT_DESC_MDWHAT, medal, NULL);
		Com_LogDescription(EVENT_DESC_MDWHY, deed, NULL);
		Com_LogDescription(EVENT_DESC_MDHM, value, NULL);
	}
	else
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf("WARNING: AddMedal(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}

		return 0;
	}

	return 1;
}

/*************
 ForceEndFlight

 Force client to return to tower
 DEBUG: check this in WB3
 *************/

void ForceEndFlight(u_int8_t remdron, client_t *client)
{
	u_int8_t buffer[10];
	u_int8_t i;
	char field[8];

	memset(buffer, 0, sizeof(buffer));
	buffer[0] = buffer[1] = 2;
	buffer[3] = 4;
	sprintf(field, "f%d", client->field);

	SendPacket(buffer, sizeof(buffer), client);
	Cmd_Move(field, client->country, client);

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

	client->status1 = client->status2 = client->infly = client->chute = client->obradar = client->mortars = client->cancollide = client->fueltimer = 0;

	for (i = 0; i < MAX_HITBY; i++)
	{
		client->hitby[i] = 0;
		client->damby[i] = 0;
	}

	client->speedxyz[0][0] = client->speedxyz[1][0] = client->speedxyz[2][0] = 0;

	client->view = client->shanghai = client->attached = NULL;
}

/*************
 ReloadWeapon

 Reload client weapons
 *************/

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

/*************
 WB3ClientSkin

 Set client skin
 *************/

void WB3ClientSkin(u_int8_t *buffer, client_t *client)
{
	u_int8_t i, j;
	wb3planeskin_t *clientskin;

	clientskin = (wb3planeskin_t *) buffer;

	if(clientskin->msgsize < 64)
	{
		memcpy(client->skin, &(clientskin->msg), clientskin->msgsize);
		
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
	
	Com_Printf("%s skin set to \"%s\"\n", client->longnick, client->skin);
}

/*************
 WB3OverrideSkin

 Make a Pingtest call
 *************/

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
	
			Com_Printf("%s[%d] sent skin to %s \"%s\"\n", client->visible[slot].client->longnick, slot, client->longnick, client->visible[slot].client->skin);
			
			SendPacket(buffer, size + 4, client);
		}
	}
}

/*************
 CreateScores

 Create Score rows
 *************/

void CreateScores(client_t *client)
{
	sprintf(my_query, "INSERT INTO score_fighter (player_id) VALUES ('%u')", client->id);

	if (!d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		sprintf(my_query, "INSERT INTO score_bomber (player_id) VALUES ('%u')", client->id);

		if (!d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			sprintf(my_query, "INSERT INTO score_ground (player_id) VALUES ('%u')", client->id);

			if (!d_mysql_query(&my_sock, my_query)) // query succeeded
			{
				sprintf(my_query, "INSERT INTO score_penalty (player_id) VALUES ('%u')", client->id);

				if (!d_mysql_query(&my_sock, my_query)) // query succeeded
				{
					sprintf(my_query, "INSERT INTO score_common (player_id) VALUES ('%u')", client->id);

					if (d_mysql_query(&my_sock, my_query)) // query succeeded
					{
						if (mysql_errno(&my_sock) != 1062)
						{
							Com_Printf("WARNING: CreateScores(penalty): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}
				}
				else
				{
					if (mysql_errno(&my_sock) != 1062)
					{
						Com_Printf("WARNING: CreateScores(ground): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
			}
			else
			{
				if (mysql_errno(&my_sock) != 1062)
				{
					Com_Printf("WARNING: CreateScores(ground): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		else
		{
			if (mysql_errno(&my_sock) != 1062)
			{
				Com_Printf("WARNING: CreateScores(bomber): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
	}
	else
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf("WARNING: CreateScores(fighter): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
}

/*************
 ClientHDSerial

 Get player's hdserial, add it to hdserials table and players_hdserials table
 *************/

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
			Com_Printf("WARNING: ClientHDSerial(id): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
			Com_Printf("WARNING: ClientHDSerial(insert): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
}

/*************
 ClientIpaddr

 Get player's ipaddr, add it to ipaddr table and players_ipaddress table
 *************/

void ClientIpaddr(client_t *client)
{
	u_int32_t ipaddr_id;

	if (!client || !strlen(client->ip))
		return;

	sprintf(my_query, "INSERT INTO ipaddress (ipaddr) VALUES ('%s')", client->ip);

	if (d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf("WARNING: ClientIpaddr(id): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			return;
		}
	}

	ipaddr_id = (u_int32_t)mysql_insert_id(&my_sock);

	if (!ipaddr_id)
		Com_Printf("DEBUG: ClientIpaddr(ipaddr_id) = 0, IP %s\n", client->ip);

	//	sprintf(my_query, "SELECT id FROM ipaddress WHERE ipaddr = '%s'", client->ip);

	sprintf(my_query, "INSERT INTO players_ipaddress (player_id, ipaddress_id) VALUES ('%u', '%u')", client->id, ipaddr_id);

	if (d_mysql_query(&my_sock, my_query))
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf("WARNING: ClientIpaddr(insert): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
}

/*************
 LogRAWPosition

 Log client RAW position to later analysis
 *************/

void LogRAWPosition(u_int8_t server, client_t *client)
{
	char file[128];
	FILE *fp;

	if (server)
		sprintf(file, "./logs/%s.srv", client->longnick);
	else
		sprintf(file, "./logs/%s.cli", client->longnick);

	if (!(fp = fopen(file, "a")))
	{
		Com_Printf("WARNING: Couldn't append file \"%s\"\n", file);
	}
	else
	{
		fprintf(fp, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%u;%d;%u;%u\n", client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->angles[0][0], client->angles[1][0],
				client->angles[2][0], client->speedxyz[0][0], client->speedxyz[1][0], client->speedxyz[2][0], client->accelxyz[0][0], client->accelxyz[1][0], client->accelxyz[2][0],
				client->aspeeds[0][0], client->aspeeds[1][0], client->aspeeds[2][0], client->timer, client->offset, arena->time, Sys_Milliseconds());
		fclose(fp);
	}
}

/*************
 LogPosition

 Log client position to later analysis
 *************/

void LogPosition(client_t *client)
{
	FILE *fp;
	char filename[128];

	sprintf(filename, "./logs/%s.pos", client->logfile);

	if (!(fp = fopen(filename, "a")))
	{
		Com_Printf("WARNING: Couldn't append file \"%s\"\n", filename);
	}
	else
	{
		fprintf(fp, "%d;%d;%d;%.0f;%.0f;%.0f;%u\n", client->posxy[0][0], client->posxy[1][0], client->posalt[0], FloorDiv(client->angles[0][0], 10), FloorDiv(client->angles[1][0], 10),
				WBtoHdg(client->angles[2][0]), (u_int32_t)time(NULL));
		fclose(fp);
	}
}

/*************
 HardHit

 Make a hardhit query (add hits in DB)
 *************/

void HardHit(u_int8_t munition, u_int8_t penalty, client_t *client)
{
	if (munition >= maxmuntype)
	{
		Com_Printf("WARNING: HardHit(): Munition ID overflow %d. maxmuntype=%d\n", munition, maxmuntype);
		return;
	}
	if (!arena->munition[munition].type)
		return;

	if (IsFighter(client))
	{
		sprintf(my_query, "UPDATE score_fighter SET");
	}
	else if (IsBomber(client))
	{
		sprintf(my_query, "UPDATE score_bomber SET");
	}
	else if (IsGround(client))
	{
		sprintf(my_query, "UPDATE score_ground SET");
	}
	else
	{
		Com_Printf("WARNING: Plane not classified (N%d)\n", client->plane);
		sprintf(my_query, "UPDATE score_fighter SET");
	}

	if (arena->munition[munition].type == 2) // rocket
	{
		if (client->infly)
		{
			if (penalty)
				client->score.groundscore += (SCORE_ROCKETHIT * -20);
			else
				client->score.groundscore += SCORE_ROCKETHIT;
		}

		strcat(my_query, " rockethits = rockethits + '1'");
	}
	else if (arena->munition[munition].type == 3) // bomb
	{
		if (client->infly)
		{
			if (penalty)
				client->score.groundscore += (SCORE_BOMBHIT * -20);
			else
				client->score.groundscore += SCORE_BOMBHIT;
		}

		strcat(my_query, " bombhits = bombhits + '1'");
	}
	else if (arena->munition[munition].type == 4) // torpedo
	{
		if (client->infly)
		{
			if (penalty)
				client->score.groundscore += (SCORE_TORPEDOHIT * -20);
			else
				client->score.groundscore += SCORE_TORPEDOHIT;
		}

		strcat(my_query, " torphits = torphits + '1'");
	}
	else if (arena->munition[munition].type == 1) // bullet
	{
		if (client->infly)
		{
			if (penalty)
				client->score.groundscore += (SCORE_BULLETHIT * -20);
			else
				client->score.groundscore += SCORE_BULLETHIT;
		}

		strcat(my_query, " gunhits = gunhits + '1'");
	}

	sprintf(my_query, "%s WHERE player_id = '%u'", my_query, client->id);

	if (d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		Com_Printf("WARNING: HardHit(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}
