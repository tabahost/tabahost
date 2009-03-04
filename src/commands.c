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
 Cmd_LoadBatch

 Executes a script file as a personal Batch File (same as Cmd_LoadConfig)
 *************/

void Cmd_LoadBatch(client_t *client)
{
	FILE *fp;
	char file[128];
	char command[1024];

	sprintf(file, "./batch/%s.cfg", client->longnick);

	if ((fp = fopen(file, "r")) == NULL)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "WARNING: Cannot execute file \"%s\"", file);
		Com_Printf(VERBOSE_WARNING, "Couldn't execute file \"%s\"\n", file);
	}
	else
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Executing batch file");

		while (fgets(command, sizeof(command), fp) != NULL)
		{
			ProcessCommands(command, client);
		}

		fclose(fp);
	}
}

/*************
 Cmd_LoadConfig

 Executes a script file
 *************/

void Cmd_LoadConfig(char *filename, client_t *client)
{
	FILE *fp;
	char file[128];
	char command[1024];

	strcpy(file, filename);
	strcat(file, ".cfg");

	if ((fp = fopen(file, "r")) == NULL)
	{
		if (client)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "WARNING: Cannot execute file \"%s\"", file);
		}

		Com_Printf(VERBOSE_WARNING, "Couldn't execute file \"%s\"\n", file);
	}
	else
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Executing config file \"%s\"", file);

		while (fgets(command, sizeof(command), fp) != NULL)
		{
			ProcessCommands(command, NULL);
		}

		fclose(fp);
	}

}

/*************
 Cmd_Ros

 Print who is online
 *************/

void Cmd_Ros(client_t *client)
{
	u_int8_t red, green, gold, purp, i;

	if (client)
	{
		if (!client->infly)
		{
			SendFileSeq1(FILE_INGAME, "roster.asc", client);
		}
	}
	else
		Sys_Printfile(FILE_INGAME);

	for (red = green = gold = purp = i = 0; i < maxentities->value; i++)
	{
		if (clients[i].attr == 1 && hideadmin->value)
			continue;

		if (clients[i].inuse && !clients[i].drone)
		{
			switch (clients[i].country)
			{
				case 1:
					red++;
					break;
				case 2:
					green++;
					break;
				case 3:
					gold++;
					break;
				case 4:
					purp++;
					break;
			}
		}
	}

	arena->numplayers = red + green + gold + purp;

	PPrintf(client, RADIO_LIGHTYELLOW, "Reds: %d, Greens: %d, Golds: %d, Purps: %d", red, green, gold, purp);
}

/*************
 Cmd_Ammo

 Print ammo configuration
 *************/

void Cmd_Ammo(client_t *client, u_int8_t arg, char *arg2)
{
	u_int8_t i;

	if (!arg2)
	{
		for (i = 0; i < maxmuntype; i++)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "%d-%s, he %d, ap %d, decay %d, type %d", i, arena->munition[i].name, arena->munition[i].he, arena->munition[i].ap, arena->munition[i].decay,
					arena->munition[i].type);
		}
	}
	else
	{
		if (*arg2 == 'h')
		{
			arg2++;
			arena->munition[arg].he = Com_Atoi(arg2);
			PPrintf(client, RADIO_LIGHTYELLOW, "HE of %s(%u) was changed to %d", arena->munition[arg].name, arg, arena->munition[arg].he);
			Com_Printf(VERBOSE_ATTENTION, "HE of %s(%u) was changed to %u by %s\n", arena->munition[arg].name, arg, arena->munition[arg].he, client ? client->longnick : "-HOST-");
		}
		else if (*arg2 == 'a')
		{
			arg2++;
			arena->munition[arg].ap = Com_Atoi(arg2);
			PPrintf(client, RADIO_LIGHTYELLOW, "AP of %s(%u) was changed to %u", arena->munition[arg].name, arg, arena->munition[arg].ap);
			Com_Printf(VERBOSE_ATTENTION, "AP of %s(%u) was changed to %u by %s\n", arena->munition[arg].name, arg, arena->munition[arg].ap, client ? client->longnick : "-HOST-");
		}
		else if (*arg2 == 'd')
		{
			arg2++;
			arena->munition[arg].decay = Com_Atoi(arg2);
			PPrintf(client, RADIO_LIGHTYELLOW, "Decay of %s(%u) was changed to %u", arena->munition[arg].name, arg, arena->munition[arg].decay);
			Com_Printf(VERBOSE_ATTENTION, "Decay of %s(%u) was changed to %d by %s\n", arena->munition[arg].name, arg, arena->munition[arg].decay, client ? client->longnick : "-HOST-");
		}
		else if (*arg2 == 't')
		{
			arg2++;
			arena->munition[arg].decay = Com_Atoi(arg2);
			PPrintf(client, RADIO_LIGHTYELLOW, "Type of %s(%u) was changed to %u", arena->munition[arg].name, arg, arena->munition[arg].type);
			Com_Printf(VERBOSE_ATTENTION, "Type of %s(%u) was changed to %d by %s\n", arena->munition[arg].name, arg, arena->munition[arg].type, client ? client->longnick : "-HOST-");
		}
		else
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Invalid argument for ammo '<h|a|d|t[number]>'");
			return;
		}
	}
}

void Cmd_Saveammo(client_t *client, char *row) // query time average 1.7sec
{
	int16_t i = 0;
	u_int16_t time;

	if (!row)
	{
		my_query[0] = 0;

		time = Sys_Milliseconds();

		if (mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_ON))
			Com_Printf(VERBOSE_ERROR, "%d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

		for (i = 0; i < maxmuntype; i++)
		{
			sprintf(my_query, "%sUPDATE munitions SET he = '%d', ap = '%u', decay = '%d', name = '%s', abbrev = '%s', muntype = '%d' WHERE id = '%u' LIMIT 1", my_query, arena->munition[i].he,
					arena->munition[i].ap, arena->munition[i].decay, arena->munition[i].name, arena->munition[i].abbrev, arena->munition[i].type, i);

			if ((i && !(i % 30)) || i == (maxmuntype - 1))
			{
				if (d_mysql_query(&my_sock, my_query))
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Saveammo(): couldn't query UPDATE id %d error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
					// TODO: FIXME: add sleep here?
					mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
					return;
				}
				else
				{
					Com_MySQL_Flush(&my_sock,__FILE__ , __LINE__);
				}

				my_query[0] = '\0';

				// FIXME MySQL, sleep() for windows
#ifdef  _WIN32
				sleep(140);
#else			
				usleep(140000);
#endif
			}
			else
			{
				strcat(my_query, "; ");
			}
		}

		mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);

		time = Sys_Milliseconds() - time;
		Com_Printf(VERBOSE_DEBUG, "SaveAmmo(): Query Time %.3fsec\n", (float)(time)/1000);
		PPrintf(client, RADIO_YELLOW, "Query Time %.3fsec\n", (float)(time)/1000);
	}
	else
	{
		i = Com_Atoi(row);

		if(i >= 0 && i < maxmuntype)
		{
			sprintf(my_query, "UPDATE munitions SET he = '%d', ap = '%u', decay = '%d', name = '%s', abbrev = '%s', muntype = '%d' WHERE id = '%u' LIMIT 1",
			arena->munition[i].he,
			arena->munition[i].ap,
			arena->munition[i].decay,
			arena->munition[i].name,
			arena->munition[i].abbrev,
			arena->munition[i].type,
			i);

			if(Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded

			{
				if(mysql_errno(&my_sock))
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Saveammo(row): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Invalid ammo type %d", i);
		}
	}
}

/*************
 Cmd_VarList

 Print all variables
 *************/

void Cmd_VarList(client_t *client, char *string)
{
	var_t *var;
	u_int8_t i = 0;

	for (var = var_vars; var; var = var->next)
	{
		if(client && (var->flags & VAR_ADMIN) && client->attr == FLAG_OP)
			continue;

		if (!string)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "%s %s \"%s\"", var->flags & VAR_ARCHIVE ? var->flags & VAR_NOSET ? "*" : "+" : var->flags & VAR_NOSET ? "x" : " ", var->name, var->string);
			i++;
		}
		else
		{
			if (!Com_Strncmp(var->name, string, strlen(string)))
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "%s %s \"%s\"", var->flags & VAR_ARCHIVE ? var->flags & VAR_NOSET ? "*" : "+" : var->flags & VAR_NOSET ? "x" : " ", var->name, var->string);
				i++;
			}
		}
	}
}

/*************
 Cmd_Move

 Move to a field or change country
 *************/

void Cmd_Move(char *field, int country, client_t *client)
{
	u_int8_t buffer[10];
	u_int8_t nothq = 0x20;
	u_int8_t i;
	int8_t fieldn = 0;
	move_t *move;

	memset(buffer, 0, sizeof(buffer));
	move = (move_t *) buffer;

	if (field)
	{
		client->hq = 0;

		if (tolower(*field) == 'f')
		{
			field++;
		}

		if (!Com_Stricmp(field, "hq"))
		{
			nothq = 0;
			client->field = 1;
			client->hq = 1;
			PPrintf(client, RADIO_YELLOW, "You moved to HQ");
		}
		else
		{
			fieldn = Com_Atoi(field)-1;

			if (fieldn >= fields->value || fieldn < 0)
			{
				PPrintf(client, RADIO_YELLOW, "Invalid field");
				return;
			}

			if (arena->fields[fieldn].country == client->country || client->attr == 1)
			{
				if (!arena->fields[fieldn].closed || client->attr == 1)
				{
					client->field = fieldn + 1;
					PPrintf(client, RADIO_YELLOW, "You moved to f%s", field);
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "That airfield is closed");
					return;
				}
			}
			else
			{
				if ((arena->fields[fieldn].type <= FIELD_SUBMARINE) || (arena->fields[fieldn].type >= FIELD_WB3POST))
					PPrintf(client, RADIO_YELLOW, "This field is not from your country");
				return;
			}
		}
	}
	else // country
	{
		client->hq = 0;

		if (!client->arenabuildsok || (client->countrytime <= time(NULL)) || client->attr == 1)
		{
			if (((country != 1) && (country != 3)) && client->attr != 1)
			{
				PPrintf(client, RADIO_YELLOW, "Invalid country");
				return;
			}

			for (i = 0; i < maxentities->value; i++) // check for IP sharing
			{
				if (clients[i].inuse && !clients[i].drone && clients[i].ready && &clients[i] != client)
				{
					if (clients[i].country != country && !strcmp(clients[i].ip, client->ip) && !clients[i].thai)
					{
						country = clients[i].country;
						PPrintf(client, RADIO_YELLOW, "Country forced to %s due IP Sharing with %s", GetCountry(country), clients[i].longnick);
						break;
					}
				}
			}

			client->country = country;
			fieldn = FirstFieldCountry(country);
			if (fieldn < 0)
			{
				nothq = 0;
				client->field = 1;
				client->hq = 1;
				fieldn = 0;
			}
			else
				client->field = fieldn+1;
			BPrintf(RADIO_YELLOW, "%s joined the %s side", client->longnick, GetCountry(country)); // Country changed to %s

			if (client->arenafieldsok && client->arenabuildsok)
			{
				if (client->hq)
					client->countrytime = 0; // give a chance to player change country again if chosen wrong (country with no field)
				else
					client->countrytime = time(NULL) + countrytime->value;
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Can't change country now, wait for %s", Com_TimeSeconds(client->countrytime - time(NULL)));
			return;
		}
	}

	move->packetid = htons(Com_WBhton(0x0403));
	move->country = htonl(country);
	move->options = nothq; // 00 to HQ
	move->field = fieldn;

	client->posxy[0][0] = arena->fields[fieldn].posxyz[0];
	client->posxy[1][0] = arena->fields[fieldn].posxyz[1];

	UpdateIngameClients(0);

	SendPacket(buffer, sizeof(buffer), client);
	SendArenaRules(client);
	WB3SendGruntConfig(client);
	//	WB3SendArenaFlags3(client);
	WB3ArenaConfig2(client);
	WB3SendAcks(client);

	SendRPS(client);
}

/*************
 Cmd_Plane

 Change current player`s aircraft
 *************/

void Cmd_Plane(u_int16_t planenumber, client_t *client)
{
	u_int32_t rpsreplace;
	char message[64];
	u_int8_t buffer[10];
	plane_t *plane;

	memset(buffer, 0, sizeof(buffer));
	memset(message, 0, sizeof(message));
	plane = (plane_t *) buffer;

	if (client->field > fields->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid Field F%u", client->field);
	}

	if (planenumber)
	{
		if (planenumber < 999)
		{
			if (!GetPlaneName(planenumber) || planenumber == PLANE_FAU)
			{
				PPrintf(client, RADIO_YELLOW, "Invalid plane");
				return;
			}

			if (client->plane != planenumber)
			{
				client->ord = 0;
				if (arcade->value)
					sprintf(message, "You selected %s (N%d)", GetPlaneName(planenumber), planenumber);
				else
					sprintf(message, "You selected %s (N%d), %d left", GetPlaneName(planenumber), planenumber, (int16_t)arena->fields[client->field - 1].rps[planenumber]);
			}

			client->plane = planenumber;
		}
		else
		{
			if (arcade->value)
				sprintf(message, "Your plane is %s (N%d)", GetPlaneName(client->plane), client->plane);
			else
				sprintf(message, "Your plane is %s (N%d), %d left", GetPlaneName(client->plane), client->plane, (int16_t)arena->fields[client->field - 1].rps[client->plane]);
		}

//		if (rps->value && strlen(message) && !arcade->value)
//		{
//			rpsreplace = (u_int32_t)(rps->value * 6000) - (arena->frame % (u_int32_t)(rps->value * 6000));
//			sprintf(message, "%s (%s to replace)", message, Com_TimeSeconds(rpsreplace/100));
//		}
		
		if (rps->value && strlen(message) && !arcade->value)
		{
			rpsreplace = (rps->value * 6000) / arena->rps[client->plane].pool[arena->fields[client->field - 1].type - 1];

			if(!((arena->fields[client->field - 1].country == 1 && arena->rps[client->plane].country & 0x01) ||
				(arena->fields[client->field - 1].country == 2 && arena->rps[client->plane].country & 0x02) ||
				(arena->fields[client->field - 1].country == 3 && arena->rps[client->plane].country & 0x04) ||
				(arena->fields[client->field - 1].country == 4 && arena->rps[client->plane].country & 0x08)))
			{
				rpsreplace = 0 ;
			}

			if(rpsreplace)
			{
				rpsreplace -= (arena->frame % rpsreplace);

				sprintf(message, "%s (%s to replace)", message, Com_TimeSeconds(rpsreplace/100));
			}
			else
			{
				sprintf(message, "%s (no replacement)", message);
			}
		}

		if (strlen(message))
			PPrintf(client, RADIO_YELLOW, "%s", message);
	}

	plane->packetid = htons(Com_WBhton(0x0415));
	plane->plane = htons(client->plane);
	plane->conv = htons(client->conv*3);
	plane->fuel = htons(client->fuel);
	plane->ord = htons(client->ord);

	SendPacket(buffer, sizeof(buffer), client);
}

/*************
 Cmd_Fuel

 Set fuel load
 *************/

void Cmd_Fuel(int8_t fuel, client_t *client)
{
	if (client->fuel != fuel)
	{
		if (fuel < 0)
			fuel = 0;
		if (fuel > 100)
			fuel = 100;

		client->fuel = fuel;

		Cmd_Plane(0, client);

		PPrintf(client, RADIO_YELLOW, "Fuel set to %d", fuel);
	}
}

/*************
 Cmd_Conv

 Set convergency distance
 *************/

void Cmd_Conv(u_int16_t distance, client_t *client)
{
	if (client->conv != distance)
	{
		if (distance < 100)
			distance = 100;
		if (distance > 999)
			distance = 999;

		client->conv = distance;
		Cmd_Plane(0, client);
		PPrintf(client, RADIO_YELLOW, "Convergency set to %d", distance);
	}
}

/*************
 Cmd_Ord

 Set ordinance
 *************/

void Cmd_Ord(u_int8_t ord, client_t *client)
{
	if (client->ord != ord)
	{
		client->ord = ord;
		Cmd_Plane(0, client);
		PPrintf(client, RADIO_YELLOW, "Ordnance set to %d", ord);
	}
}

/*************
 Cmd_Easy

 Enable/Disable Easy mode
 *************/

void Cmd_Easy(u_int8_t easy, client_t *client)
{

	if (easymode->value)
	{
		if (easymode->value > 1)
			client->easymode = easy = 1;
		else
			client->easymode = easy;
	}
	else
		client->easymode = easy = 0;

	if (easy)
		PPrintf(client, RADIO_YELLOW, "Easy flight mode enabled");
	else
		PPrintf(client, RADIO_YELLOW, "Easy flight mode disabled");
}

/*************
 Cmd_TK

 Set player as TK or removes it
 *************/

void Cmd_TK(char *tkiller, u_int8_t newvalue, client_t *client) // twin of Cmd_Ban
{
	client_t *ctk;
	u_int32_t id = 0;
	u_int8_t oldvalue;
	int16_t num_rows;
	u_int16_t i;

	if (!(ctk = FindLClient(tkiller)))
	{
		sprintf(my_query, "SELECT id, teamkiller FROM players WHERE longnick = '%s'", tkiller);

		if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
		{
			if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
			{
				if ((my_row = mysql_fetch_row(my_result)))
				{
					id = Com_Atou(Com_MyRow("id"));
					oldvalue = Com_Atou(Com_MyRow("teamkiller"));
				}
				else
				{
					if (!mysql_errno(&my_sock))
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Player %s not found", tkiller);
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "Cmd_TK(find): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						PPrintf(client, RADIO_YELLOW, "Cmd_TK(find): Couldn't Fetch Row, please contact admin");
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
					return;
				}

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_TK(find): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				return;
			}
		}
		else
		{
			if (mysql_errno(&my_sock))
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_TK(find): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
			return;
		}
	}
	else
	{
		oldvalue = ctk->tkstatus;
		id = ctk->id;
	}

	if (id)
	{
		if (oldvalue == newvalue)
		{
			if (oldvalue)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Player %s is already marked as a team killer", tkiller);
			}
			else
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Player %s is not a team killer yet", tkiller);
			}

			return;
		}
		else
		{
			if (!ctk && client) // clear SQL flood check if player id was get from DB
			{
				client->lastsql = 0;
			}

			sprintf(my_query, "UPDATE players SET teamkiller = '%u' WHERE id = '%u'", newvalue, id);

			if (!Com_MySQL_Query(client, &my_sock, my_query))
			{
				if (newvalue)
				{
					if (ctk && !client)
						BPrintf(RADIO_LIGHTYELLOW, "*** %s became a Team Killer!!! (%d friends killed) ***", ctk->longnick, ctk->tklimit);
					else
						BPrintf(RADIO_LIGHTYELLOW, "%s was set as TK by %s", tkiller, client?client->longnick:"-HOST-");

					Com_Printf(VERBOSE_ATTENTION, "%s was set as TK by %s\n", tkiller, client ? client->longnick : "-HOST-");
				}
				else
				{
					if (ctk)
						PPrintf(ctk, RADIO_YELLOW, "*** Congratulations, your TK status was removed by %s***", client ? client->longnick : "-HOST-");
					else
						PPrintf(client, RADIO_YELLOW, "You removed TK status from %s", tkiller);
					Com_Printf(VERBOSE_ATTENTION, "%s has removed TK from %s\n", client ? client->longnick : "-HOST-", tkiller);
				}

				if (ctk)
				{
					ctk->tkstatus = newvalue;
					ctk->wings = 0;
					ctk->tklimit = 0;
				}
				/*
				 RemoveClient(ctk);
				 */
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_TK(TKid): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			if (wb3->value)
				sprintf(
						my_query,
						"SELECT ipaddress.id FROM players, players_ipaddress, ipaddress WHERE players.id = '%u' AND players.id = players_ipaddress.player_id AND ipaddress.id = players_ipaddress.ipaddress_id",
						id);
			else
				sprintf(
						my_query,
						"SELECT hdserials.id FROM players, players_hdserials, hdserials WHERE players.id = '%u' AND players.id = players_hdserials.player_id AND hdserials.id = players_hdserials.hdserial_id",
						id);

			if (!d_mysql_query(&my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((num_rows = mysql_num_rows(my_result)) > 0)
					{
						if (wb3->value)
							sprintf(my_query, "UPDATE ipaddress SET teamkiller='%u' WHERE id IN(", newvalue);
						else
							sprintf(my_query, "UPDATE hdserials SET teamkiller='%u' WHERE id IN(", newvalue);

						for (i = 0; i < num_rows; i++)
						{
							if ((my_row = mysql_fetch_row(my_result)))
							{
								sprintf(my_query, "%s'%u'", my_query, Com_Atou(Com_MyRow("id")));
							}
							else
							{
								mysql_free_result(my_result);
								my_result = NULL;
								Com_Printf(VERBOSE_WARNING, "Cmd_TK(TKhd): Couldn't Fetch Row %d, error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
								break;
							}

							if (i == (num_rows - 1))
							{
								strcat(my_query, ")");

								if (d_mysql_query(&my_sock, my_query))
								{
									Com_Printf(VERBOSE_WARNING, "Cmd_TK(TKhd): Couldn't query UPDATE id %d error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
									return;
								}
							}
							else
							{
								strcat(my_query, ",");
							}
						}
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "Cmd_TK(TKhd): num_rows = %u\n", num_rows);
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_TK(TKhd): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_TK(TKhd): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Cmd_TK() id == NULL\n");
	}
}

/*************
 Cmd_Fly

 Process the init of flight of a client
 *************/

u_int8_t Cmd_Fly(u_int16_t position, client_t *client)
{
	u_int16_t i = 0, j = 0;
	u_int8_t buffer[103];
	u_int32_t rules = 0;
	initflight_t *fly;
	wb3initflight_t *wb3fly;
	initflight2_t *fly2;

	memset(buffer, 0, sizeof(buffer));

	if (wb3->value)
	{
		wb3fly = (wb3initflight_t *)buffer;
	}
	else
	{
		fly = (initflight_t *)buffer;
	}

	wb3fly->packetid = htons(Com_WBhton(0x0401));

	if (!(client->attr & (FLAG_ADMIN | FLAG_OP)))
	{
		if (!allowtakeoff->value)
		{
			PPrintf(client, RADIO_YELLOW, "Takeoff was blocked by administrators");

			if (wb3->value)
			{
				wb3fly->error = htons(0x09);
				SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
			}
			return 1;
		}
		else if (!client->lives)
		{
			PPrintf(client, RADIO_YELLOW, "You are killed and cannot fly");
			if (wb3->value)
			{
				wb3fly->error = htons(0x03);
				SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
			}
			return 1;
		}
	}

	if (client->tkstatus && !IsBomber(client))
	{
		PPrintf(client, RADIO_YELLOW, "You are a team killer, you must fly bomber");
		if (wb3->value)
		{
			wb3fly->error = htons(0x07);
			SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
		}
		return 1;
	}

	if (client->field > fields->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid Field F%u", client->field);
		if (wb3->value)
		{
			wb3fly->error = htons(0x02);
			SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
		}
		return 1;
	}

	if ((arena->fields[client->field-1].country != client->country) && (client->attr != 1))
	{
		PPrintf(client, RADIO_YELLOW, "This field is not from your country");
		if (wb3->value)
		{
			wb3fly->error = htons(0x02);
			SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
		}
		return 1;
	}

	if (arena->fields[client->field-1].closed && client->attr != 1)
	{
		PPrintf(client, RADIO_YELLOW, "You cannot take off - This field is closed");
		if (wb3->value)
		{
			wb3fly->error = htons(0x08);
			SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
		}
		return 1;
	}

	if (!GetPlaneName(client->plane) || client->plane == PLANE_FAU)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid plane");
		if (wb3->value)
		{
			wb3fly->error = htons(0x07);
			SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
		}
		return 1;
	}

	if (position != 100)
	{
		if ((arena->fields[client->field - 1].rps[client->plane] > -1 && arena->fields[client->field - 1].rps[client->plane] < 1) && !client->attached && client->attr != 1)
		{
			PPrintf(client, RADIO_YELLOW, "Plane not available");
			if (wb3->value)
			{
				wb3fly->error = htons(0x07);
				SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
			}
			return 1;
		}
	}

	if (client->flypenalty && client->flypenaltyfield == client->field && !client->attached && client->attr != 1)
	{
		PPrintf(client, RADIO_YELLOW, "You can't take off from this field for %s", Com_TimeSeconds(client->flypenalty / 100));
		if (wb3->value)
		{
			wb3fly->error = htons(0x09);
			SendPacket(buffer, (wb3->value ? 69 : 67) + 12, client);
		}
		return 1;
	}

	if (client->attr & FLAG_ADMIN)
	{
		rules = (FLAG_AIRSHOWSMOKE | FLAG_ENEMYNAMES | FLAG_ENEMYPLANES);
	}
	else
	{
		if (midairs->value)
			rules |= FLAG_MIDAIRS;
		if (blackout->value)
			rules |= FLAG_BLACKOUT;
		if (airshowsmoke->value)
			rules |= FLAG_AIRSHOWSMOKE;
		if (client->easymode)
			rules |= FLAG_EASYMODE;
		if (client->easymode > 1)
			rules |= FLAG_EASYMODE2;
		if (enemynames->value)
			rules |= FLAG_ENEMYNAMES;
		if (enemyplanes->value)
			rules |= FLAG_ENEMYPLANES;
		if (enableottos->value)
			rules |= FLAG_ENABLEOTTOS;
		//		if(dpitch->value) // DEBUG
		//			rules |= FLAG_UNK1;
		//		if(droll->value) // DEBUG
		//			rules |= FLAG_UNK2;
	}

	if (!wb3->value)
	{
		if (client->attached)
		{
			fly->fuel = htonl(50);
		}
		else
		{
			if (IsFighter(client))
			{
				if (arena->fields[client->field - 1].type == FIELD_CV || arena->fields[client->field - 1].type == FIELD_CARGO || arena->fields[client->field - 1].type == FIELD_DD
						|| arena->fields[client->field - 1].type == FIELD_SUBMARINE)
				{
					i = 80;
				}
				else
				{
					for (i = 0, j = 0; i < MAX_BUILDINGS; i++)
					{
						if (!arena->fields[client->field - 1].buildings[i].field)
							break;

						if (arena->fields[client->field - 1].buildings[i].type == BUILD_FUEL && !arena->fields[client->field - 1].buildings[i].status)
							j++;
					}

					i = (30 * j) + 30;
				}

				if ((u_int8_t)client->fuel <= i || (u_int8_t)client->attr == 1)
				{
					fly->fuel = htonl(client->fuel);
				}
				else
				{
					PPrintf(client, RADIO_DARKGREEN, "Fuel limited to %u%%", i);

					fly->fuel = htonl(i);
				}
			}
			else
			{
				fly->fuel = htonl(client->fuel);
			}
		}
	}

	if (client->attached)
	{
		if (client->attached->drone & (DRONE_HMACK | DRONE_HTANK))
		{
			if (wb3->value)
			{
				wb3fly->alt = htonl(arena->fields[client->field - 1].posxyz[2]);
			}
			else
			{
				fly->alt = htonl(arena->fields[client->field - 1].posxyz[2]);
			}
		}

		if (i == MAX_RELATED)
		{
			if (wb3->value)
			{
				wb3fly->alt = htonl(0x0102);
			}
			else
			{
				fly->alt = htonl(0x0102);
			}
		}
	}
	else if ((arena->fields[client->field - 1].type == FIELD_CARGO || arena->fields[client->field - 1].type == FIELD_SUBMARINE) && !startalt->value)
	{
		if (wb3->value)
		{
			wb3fly->alt = htonl(30);
		}
		else
		{
			fly->alt = htonl(30);
		}
	}
	else if (arena->fields[client->field - 1].type == FIELD_DD && !startalt->value)
	{
		if (wb3->value)
		{
			wb3fly->alt = htonl(70);
		}
		else
		{
			fly->alt = htonl(70);
		}
	}
	else
	{
		if (wb3->value)
		{
			wb3fly->alt = htonl(startalt->value);
		}
		else
		{
			fly->alt = htonl(startalt->value);
		}
	}

	if (!wb3->value)
	{
		if (arena->fields[client->field - 1].type == FIELD_CV || arena->fields[client->field - 1].type == FIELD_CARGO || arena->fields[client->field - 1].type == FIELD_DD
				|| arena->fields[client->field - 1].type == FIELD_SUBMARINE || client->attached)
		{
			fly->runway = 0;
		}
		else
		{
			fly->runway = htonl((rand()%3)+1);
		}
	}

	if (wb3->value)
	{
		strcpy(client->skin, CreateSkin(client, 1));
		
		wb3fly->country = htonl(client->country);
		wb3fly->field = htons(client->field - 1);
		wb3fly->bulletradius1 = htons((u_int16_t)(bulletradius->value * 10));
		wb3fly->bulletradius2 = htons((u_int16_t)(bulletradius->value * 10));
		wb3fly->gunrad = htons((u_int16_t)(gunrad->value * 10));
		wb3fly->bulletradius3 = htons((u_int16_t)(bulletradius->value * 10));
		wb3fly->unknown10 = htonl(100); // same value from wb3requestfly_t unk4 (100)
		wb3fly->unknown11 = htonl(900); // same value from wb3requestfly_t unk5 (900)
		wb3fly->rules = htonl(rules);
		wb3fly->attachedpos = htonl(position);
		wb3fly->numofarrays = 1;
	}
	else
	{
		fly->conv = htonl(client->conv * 3);
		fly->country = htonl(client->country);
		fly->field = htons(client->field - 1);
		fly->bulletradius1 = htons((u_int16_t)(bulletradius->value * 10));
		fly->bulletradius2 = htons((u_int16_t)(bulletradius->value * 10));
		fly->gunrad = htons((u_int16_t)(gunrad->value * 10));
		fly->bulletradius3 = htons((u_int16_t)(bulletradius->value * 10));
		fly->unknown6 = htons(0x01); // 0x01
		fly->unknown7 = htons(0x01); // 0x01
		fly->unknown8 = htons(0x0a); // 0x0a
		fly->unknown9 = 1; // 0x100
		fly->rules = htonl(rules);
		fly->attachedpos = htonl(position);
		fly->numofarrays = 1;
	}

	if (client->attached)
	{
		if (wb3->value)
		{
			wb3fly->plane = htons(client->attached->plane);
			j = wb3fly->numofarrays;
		}
		else
		{
			fly->plane = htons(client->attached->plane);
			fly->attached = htonl(2);
			j = fly->numofarrays;
		}

		for (i = 0; i < j; i++)
		{
			fly2 = (initflight2_t *)(buffer + (wb3->value ? 69 : 67) + (12 * i));
			fly2->ord = 0;
		}
	}
	else
	{
		if (wb3->value)
		{
			wb3fly->plane = htons(client->plane);

			j = wb3fly->numofarrays;
		}
		else
		{
			fly->plane = htons(client->plane);
			j = fly->numofarrays;
		}

		for (i = 0; i < j; i++)
		{
			fly2 = (initflight2_t *)(buffer + (wb3->value ? 69 : 67) + (12 * i));
			fly2->ord = client->ord;
		}
	}

	client->dronetimer = arena->time; // stores time when client started flight
	client->damaged = 0;

	client->infly = 1;
	if (arcade->value)
	{
		client->commandos = 1;
	}
	else
	{
		client->commandos = 4;
	}

	client->status_damage = 0; // to force player not appear damaged after takeoff
	client->chute = 0; // to assure client is not flagged as chute

	sprintf(client->logfile, "%s,%s,%s,%s,%u", mapname->string, client->longnick, GetSmallPlaneName(client->plane), GetCountry(client->country), (u_int32_t)time(NULL));

	client->obradar = 0;

	if (arena->year >= 1944)
	{
		if (wb3->value)
		{
			switch (client->plane)
			{
				case 1: /*f6f5*/
				case 4: /*f4u1d*/
				case 9: /*ki84ia*/
				case 16: /*110g2*/
				case 19: /*190d9*/
				case 27: /*p38l*/
				case 35: /*ju88a4*/
				case 37: /*b25j*/
				case 38: /*b17g*/
				case 48: /*me262*/
				case 52: /*mosq6*/
				case 57: /*ju87g*/
				case 59: /*b24j*/
				case 71: /*f86*/
				case 82: /*mosqnf2*/
				case 88: /*g4m1*/
				case 101: /*he111h3*/
				case 113: /*do17z*/
				case 114: /*109g2eto*/
				case 125: /*ki44iib*/
					client->obradar = (int32_t)obradar->value;
					break;
				default:
					client->obradar = 0;
			}
		}
		else
		{
			switch (client->plane)
			{
				case 16:
				case 26:
				case 27:
				case 37:
				case 38:
				case 48:
				case 52:
				case 53:
				case 57:
				case 59:
				case 67:
				case 91:
				case 95:
				case 100:
					client->obradar = (int32_t)obradar->value;
					break;
				default:
					client->obradar = 0;
			}
		}
	}

	if (!client->attached)
	{
		if (IsCargo(client))
			client->mortars = mortars->value;

		Cmd_Plane(client->plane, client);

		SendArenaRules(client);
		WB3SendGruntConfig(client);
		//	WB3SendArenaFlags3(client);
		WB3ArenaConfig2(client);
	}

	SendPacket(buffer, (wb3->value ? 69 : 67) + (12 * j /*num of arrays*/), client);

	if (client->lives > 0)
	{
		PPrintf(client, RADIO_YELLOW, "%d li%s left", client->lives, client->lives == 1 ? "fe" : "ves");
	}

	if (!(client->attached && client->attached->drone & DRONE_FAU))
	{
		if (position != 100)
		{
			Com_Printf(VERBOSE_ALWAYS, "%s takeoff from f%d with plane %s ord %d country %s\n", client->longnick, client->field, GetSmallPlaneName(client->plane), client->ord, GetCountry(client->country));

			Com_LogEvent(EVENT_TAKEOFF, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
			Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
			Com_LogDescription(EVENT_DESC_PLORD, client->ord, NULL);
			Com_LogDescription(EVENT_DESC_FIELD, client->field, NULL);

			ScoresEvent(SCORE_TAKEOFF, client, 0);
		}
	}

	client->lastscore = 0; // TODO: score, position fix?

	if (position != 100)
	{
		if (arena->fields[client->field - 1].rps[client->plane] != -1)
		{
			if ((client->plane < maxplanes) && !client->tkstatus)
				arena->fields[client->field - 1].rps[client->plane]--;
		}
	}

	ClearMapDots(client);

	SetPlaneDamage(client->plane, client);

	UpdateIngameClients(0);

	SendOttoParams(client);
	//if (!wb3->value)
		SendExecutablesCheck(2, client);

	if (!client->attached && client->wings && IsBomber(client))
		DroneWings(client);

	return 0;
}

/*************
 Cmd_Capt

 Capture a Field
 *************/

u_int8_t Cmd_Capt(u_int16_t field, u_int8_t country, client_t *client) // field is == Fx -1
{
	u_int8_t buffer[8];
	u_int16_t i, j, k;
	captfield_t *capt;

	if (field >= (fields->value + cities->value))
	{
		PPrintf(client, RADIO_YELLOW, "Invalid field/city");
		return 0;
	}

	if (field >= fields->value) // capture just a city
	{
		if (field < (fields->value + cities->value))
		{
			field -= (u_int16_t)fields->value;
			if (arena->cities[field].field)
			{
				PPrintf(client, RADIO_YELLOW, "You need to capture F%d to capture this city", arena->cities[field].field->number);
			}
			else
			{
				arena->cities[field].country = country;

				for (i = 0; i < MAX_BUILDINGS; i++)
				{
					if (arena->cities[field].buildings[i].field)
					{
						arena->cities[field].buildings[i].country = country;
					}
					else
						break;
				}

				SendFieldStatus((arena->cities[field].number - 1) + fields->value, NULL);
			}
		}

		return 0;
	}

	if (arena->fields[field].type >= FIELD_CV && arena->fields[field].type <= FIELD_SUBMARINE)
	{
		//~ if(!client) // a real capture
		//~ {
		//~ sprintf(buffer, "f%d", field+1);
		//~ Cmd_Seta(buffer, 0, -1, 0); // remove all planes from a sunk CV
		//~ }

		if (!client && cvcapture->value) // makes cv be captured to enemy if friendly killed
		{
			if (arena->fields[field].country == country)
			{
				if (country == COUNTRY_GOLD)
					country = COUNTRY_RED;
				else
					country = COUNTRY_GOLD;
			}
		}
	}

	memset(buffer, 0, sizeof(buffer));

	capt = (captfield_t *)buffer;

	capt->packetid = htons(Com_WBhton(0x0306));
	capt->field = htons(field);
	capt->country = htonl(country);

	if (client)
	{
		SendPacket(buffer, sizeof(buffer), client);
	}
	else
	{
		if (arena->fields[field].country != country)
		{
			arena->fields[field].ctf = country;
			arena->fields[field].ctfcount = arena->frame;
			
			BPrintf(RADIO_YELLOW, "System: FIELDF%d has been captured by the %s", field+1, GetCountry(country));

			for (/*j = */i = 0; i < MAX_CITYFIELD; i++) // Capture field and it captures the cities
			{
				if (arena->fields[field].city[i])
				{
					if (arena->fields[field].city[i]->country > 0 && arena->fields[field].city[i]->country <= 4)
						DebiteFactoryBuildings(arena->fields[field].city[i]);

					arena->fields[field].city[i]->country = country;

					for (k = 0; k < MAX_BUILDINGS; k++)
					{
						if (arena->fields[field].city[i]->buildings[k].field)
						{
							arena->fields[field].city[i]->buildings[k].country = country;
						}
						else
							break;
					}

					SendFieldStatus((arena->fields[field].city[i]->number - 1) + fields->value, NULL);

					if (country > 0 && country <= 4)
						CrediteFactoryBuildings(arena->fields[field].city[i]);

					//					if(arena->fields[field].city[i]->needtoclose)
					//						j = 1;
				}
			}

			//			if(!j)
			//				arena->fields[field].abletocapture = 1;

			// check end of war (arena reset)
			if (canreset->value && arena->fields[field].country /* to avoid reset by capt neutrals */)
			{
				for (j = i = 0; i < fields->value; i++)
				{
					if (killcvtoreset->value || ((arena->fields[i].type <= FIELD_MAIN) || (arena->fields[i].type >= FIELD_WB3POST)))
					{
						if (arena->fields[i].country == arena->fields[field].country)
							j++;
					}

					if (j > 2)
						break;
				}

				if (j <= 2)
				{
					BPrintf(RADIO_YELLOW, "*********************************");
					BPrintf(RADIO_YELLOW, "*********    THE WAR    *********");
					BPrintf(RADIO_YELLOW, "*********    IS OVER    *********");
					BPrintf(RADIO_YELLOW, "*********************************");
					BPrintf(RADIO_YELLOW, "*********  ALLIES %s  *********", arena->fields[field].country == COUNTRY_RED?"LOST":" WON");
					BPrintf(RADIO_YELLOW, "*********   AXIS  %s   *********", arena->fields[field].country == COUNTRY_GOLD?"LOST":"WON");
					BPrintf(RADIO_YELLOW, "*********************************");

					Com_LogEvent(EVENT_RESET, 0, 0);
					Com_LogDescription(EVENT_DESC_TERRAIN, 0, mapname->string);

					if (changemaponreset->value)
					{
						if (!arena->countdown)
						{
							arena->countdown = 18001; // 3 minutes
						}
					}
					else
					{
						LoadArenaStatus(dirname->string, NULL, 1);
					}

					for (i = 0; i < maxentities->value; i++)
					{
						
						if (clients[i].inuse)
						{
							if(ctf->value)
							{
								if(clients[i].ready && clients[i].infly && !clients[i].drone && clients[i].attr != 1)
									ForceEndFlight(TRUE, &clients[i]);
							}

							if (clients[i].drone & (DRONE_HMACK | DRONE_HTANK | DRONE_EJECTED))
								RemoveDrone(&clients[i]);

							if (clients[i].drone & (DRONE_COMMANDOS))
								clients[i].dronetimer = 0;
						}
					}

					return 1;
				}
			}

			////////////

			arena->fields[field].country = country;

			for (i = 0; i < MAX_BUILDINGS; i++)
			{
				if (arena->fields[field].buildings[i].field)
					arena->fields[field].buildings[i].country = country;
				else
					break;
			}

			if (arcade->value && rps->value) // set available planes w/o capture enemy planes
				UpdateRPS(0);
		}

		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				if(clients[i].thai) // Cmd_Capt
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				SendPacket(buffer, sizeof(buffer), &clients[i]);
			}
		}
		
		SendFieldStatus(field, NULL);
	}

	return 1;
}

/*************
 Cmd_White

 Include/exclude player in whitelist
 *************/

void Cmd_White(char *user, u_int8_t white, client_t *client)
{
	u_int32_t id = 0;
	u_int8_t all = 0;
	client_t *puser= NULL;

	if (!strcmp(user, "all"))
	{
		all = 1;
	}

	if (!all)
	{
		if ((puser = FindLClient(user)))
		{
			id = puser->id;
		}
		else
		{
			sprintf(my_query, "SELECT id FROM players WHERE longnick = '%s'", user);

			if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if (mysql_num_rows(my_result) > 0)
					{
						if ((my_row = mysql_fetch_row(my_result)))
						{
							id = Com_Atou(Com_MyRow("id"));
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Cmd_White(check): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}
					else
					{
						PPrintf(client, RADIO_YELLOW, "Player %s not found", user);
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_White(check): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				if (mysql_errno(&my_sock))
					Com_Printf(VERBOSE_WARNING, "Cmd_White(check): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
	}

	if (id || all)
	{
		sprintf(my_query, "UPDATE players SET white = '%u'", white);

		if (!all)
		{
			sprintf(my_query, "%s WHERE id = '%u'", my_query, id);
		}

		if (!d_mysql_query(&my_sock, my_query))
		{
			PPrintf(client, RADIO_YELLOW, "%s changed \'white\' attribute from \'%s\' to %u\n", client ? client->longnick : "-HOST-", user, white);
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_White(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}

	}
}

/*************
 Cmd_Chmod

 Changes user privileges
 *************/

void Cmd_Chmod(char *user, int8_t mod, client_t *client)
{
	u_int32_t id;
	u_int8_t attr;
	client_t *puser= NULL;

	if (client)
	{
		if (client->attr & FLAG_OP)
		{
			if (mod == 1)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "You are not authorized to give this privilege");
				return;
			}
		}
	}

	sprintf(my_query, "SELECT id, attr FROM players WHERE longnick = '%s'", user);

	if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if (mysql_num_rows(my_result) > 0)
			{
				if ((my_row = mysql_fetch_row(my_result)))
				{
					id = Com_Atou(Com_MyRow("id"));
					attr = Com_Atou(Com_MyRow("attr"));

					if (mod < 0)
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "\"%s\" is \"%s\"", user, attr == 1 ? "Admin" : attr == 2 ? "OP" : attr == 0 ? "Player" : "Unknown");
					}
					else
					{
						if (attr != mod)
						{
							sprintf(my_query, "UPDATE players SET attr = '%u' WHERE id = '%u' ", mod, id);

							if (!d_mysql_query(&my_sock, my_query))
							{
								if ((puser = FindLClient(user)))
								{
									if (((attr == 0 || attr == 2) && mod == 1) || ((attr == 1) && (mod == 2 || mod == 0)))
									{
										if (mod == 1 && hideadmin->value)
											arena->numplayers--;
										else if (hideadmin->value)
											arena->numplayers++;

										PPrintf(puser, RADIO_LIGHTYELLOW, "Your attributes are changed, please restart your Warbirds");
									}

									puser->attr = mod;

									puser->shortnick = ascii2wbnick(puser->longnick, puser->attr);
								}

								if (client)
									PPrintf(client, RADIO_YELLOW, "Attributes of %s changed to %s\n", user, mod == 1 ? "Admin" : mod == 2 ? "OP" : mod == 0 ? "Player" : "Unknown");

								Com_Printf(VERBOSE_ATTENTION, "Attributes of %s changed to %s by %s\n", user, mod == 1 ? "Admin" : mod == 2 ? "OP" : mod == 0 ? "Player" : "Unknown", client ? client->longnick : "-HOST-");
							}
							else
							{
								Com_Printf(VERBOSE_WARNING, "Cmd_Chmod(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							}
						}
						else
						{
							PPrintf(client, RADIO_LIGHTYELLOW, "%s is already %s", user, attr == 1 ? "an Admin" : attr == 2 ? "an OP" : attr == 0 ? "a Player" : "an Unknown");
						}
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Chmod(check): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Player %s not found", user);
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_Chmod(check): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		if (mysql_errno(&my_sock))
			Com_Printf(VERBOSE_WARNING, "Cmd_Chmod(check): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/*************
 Cmd_Part

 Configure plane parts
 *************/

void Cmd_Part(char *argv[], u_int8_t argc, client_t *client)
{
	int16_t i = 0, part = 0;
	u_int16_t j;
	client_t *player;

	if (!argv[0])
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "usage: .part <planenum>|<player> <part> [h<hitpo> a<apstop> i<immun> u<parent>]");
	}
	else
	{
		if (argv[1])
		{
			part = Com_Atoi(argv[1]);

			if (part >= MAX_PLACE)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid part number %d", part);
				return;
			}
		}

		if (isdigit(*argv[0])) // plane number
		{
			j = Com_Atoi(argv[0]);

			if (!GetPlaneName(j))
			{
				PPrintf(client, RADIO_YELLOW, "Invalid plane %d", j);
				return;
			}

			if (argv[1])
			{
				if (argc > 2)
				{
					for (i = 2; i < argc; i++) // pass thru all remain args (h, a, i, u)
					{
						if (tolower(*argv[i]) == 'h')
						{
							argv[i]++;
							arena->planedamage[j].points[part] = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'a')
						{
							argv[i]++;
							arena->planedamage[j].apstop[part] = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'i')
						{
							argv[i]++;
							arena->planedamage[j].imunity[part] = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'u')
						{
							argv[i]++;
							arena->planedamage[j].parent[part] = Com_Atoi(argv[i]);
						}
					}
				}
				else
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s %s h=%d a=%d i=%d u=%d(%s)", GetSmallPlaneName(j), GetSmallHitSite(part), arena->planedamage[j].points[part],
							arena->planedamage[j].apstop[part], arena->planedamage[j].imunity[part], arena->planedamage[j].parent[part], GetSmallHitSite(arena->planedamage[j].parent[part]));
				}
			}
			else
			{
				for (part = 0; part < MAX_PLACE; part++)
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s %d(%s) h=%d a=%d i=%d u=%d(%s)", GetSmallPlaneName(j), part, GetSmallHitSite(part), arena->planedamage[j].points[part],
							arena->planedamage[j].apstop[part], arena->planedamage[j].imunity[part], arena->planedamage[j].parent[part], GetSmallHitSite(arena->planedamage[j].parent[part]));
				}
			}
		}
		else // player nick
		{
			player = FindLClient(argv[0]);

			if (!player)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Player not found");
				return;
			}

			if (argv[1])
			{
				if (argc > 2)
				{
					for (i = 2; i < argc; i++) // pass thru all remain args (h, a, i, u)
					{
						if (tolower(*argv[i]) == 'h')
						{
							argv[i]++;
							player->armor.points[part] = Com_Atoi(argv[i]);

							// FIX: if argv[i] == Zero, explode part
						}
						else if (tolower(*argv[i]) == 'a')
						{
							argv[i]++;
							player->armor.apstop[part] = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'i')
						{
							argv[i]++;
							player->armor.imunity[part] = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'u')
						{
							argv[i]++;
							player->armor.parent[part] = Com_Atoi(argv[i]);
						}
					}
				}
				else
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s %s h=%d a=%d i=%d u=%d(%s)", player->longnick, GetSmallHitSite(part), player->armor.points[part], player->armor.apstop[part],
							player->armor.imunity[part], player->armor.parent[part], GetSmallHitSite(player->armor.parent[part]));
				}
			}
			else
			{
				for (part = 0; part < MAX_PLACE; part++)
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s %d(%s) h=%d a=%d i=%d u=%d(%s)", player->longnick, part, GetSmallHitSite(part), player->armor.points[part], player->armor.apstop[part],
							player->armor.imunity[part], player->armor.parent[part], GetSmallHitSite(player->armor.parent[part]));
				}
			}
		}
	}
}

/*************
 Cmd_Decl

 Declares an structure
 *************/

void Cmd_Decl(char *argv[], u_int8_t argc, client_t *client)
{
	building_t *build;
	int16_t i, j, field;

	if (!argv[0])
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "=========B=U=I=L=D=I=N=G=========");
		PPrintf(client, RADIO_LIGHTYELLOW, "1-50 CAL Ack  18-Bridge");
		PPrintf(client, RADIO_LIGHTYELLOW, "2-20mm Ack    19-Special Building");
		PPrintf(client, RADIO_LIGHTYELLOW, "3-40mm Ack    20-Factory");
		PPrintf(client, RADIO_LIGHTYELLOW, "4-88mm Flak   21-Barracks");
		PPrintf(client, RADIO_LIGHTYELLOW, "5-Tower       22-Static plane");
		PPrintf(client, RADIO_LIGHTYELLOW, "6-Hangar      23-Refinery");
		PPrintf(client, RADIO_LIGHTYELLOW, "7-Fuel        24-Plane Factory");
		PPrintf(client, RADIO_LIGHTYELLOW, "8-Ammo Depot  25-Building");
		PPrintf(client, RADIO_LIGHTYELLOW, "9-Radar       26-Crane");
		PPrintf(client, RADIO_LIGHTYELLOW, "10-Warehouse  27-Strategic Target");
		PPrintf(client, RADIO_LIGHTYELLOW, "11-Radio      28-Artillery");
		PPrintf(client, RADIO_LIGHTYELLOW, "12-Antenna    29-Hut");
		PPrintf(client, RADIO_LIGHTYELLOW, "13-CV         30-Truck");
		PPrintf(client, RADIO_LIGHTYELLOW, "14-Destroyer  31-Tree");
		PPrintf(client, RADIO_LIGHTYELLOW, "15-Cruiser    32-Spawn Point");
		PPrintf(client, RADIO_LIGHTYELLOW, "16-Cargo Ship 33-House");
		PPrintf(client, RADIO_LIGHTYELLOW, "17-Submarine  34-Rock");
		PPrintf(client, RADIO_LIGHTYELLOW, "usage: .decl <buildID> f<field> [<type>&|x<value>&|y<value>&|z<value>]\nOBS:if already field declared you can change type using only <type>");
		PPrintf(client, RADIO_LIGHTYELLOW, "============F=I=E=L=D============");
		PPrintf(client, RADIO_LIGHTYELLOW, "1-Small       11-Port");
		PPrintf(client, RADIO_LIGHTYELLOW, "2-Medium      12-Convoy");
		PPrintf(client, RADIO_LIGHTYELLOW, "3-Main        13-Factory");
		PPrintf(client, RADIO_LIGHTYELLOW, "4-CV          14-Refinery");
		PPrintf(client, RADIO_LIGHTYELLOW, "5-Cargo       15-OpenField");
		PPrintf(client, RADIO_LIGHTYELLOW, "6-Destroyers  16-WB3Post");
		PPrintf(client, RADIO_LIGHTYELLOW, "7-Submarine   17-WB3Village");
		PPrintf(client, RADIO_LIGHTYELLOW, "8-Radar       18-WB3Town");
		PPrintf(client, RADIO_LIGHTYELLOW, "9-Bridge      19-WB3Port");
		PPrintf(client, RADIO_LIGHTYELLOW, "10-City");
		PPrintf(client, RADIO_LIGHTYELLOW, "usage: .decl f<field> [<type>&|x<value>&|y<value>&|z<value>&|r<value>]");
		PPrintf(client, RADIO_LIGHTYELLOW, "=============C=I=T=Y=============");
		PPrintf(client, RADIO_LIGHTYELLOW, "8-Radar       12-Convoy");
		PPrintf(client, RADIO_LIGHTYELLOW, "9-Bridge      13-Factory");
		PPrintf(client, RADIO_LIGHTYELLOW, "10-City       14-Refinery");
		PPrintf(client, RADIO_LIGHTYELLOW, "11-Port       15-Open field");
		PPrintf(client, RADIO_LIGHTYELLOW, "usage: .decl c<city> [<type>&|x<value>&|y<value>&|z<value>&|l<field>]");
	}
	else if (!argv[1]) // print current set for given attribute
	{
		if (tolower(*argv[0]) == 'f')
		{
			argv[0]++;
			if ((i = Com_Atoi(argv[0])) <= fields->value)
				PPrintf(client, RADIO_LIGHTYELLOW, "Field %d, type %s, x=%d, y=%d, z=%d, r=%d", arena->fields[i-1].number, GetFieldType(arena->fields[i-1].type), arena->fields[i-1].posxyz[0],
						arena->fields[i-1].posxyz[1], arena->fields[i-1].posxyz[2], arena->fields[i-1].radius);
			else
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid Field");
			return;
		}
		else if (tolower(*argv[0]) == 'c')
		{
			argv[0]++;
			if ((i = Com_Atoi(argv[0])) <= cities->value)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "City %d, type %s, x=%d, y=%d, z=%d, f=%d, c=%d, ctr=%d\nn=%s", i, GetFieldType(arena->cities[i-1].type), arena->cities[i-1].posxyz[0],
						arena->cities[i-1].posxyz[1], arena->cities[i-1].posxyz[2], arena->cities[i-1].field ? arena->cities[i-1].field->number : 0, arena->cities[i-1].needtoclose,
						arena->cities[i-1].country, arena->cities[i-1].name);
			}
			else
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid City");
			return;
		}

		if (!(build = GetBuilding(Com_Atoi(argv[0]))))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Building is not assigned to a field");
		}
		else
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Type: %s, Field: %c%d, x=%d, y=%d, z=%d, %s", GetBuildingType(build->type),
					((build->fieldtype <= FIELD_SUBMARINE) || (build->fieldtype >= FIELD_WB3POST)) ? 'f' : 'c', build->field, build->posx, build->posy, build->posz, build->infield?"IN":"OUT");
			PPrintf(client, RADIO_LIGHTYELLOW, "Hitpoints: %d, APstop: %d, Imunity: %d", build->armor, GetBuildingAPstop(build->type, client), GetBuildingImunity(build->type, client));
		}
	}
	else
	{
		if (tolower(*argv[0]) == 'f') // set field attributes
		{
			argv[0]++;

			if (Com_Atoi(argv[0]) > fields->value)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid Field");
				return;
			}

			for (i = 1; i < argc; i++)
			{
				if (tolower(*argv[i]) == 'x')
				{
					argv[i]++;
					arena->fields[Com_Atoi(argv[0])-1].posxyz[0] = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'y')
				{
					argv[i]++;
					arena->fields[Com_Atoi(argv[0])-1].posxyz[1] = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'z')
				{
					argv[i]++;
					arena->fields[Com_Atoi(argv[0])-1].posxyz[2] = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'r')
				{
					argv[i]++;
					arena->fields[Com_Atoi(argv[0])-1].radius = Com_Atou(argv[i]);
				}
				else if (tolower(*argv[i]) == 'c')
				{
					argv[i]++;

					i = Com_Atoi(argv[i]);

					if (i && i > cities->value)
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Invalid city");
						return;
					}

					for (j = 0; j < MAX_CITYFIELD; j++)
					{
						if (arena->fields[Com_Atoi(argv[0])-1].city[j] == &arena->cities[i - 1])
							break;
					}

					if (j == MAX_CITYFIELD)
					{
						for (j = 0; j < MAX_CITYFIELD; j++)
						{
							if (!arena->fields[Com_Atoi(argv[0])-1].city[j])
							{
								arena->fields[Com_Atoi(argv[0])-1].city[j] = &arena->cities[i - 1];
								arena->cities[i - 1].field = &arena->fields[Com_Atoi(argv[0])-1];
							}
						}
					}
				}
				else
				{
					arena->fields[Com_Atoi(argv[0])-1].type = Com_Atoi(argv[i]);
					SetBFieldType(arena->fields[Com_Atoi(argv[0])-1].buildings, Com_Atoi(argv[i]));
				}
			}
		}
		else if (tolower(*argv[0]) == 'c') // set cities attributes
		{
			argv[0]++;

			if (Com_Atoi(argv[0]) > cities->value)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid city");
				return;
			}

			for (i = 1; i < argc; i++)
			{
				if (tolower(*argv[i]) == 'x')
				{
					argv[i]++;
					arena->cities[Com_Atoi(argv[0])-1].posxyz[0] = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'y')
				{
					argv[i]++;
					arena->cities[Com_Atoi(argv[0])-1].posxyz[1] = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'z')
				{
					argv[i]++;
					arena->cities[Com_Atoi(argv[0])-1].posxyz[2] = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'f')
				{
					argv[i]++;

					i = Com_Atoi(argv[i]);

					if (i && i > fields->value)
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Invalid field");
						return;
					}

					if (arena->cities[Com_Atoi(argv[0])-1].field)
					{
						for (j = 0; j < MAX_CITYFIELD; j++)
						{
							if (arena->cities[Com_Atoi(argv[0])-1].field->city[j] == &arena->cities[Com_Atoi(argv[0]) - 1])
							{
								arena->cities[Com_Atoi(argv[0])-1].field->city[j] = 0;
								break;
							}
						}

						arena->cities[Com_Atoi(argv[0])-1].field = 0;
					}

					for (j = 0; j < MAX_CITYFIELD; j++)
					{
						if (!arena->fields[i - 1].city[j])
						{
							arena->cities[Com_Atoi(argv[0])-1].field = &arena->fields[i - 1];
							arena->fields[i - 1].city[j] = &arena->cities[Com_Atoi(argv[0]) - 1];
							break;
						}
					}

					if (j == MAX_CITYFIELD)
						Com_Printf(VERBOSE_WARNING, "Cmd_Decl(): Couldnt attach city to field\n");

				}
				else if (tolower(*argv[i]) == 'c')
				{
					argv[i]++;

					arena->cities[Com_Atoi(argv[0])-1].needtoclose = Com_Atoi(argv[i]);
				}
				else if (tolower(*argv[i]) == 'n')
				{
					argv[i]++;

					strcpy(arena->cities[Com_Atoi(argv[0])-1].name, argv[i]);
				}
				else
				{
					arena->cities[Com_Atoi(argv[0])-1].type = Com_Atoi(argv[i]);
					SetBFieldType(arena->cities[Com_Atoi(argv[0])-1].buildings, Com_Atoi(argv[i]));
				}
			}
		}
		else // struct attributes
		{
			for (i = 1; i < argc; i++)
			{
				if (tolower(*argv[i]) == 'f')
				{
					if (strlen(argv[i]) > 1)
					{
						argv[i]++;
						if (!isdigit(*argv[i]))
						{
							PPrintf(client, RADIO_LIGHTYELLOW, "Invalid argument");
							return;
						}
					}
					else
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Invalid argument");
						return;
					}

					if (GetBuilding(Com_Atoi(argv[0])))
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Building %s already assigned to a field or city", argv[0]);
					}
					else
					{
						field = Com_Atoi(argv[i])-1;

						for (j = 0; j < MAX_BUILDINGS; j++)
						{
							if (!arena->fields[field].buildings[j].field)
							{
								arena->fields[field].buildings[j].field = field + 1;
								arena->fields[field].buildings[j].fieldtype = arena->fields[field].type;
								arena->fields[field].buildings[j].id = Com_Atoi(argv[0]);
								break;
							}
						}
					}
				}
				else if (tolower(*argv[i]) == 'c')
				{
					if (strlen(argv[i]) > 1)
					{
						argv[i]++;
						if (!isdigit(*argv[i]))
						{
							PPrintf(client, RADIO_LIGHTYELLOW, "Invalid Argument");
							return;
						}
					}
					else
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Invalid Argument");
						return;
					}

					if (GetBuilding(Com_Atoi(argv[0])))
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Building %s already assigned to a field or city", argv[0]);
					}
					else
					{
						field = Com_Atoi(argv[i])-1;

						for (j = 0; j < MAX_BUILDINGS; j++)
						{
							if (!arena->cities[field].buildings[j].field)
							{
								arena->cities[field].buildings[j].field = field + 1;
								arena->cities[field].buildings[j].fieldtype = arena->cities[field].type;
								arena->cities[field].buildings[j].id = Com_Atoi(argv[0]);
								break;
							}
						}
					}
				}
				else
				{
					if (!(build = GetBuilding(Com_Atoi(argv[0]))))
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Building %d is not assigned to a field", Com_Atoi(argv[0]));
					}
					else
					{
						if (tolower(*argv[i]) == 'x')
						{
							argv[i]++;
							build->posx = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'y')
						{
							argv[i]++;
							build->posy = Com_Atoi(argv[i]);
						}
						else if (tolower(*argv[i]) == 'z')
						{
							argv[i]++;
							build->posz = Com_Atoi(argv[i]);
						}
						else
						{
							build->type = Com_Atoi(argv[i]);
							build->armor = GetBuildingArmor(build->type, client);
						}
					}
				}
			}
		}
	}
}

/*************
 Cmd_Pingtest

 Make a Pingtest call
 *************/
 
void Cmd_Pingtest(u_int16_t frame, client_t *client)
{
	buildstatus_t *buildstatus;
	u_int8_t buffer[6];

	memset(buffer, 0, sizeof(buffer));

	buildstatus = (buildstatus_t *) buffer;

	buildstatus->packetid = htons(Com_WBhton(0x0307));
	buildstatus->building = htons(frame);
	buildstatus->status = 4;
	if(!Com_Strncmp(mapname->string, "atoll", 5))
		buildstatus->country = arena->fields[1].country;
	else
		buildstatus->country = arena->fields[0].country;
	if (!frame)
		client->pingtest = arena->time;

	SendPacket(buffer, sizeof(buffer), client);
}

/*************
 Cmd_Undecl

 Remove an structure from declaration list
 *************/

void Cmd_Undecl(u_int16_t id, client_t *client)
{
	u_int16_t i, j;
	building_t *build, *buildings;

	if (id == 0xffff)
	{
		for (i = 0; i < fields->value; i++)
		{
			for (j = 0; j < MAX_BUILDINGS; j++)
				arena->fields[i].buildings[j].field = 0;
		}

		for (i = 0; i < cities->value; i++)
		{
			for (j = 0; j < MAX_BUILDINGS; j++)
				arena->cities[i].buildings[j].field = 0;
		}

		PPrintf(client, RADIO_LIGHTYELLOW, "All buildings undeclared");
	}
	else
	{
		if (!(build = GetBuilding(id)))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Building not declared yet");
			return;
		}

		i = build->field - 1;

		if ((build->fieldtype <= FIELD_SUBMARINE) || (build->fieldtype <= FIELD_WB3POST))
		{
			buildings = arena->fields[i].buildings;
		}
		else
		{
			buildings = arena->cities[i].buildings;
		}

		for (j = 0; j < MAX_BUILDINGS; j++)
		{
			if (buildings[j].id == id)
			{
				buildings[j].field = 0;

				if (++j < MAX_BUILDINGS)
				{
					while (buildings[j].field)
					{
						buildings[j-1].field = buildings[j].field;
						buildings[j-1].fieldtype = buildings[j].fieldtype;
						buildings[j-1].country = buildings[j].country;
						buildings[j-1].id = buildings[j].id;
						buildings[j-1].type = buildings[j].type;
						buildings[j-1].status = buildings[j].status;
						buildings[j-1].timer = buildings[j].timer;
						buildings[j-1].armor = buildings[j].armor;
						buildings[j-1].posx = buildings[j].posx;
						buildings[j-1].posy = buildings[j].posy;
						buildings[j-1].posz = buildings[j].posz;

						buildings[j].field = 0;

						if (++j >= MAX_BUILDINGS)
							break;
					}
				}
			}
		}
		PPrintf(client, RADIO_LIGHTYELLOW, "Building %d undeclared", id);
	}
}

/*************
 Cmd_Time

 Set Arena Time
 *************/

void Cmd_Time(u_int16_t time, char *mult, client_t *client)
{
	u_int8_t i, hour, minute;
	arenatime_t *arenatime;
	u_int8_t buffer[5];

	hour = time/100;
	minute = time%100;

	memset(buffer, 0, sizeof(buffer));

	arenatime = (arenatime_t *) buffer;

	arenatime->packetid = htons(Com_WBhton(0x0301));
	arenatime->hour = arena->hour = hour;
	arenatime->minute = arena->minute = minute;

	if (mult)
	{
		Var_Set("timemult", mult);
	}

	arenatime->mult = arena->multiplier;

	if (client)
	{
		SendPacket(buffer, sizeof(buffer), client);
	}
	else
	{
		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone && clients[i].ready)
			{
				SendPacket(buffer, sizeof(buffer), &clients[i]);
				SendArenaRules(&clients[i]);
			}
		}
	}

	planerangelimit->modified = enemyidlim->modified = friendlyidlim->modified = 1;
}

/*************
 Cmd_Date

 Set Arena Date
 *************/

void Cmd_Date(u_int8_t month, u_int8_t day, u_int16_t year, client_t *client)
{

	if (!month || month > 12)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Invalid month");
		return;
	}

	if (day > 28) // check days
	{
		if (month == 2 && (year % 4))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Invalid day");
			return;
		}

		if (day > 30)
		{
			if ((month == 4 || month == 6 || month == 9 || month == 11))
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid day");
				return;
			}
			else if (day > 31)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid day");
				return;
			}
		}
	}

	arena->day = day;
	arena->month = month;
	arena->year = year;

	if (wb3->value)
		WB3DotCommand(NULL, ".date %u %u %u", arena->month, arena->day, arena->year);
}

/*************
 Cmd_Field

 Send field info
 *************/

void Cmd_Field(u_int8_t field, client_t *client)
{
	u_int8_t country, type, status, build_alive, build_total;
	u_int16_t i, j;
	u_int32_t reup, treup;
	float tonnage_recover;
	u_int8_t c_cities, totalcities;
	FILE *fp;
	char buffer[32];

	if (!field)
	{
		// Prints all fields status
		if (!(fp = fopen("./fields/fields.txt", "wb")))
		{
			PPrintf(client, RADIO_YELLOW, "WARNING: Cmd_Field() Couldn't open file \"./fields/fields.txt\"");
		}
		else
		{
			fprintf(fp, "FIELD   SIDE    TYPE   STATUS   UP   PARAS");
			
			if(!oldcapt->value && wb3->value)
			{
				fprintf(fp, "   ToT   TTC\n-------------------------------------------------------\n");
			}
			else
			{
				fprintf(fp, "\n--------------------------------------------\n");
			}

			for(i = 0; i < fields->value; i++)
			{
				fprintf(fp, "F%d%s%9s%9s", i+1, i < 9 ? " " :"", GetCountry(arena->fields[i].country), GetFieldType(arena->fields[i].type));

				if (arena->fields[field].type >= FIELD_CV && arena->fields[field].type <= FIELD_SUBMARINE)
				{
					if (arena->fields[field].cv)
						fprintf(fp, "%.3f ft/s", arena->fields[field].cv->speed);
					else
						Com_Printf(VERBOSE_WARNING, "Cmd_Field() cv pointer = 0\n");
				}
				else
				{
					fprintf(fp, "%8s", arena->fields[i].closed ? "Closed" : "Open");
				}
				
				build_total = build_alive = 0;
				
				for (j = 0; j < MAX_BUILDINGS; j++)
				{
					if (!arena->fields[i].buildings[j].field)
						break;

					if (IsVitalBuilding(&(arena->fields[i].buildings[j]), oldcapt->value))
					{
						if(!arena->fields[i].buildings[j].status)
							build_alive++;
						build_total++;
					}
				}

				fprintf(fp, "%5.0f%%%4d/%02d", (float)build_alive*100/build_total, arena->fields[i].paras, GetFieldParas(arena->fields[i].type));
				//fprintf(fp, "%s", buffer);

				if(!oldcapt->value && wb3->value)
				{
					fprintf(fp, "%6.0f%6.0f\n", arena->fields[i].tonnage, GetTonnageToClose(i+1));
				}
				else
				{
					fprintf(fp, "\n");
				}
				
//				if (status)
//				{
//					fprintf(fp, "ABLE TO CAPTURE: %s\n", arena->fields[field].abletocapture ? "Yes" : "No");
//				}
			}

			fclose(fp);

			if(client)
				SendFileSeq1("./fields/fields.txt", "fields.asc", client);
		}
		return;
	}

	if (field > fields->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid field");
		return;
	}

	field--;

	country = arena->fields[field].country;
	type = arena->fields[field].type;
	status = arena->fields[field].closed;

	build_total = build_alive = 0;
	
	for (i = 0; i < MAX_BUILDINGS; i++)
	{
		if (!arena->fields[field].buildings[i].field)
			break;

		if (IsVitalBuilding(&(arena->fields[field].buildings[i]), oldcapt->value))
		{
			if(!arena->fields[field].buildings[i].status)
				build_alive++;
			
			build_total++;
		}
	}

	if (!client || client->infly)
	{
		PPrintf(client, RADIO_YELLOW, "Field F%d: Country: %s, Type: %s, Status: %s", field+1, GetCountry(country), GetFieldType(type), status ? "Closed" : "Open");
		
		if(!oldcapt->value && wb3->value)
		{
			PPrintf(client, RADIO_YELLOW, "%.2f%% up, %d/%d paras, ToT: %.2f, TTC: %.2f", (float)build_alive*100/build_total, arena->fields[field].paras, GetFieldParas(arena->fields[field].type), arena->fields[field].tonnage, GetTonnageToClose(field+1));
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "%.2f%% up, %d/%d paras", (float)build_alive*100/build_total, arena->fields[field].paras, GetFieldParas(arena->fields[field].type));
		}

		if (status)
		{
			reup = MAX_UINT32;

			for (i = 0; i < MAX_BUILDINGS; i++)
			{
				if (!arena->fields[field].buildings[i].field)
					break;

				if (arena->fields[field].buildings[i].status && arena->fields[field].buildings[i].timer < reup && IsVitalBuilding(&(arena->fields[field].buildings[i]), oldcapt->value))
				{
					reup = arena->fields[field].buildings[i].timer;
				}
			}
			
			if(!oldcapt->value && wb3->value)
			{
				c_cities = totalcities = 0;

				for(i = 0; i < fields->value; i++)
				{
					if((arena->fields[i].type >= FIELD_WB3POST) && (arena->fields[i].type <= FIELD_WB3PORT))
					{
						if(arena->fields[i].country == arena->fields[field].country)
						{
							c_cities++;
						}
						
						totalcities++;
					}
				}

				tonnage_recover = (1.0 + (((float)c_cities / totalcities) - 0.5) / 2.0) * GetTonnageToClose(field+1) / (24.0 * rebuildtime->value / 9.0);

				treup = ((arena->fields[field].tonnage - GetTonnageToClose(field+1)) * 6000) / tonnage_recover;
			
				if(treup < reup)
				{
					if(!client || client->attr)
						PPrintf(client, RADIO_RED, "DEBUG: Reup: %u, ToT Reup = %u", reup, treup);
					reup = treup;
				}
			}

			PPrintf(client, RADIO_YELLOW, "Reopen in %s, Able to capture: %s", Com_TimeSeconds(reup/100), arena->fields[field].abletocapture ? "Yes" : "No");
		}
	}
	else
	{
		memset(buffer, 0, sizeof(buffer));

		sprintf(buffer, "./fields/field%d.txt", field+1);

		if (!(fp = fopen(buffer, "wb")))
		{
			PPrintf(client, RADIO_YELLOW, "WARNING: Cmd_Field() Couldn't open file \"%s\"", buffer);
		}
		else
		{
			fprintf(fp, "FIELD F%d\nCOUNTRY: %s\nTYPE: %s\nSTATUS: %s\nPARAS: %d/%d\n\n", field+1, GetCountry(country), GetFieldType(type), status ? "Closed" : "Open", arena->fields[field].paras, GetFieldParas(arena->fields[field].type));
			if(!oldcapt->value && wb3->value)
			{
				fprintf(fp, "TONNAGE ON TARGET: %.2f\nTONNAGE TO CLOSE: %.2f\n", arena->fields[field].tonnage, GetTonnageToClose(field+1));
			}

			if (status)
			{
				fprintf(fp, "ABLE TO CAPTURE: %s\n", arena->fields[field].abletocapture ? "Yes" : "No");
			}

			if (arena->fields[field].type >= FIELD_CV && arena->fields[field].type <= FIELD_SUBMARINE)
			{
				if (arena->fields[field].cv)
					fprintf(fp, "CONVOY SPEED: %.3f ft/s\n\n", arena->fields[field].cv->speed);
				else
					Com_Printf(VERBOSE_WARNING, "Cmd_Field() cv pointer = 0\n");
			}
			else
			{
				fputc('\n', fp);
			}

			for (i = 0; i < MAX_BUILDINGS; i++)
			{
				if (arena->fields[field].buildings[i].field)
				{
					if (IsVitalBuilding(&(arena->fields[field].buildings[i]), oldcapt->value))
					{
						fprintf(fp, "%s %s", GetBuildingType(arena->fields[field].buildings[i].type), arena->fields[field].buildings[i].timer ? "DESTROYED" : "LIVES");

						if (arena->fields[field].buildings[i].timer)
							if ((arena->fields[field].type <= FIELD_MAIN) || (arena->fields[field].type >= FIELD_WB3POST))
								fprintf(fp, " (%s)\n", Com_TimeSeconds(arena->fields[field].buildings[i].timer/100));
							else
								fprintf(fp, " DESTROYED\n");
						else
							fputc('\n', fp);
					}
				}
				else
					break;
			}

			for (i = 0; i < MAX_CITYFIELD; i++)
			{
				if (arena->fields[field].city[i])
				{
					if (arena->fields[field].city[i]->needtoclose)
					{
						fprintf(fp, "%s (c%d) (%s) %s \n", arena->fields[field].city[i]->name, arena->fields[field].city[i]->number, Com_Padloc(arena->fields[field].city[i]->posxyz[0],
								arena->fields[field].city[i]->posxyz[1]), arena->fields[field].city[i]->closed ? "CLOSED" : "OPEN");
					}
				}
			}

			fclose(fp);

			SendFileSeq1(buffer, "field.asc", client);
		}
	}
}

/*************
 Cmd_City

 Send city info
 *************/

void Cmd_City(u_int8_t citynum, client_t *client)
{
	u_int8_t country, type, status;
	u_int16_t i;
	FILE *fp;
	char buffer[32];

	if (!citynum)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid argument");
		return;
	}

	if (citynum > cities->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid City");
		return;
	}

	country = arena->cities[citynum-1].country;
	type = arena->cities[citynum-1].type;
	status = arena->cities[citynum-1].closed;

	if (!client || client->infly)
	{
		PPrintf(client, RADIO_YELLOW, "City C%d: Country %s, Type: %s, Status: %s", citynum, GetCountry(country), GetFieldType(type), status ? "Closed" : "Open");
	}
	else
	{
		memset(buffer, 0, sizeof(buffer));

		sprintf(buffer, "./fields/city%d.txt", citynum);

		if (!(fp = fopen(buffer, "wb")))
		{
			PPrintf(client, RADIO_YELLOW, "WARNING: Cmd_Field() Cannot open file \"%s\"", buffer);
		}

		fprintf(fp, "CITY C%d\nNAME: %s\nCOUNTRY: %s\nTYPE: %s\nLINKED TO: F%d\nPOSITION: %s\nNEED TO CLOSE:%s\nSTATUS: %s\n\n", citynum, arena->cities[citynum-1].name, GetCountry(country),
				GetFieldType(type), arena->cities[citynum-1].field ? arena->cities[citynum-1].field->number : 0, Com_Padloc(arena->cities[citynum-1].posxyz[0], arena->cities[citynum-1].posxyz[1]),
				arena->cities[citynum-1].needtoclose ? "Yes" : "No", status ? "Closed" : "Open");
		citynum--;

		for (i = 0; i < MAX_BUILDINGS; i++)
		{
			if (arena->cities[citynum].buildings[i].field)
			{
				fprintf(fp, "%s %s\n", GetBuildingType(arena->cities[citynum].buildings[i].type), arena->cities[citynum].buildings[i].timer ? "DESTROYED" : "LIVES");
			}
			else
				break;
		}

		fclose(fp);

		SendFileSeq1(buffer, "city.asc", client);
	}
}

/*************
 Cmd_StartFau

 Declares an structure
 *************/

void Cmd_StartFau(u_int32_t dist, float angle, u_int8_t attached, client_t *client)
{
	client_t *drone;
	u_int32_t time;

	if (arena->fields[client->field-1].closed)
	{
		PPrintf(client, RADIO_YELLOW, "This field is closed");
		return;
	}

	if (arena->fields[client->field-1].country != client->country)
	{
		PPrintf(client, RADIO_YELLOW, "This field is not from your country");
		return;
	}

	if (client->tkstatus)
	{
		PPrintf(client, RADIO_YELLOW, "You are a team killer, you cannot use V-1");
		return;
	}

	if (arena->fields[client->field-1].warehouse)
	{
		PPrintf(client, RADIO_YELLOW, "You can't launch V-1 from this field. Warehouse is destroyed!");
		return;
	}

	if (angle*10 > 3599)
	{
		PPrintf(client, RADIO_YELLOW, "angle: 0<->359.9 degrees");
		return;
	}

	if ((dist < 30000) || (dist > 158400))
	{
		PPrintf(client, RADIO_YELLOW, "dist: 30,000<->158,400 feet");
		return;
	}

	if (arena->year < 1944)
	{
		PPrintf(client, RADIO_YELLOW, "We don't have V-1 before 1944");
		return;
	}

	if (client->attr == 1)
	{
		PPrintf(client, RADIO_YELLOW, "You're an Admin, you can't start V-1");
		return;
	}

	if ((arena->fields[client->field - 1].rps[PLANE_FAU] > -1 && arena->fields[client->field - 1].rps[PLANE_FAU] < 1))
	{
		if (rps->value)
		{
			time = ((u_int32_t)(rps->value * 6000) - (arena->frame % (u_int32_t)(rps->value * 6000)))/100;
		}
		else
		{
			time = 0;
		}

		PPrintf(client, RADIO_YELLOW, "No FAU available, wait %s", Com_TimeSeconds(time));
		return;
	}
	else if (arena->fields[client->field - 1].rps[PLANE_FAU] >= 1)
	{
		arena->fields[client->field - 1].rps[PLANE_FAU]--;
	}

	drone = AddDrone(DRONE_FAU, arena->fields[client->field - 1].posxyz[0], arena->fields[client->field - 1].posxyz[1], arena->fields[client->field - 1].posxyz[2], client->country, PLANE_FAU, client);

	if (!drone)
		return;

	Com_LogEvent(EVENT_FAU, client->id, 0);
	Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
	Com_LogDescription(EVENT_DESC_FIELD, client->field, NULL);

	drone->status_damage = 0xC003;

	drone->speedxyz[0][0] = DRONE_FAU_SPEED * sin(Com_Rad(angle)) * -1;
	drone->speedxyz[1][0] = DRONE_FAU_SPEED * cos(Com_Rad(angle));
	drone->speedxyz[2][0] = 100;
	drone->angles[0][0] = 80;

	PPrintf(client, RADIO_YELLOW, "V-1 launched azimuth: %.1f°, distance: %d fts ", angle, dist);

	if ((angle *= 10) > 901)
	{
		angle -= 3600;
	}

	drone->angles[2][0] = angle;
	drone->dronetimer = (float)dist*100/DRONE_FAU_SPEED;

	if (attached)
	{
		client->attached = drone;
		SendGunnerStatusChange(drone, 2, client); // define position to be attached in drone
		SendAttachList(NULL, client);
		client->visible[MAX_SCREEN - 1].client = drone;
		AddRemovePlaneScreen(drone, client, FALSE);
		Cmd_Fly(2, client);
		//SendPlaneStatus(drone, client);
	}
}

/*************
 Cmd_Say

 -HOST- Message
 *************/

void Cmd_Say(char *argv[], u_int8_t argc, client_t *client)
{
	char buffer[256];
	u_int8_t i;
	u_int16_t len;

	memset(buffer, 0, sizeof(buffer));

	for (i = 0, len = 0; i < argc; i++)
	{
		if ((len += strlen(argv[i])) < 256)
		{
			strcat(buffer, argv[i]);
			if (i < argc-1)
				buffer[strlen(buffer)] = ' ';
		}
		else
			break;
	}

	if (!client)
		BPrintf(RADIO_BLUE, "%s", buffer);
	else
		PPrintf(client, RADIO_YELLOW, "%s", buffer); // echo
}

/*************
 Cmd_Seta

 Set amount of planes to a desired field or field type
 *************/

void Cmd_Seta(char *field, int8_t country, int16_t plane, int8_t amount)
{
	int16_t ftype, fnum, i, j;

	if (tolower(*field) == 'f')
	{
		field++;

		if ((fnum = Com_Atoi(field)) > 0)
		{
			if (plane > 0 && plane < maxplanes)
				arena->fields[fnum-1].rps[plane] = amount;
			else if (plane < 0)
			{
				for (i = 0; i < maxplanes; i++)
					arena->fields[fnum-1].rps[i] = amount;
			}
		}
		else if (fnum < 0)
		{
			for (i = 0; i < fields->value; i++)
			{
				if (fnum == -2 && (arena->fields[i].type == FIELD_CV || arena->fields[i].type == FIELD_CARGO || arena->fields[i].type == FIELD_DD || arena->fields[i].type == FIELD_SUBMARINE))
					continue;

				if (plane > 0 && plane < maxplanes)
					arena->fields[i].rps[plane] = amount;
				else if (plane < 0)
				{
					for (j = 0; j < maxplanes; j++)
						arena->fields[i].rps[j] = amount;
				}
			}

		}
	}
	else if (tolower(*field) == 't')
	{
		field++;

		if ((ftype = Com_Atoi(field) > FIELD_SUBMARINE))
			ftype = FIELD_SUBMARINE;
		if (ftype < FIELD_LITTLE)
			ftype = FIELD_LITTLE;

		for (i = 0; i < fields->value; i++)
		{
			if ((arena->fields[i].type == ftype) && (arena->fields[i].country == country))
			{
				arena->fields[i].rps[plane] = amount;
			}
		}
	}

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone && clients[i].ready && !clients[i].infly)
			SendRPS(&clients[i]);
	}
}

/*************
 Cmd_Show

 Show arena settings
 *************/

void Cmd_Show(client_t *client)
{
	char filename[128];
	FILE *fp;

	memset(filename, 0, sizeof(filename));

	sprintf(filename, "%s.LOCK", FILE_ARNASETTINGS);

	Sys_WaitForLock(filename);

	if (Sys_LockFile(filename) < 0)
		return;

	filename[strlen(filename) - 5] = '\0';

	if ((fp = fopen(filename, "wb")) == NULL)
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't open file \"%s\"\n", filename);
	}
	else
	{
		fprintf(fp, "airshowsmoke     %8s\n", airshowsmoke->string);
		fprintf(fp, "blackout         %8s\n", blackout->string);
		fprintf(fp, "contrail         %8s\n", contrail->string);
		fprintf(fp, "countrytime      %8s\n", countrytime->string);
		fprintf(fp, "currday          %8s\n", currday->string);
		fprintf(fp, "currmonth        %8s\n", currmonth->string);
		fprintf(fp, "curryear         %8s\n", curryear->string);
		fprintf(fp, "cvcapture        %8s\n", cvcapture->string);
		fprintf(fp, "cvdelay          %8s\n", cvdelay->string);
		fprintf(fp, "cvradarrange1    %8s\n", cvradarrange1->string);
		fprintf(fp, "cvradarrange3    %8s\n", cvradarrange3->string);
		fprintf(fp, "cvrange          %8s\n", cvrange->string);
		fprintf(fp, "cvs              %8s\n", cvs->string);
		fprintf(fp, "cvsalvo          %8s\n", cvsalvo->string);
		fprintf(fp, "cvspeed          %8s\n", cvspeed->string);
		fprintf(fp, "dayhours         %8s\n", dayhours->string);
		fprintf(fp, "easymode         %8s\n", easymode->string);
		fprintf(fp, "emulatecollision %8s\n", emulatecollision->string);
		fprintf(fp, "enemyidlim       %8s\n", enemyidlim->string);
		fprintf(fp, "enemynames       %8s\n", enemynames->string);
		fprintf(fp, "enemyplanes      %8s\n", enemyplanes->string);
		fprintf(fp, "flakmax          %8s\n", flakmax->string);
		fprintf(fp, "flypenalty       %8s\n", flypenalty->string);
		fprintf(fp, "friendlyfire     %8s\n", friendlyfire->string);
		fprintf(fp, "friendlyidlim    %8s\n", friendlyidlim->string);
		fprintf(fp, "fueldiv          %8s\n", fueldiv->string);
		fprintf(fp, "gunstats         %8s\n", gunstats->string);
		fprintf(fp, "iff              %8s\n", iff->string);
		fprintf(fp, "katyrange        %8s\n", katyrange->string);
		fprintf(fp, "mapname          %8s\n", mapname->string);
		fprintf(fp, "midairs          %8s\n", midairs->string);
		fprintf(fp, "mortars          %8s\n", mortars->string);
		fprintf(fp, "mview            %8s\n", mview->string);
		fprintf(fp, "ottoaccuracy     %8s\n", ottoaccuracy->string);
		fprintf(fp, "ottoadjust       %8s\n", ottoadjust->string);
		fprintf(fp, "ottoburstoff     %8s\n", ottoburstoff->string);
		fprintf(fp, "ottoburston      %8s\n", ottoburston->string);
		fprintf(fp, "ottoburstonmax   %8s\n", ottoburstonmax->string);
		fprintf(fp, "ottorange        %8s\n", ottorange->string);
		fprintf(fp, "ottoretarget     %8s\n", ottoretarget->string);
		fprintf(fp, "ottooverrides    %8s\n", ottooverrides->string);
		fprintf(fp, "planerangelimit  %8s\n", planerangelimit->string);
		fprintf(fp, "radaralt         %8s\n", radaralt->string);
		fprintf(fp, "radarrange1      %8s\n", radarrange1->string);
		fprintf(fp, "radarrange3      %8s\n", radarrange3->string);
		fprintf(fp, "rebuildtime      %8s\n", rebuildtime->string);
		fprintf(fp, "rps              %8s\n", rps->string);
		fprintf(fp, "tanksrange       %8s\n", tanksrange->string);
		fprintf(fp, "teamkiller       %8s\n", teamkiller->string);
		fprintf(fp, "timemult         %8s\n", timemult->string);
		fprintf(fp, "timeout          %8s\n", timeout->string);
		fprintf(fp, "whitelist        %8s\n", whitelist->string);

		fclose(fp);

		SendFileSeq1(filename, "configs.asc", client);
	}

	Sys_UnlockFile(strcat(filename, ".LOCK"));
}

/*************
 Cmd_Score

 Send score to player
 *************/

void Cmd_Score(char *player, client_t *client)
{
	u_int16_t h;
	u_int8_t i, m, s;
	int16_t num_rows;
	u_int32_t ftime, player_id, debug_querytime;
	char filename[128];
	char nickname[7];
	float ratio1, ratio2, ratio3, ratio4, csortie;
	FILE *fp;
	MYSQL_RES *temp_result= NULL;
	MYSQL_ROW temp_row= NULL;

	memset(filename, 0, sizeof(filename));
	memset(nickname, 0, sizeof(nickname));

	strncpy(nickname, player ? player : client->longnick, 6);

	sprintf(filename, "./players/%s.score.LOCK", nickname);

	Sys_WaitForLock(filename);

	if (Sys_LockFile(filename) < 0)
		return;

	filename[strlen(filename) - 5] = '\0';

	if ((fp = fopen(filename, "wb")))
	{
		player_id = 0;

		if (strcmp(nickname, client->longnick)) // if player want someone else's score 
		{
			debug_querytime = Sys_Milliseconds();

			sprintf(my_query, "SELECT id FROM players WHERE longnick = '%s'", nickname);

			if (!Com_MySQL_Query(client, &my_sock, my_query))
			{
				Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[1]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((my_row = mysql_fetch_row(my_result)))
					{
						player_id = Com_Atou(Com_MyRow("id"));
					}
					else
					{
						if (!mysql_errno(&my_sock))
						{
							PPrintf(client, RADIO_YELLOW, "Player %s doesn't exists", nickname);
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Cmd_Score(id): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							PPrintf(client, RADIO_YELLOW, "Cmd_Score(id): Couldn't Fetch Row, please contact admin");
						}
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Score(id): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					PPrintf(client, RADIO_YELLOW, "Cmd_Score(id, NULL): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
				}
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Score(id): Query error error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				PPrintf(client, RADIO_YELLOW, "Cmd_Score(id): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
			}
		}
		else // or player wants his own
		{
			player_id = client->id;
		}

		if (player_id)
		{
			debug_querytime = Sys_Milliseconds();

			/*******************************************************
			 Variables: nickname, countrynumber, percentsorties, squadname, flighttime, totalscore, lastscore
			 
			 players.country, score_common.flyred, score_common.flygold, score_common.flighttime, score_common.totalscore, score_common.lastscore, squads.name
			 ********************************************************/

			sprintf(
					my_query,
					"SELECT players.country, players.rank, score_common.flyred, score_common.flygold, score_common.flighttime, score_common.totalscore, score_common.lastscore, squads.name FROM players INNER JOIN score_common ON players.id = score_common.player_id LEFT JOIN squads ON players.squad_owner = squads.owner WHERE players.id = '%u'",
					player_id);

			if (!(player_id == client->id ? Com_MySQL_Query(client, &my_sock, my_query) : d_mysql_query(&my_sock, my_query)))
			{
				Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[2]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);

				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((my_row = mysql_fetch_row(my_result)))
					{
						csortie = Com_Atou(Com_MyRow("flyred")) + Com_Atou(Com_MyRow("flygold"));

						if (Com_Atoi(Com_MyRow("country")) == 1)
						{
							csortie = Com_Atof(Com_MyRow("flyred")) / csortie;
						}
						else if (Com_Atoi(Com_MyRow("country")) == 3)
						{
							csortie = Com_Atof(Com_MyRow("flygold")) / csortie;
						}

						csortie *= 100;

						ftime = Com_Atou(Com_MyRow("flighttime"));

						h = m = s = 0;

						if (ftime / 3600)
						{
							h = ftime / 3600;

							ftime = ftime - (3600 * h);
						}
						if (ftime / 60)
						{
							m = ftime / 60;

							ftime = ftime - (60 * m);
						}
						if (ftime)
						{
							s = ftime;
						}

						fprintf(fp, "======================\n");
						fprintf(fp, "Game ID: %s\nFlying for: %s (%.2f%% of sorties)\n\nSquad: %s\n\n", nickname, GetCountry(Com_Atoi(Com_MyRow("country"))), csortie,
								Com_MyRow("name") ? Com_MyRow("name") : "None");
						fprintf(fp, "FLIGHT TIME: %uh %um %us\nTOTAL SCORE: %10.3f\n\n", h, m, s, Com_Atof(Com_MyRow("totalscore")));
						fprintf(fp, "Last mission: %8.3f\n", Com_Atof(Com_MyRow("lastscore")));
						fprintf(fp, "Elo Rating: %u\n", Com_Atou(Com_MyRow("rank")));
						fprintf(fp, "======================\n\n");
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;

					/*********************************************************************/

					if (!mysql_errno(&my_sock))
					{
						debug_querytime = Sys_Milliseconds();
						sprintf(my_query, "SELECT * FROM score_fighter WHERE player_id = '%u'", player_id);

						if (!d_mysql_query(&my_sock, my_query))
						{
							Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[4]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
							if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
							{
								if ((my_row = mysql_fetch_row(my_result)))
								{
									if (Com_Atof(Com_MyRow("fighter_score")) || Com_Atof(Com_MyRow("jabo_score")) || Com_Atof(Com_MyRow("capt_score")) || Com_Atof(Com_MyRow("cost_score")))
									{
										fprintf(fp, "(+) FIGHTER score: %10.3f\n", Com_Atof(Com_MyRow("fighter_score")));
										fprintf(fp, "(+) JABO    score: %10.3f\n", Com_Atof(Com_MyRow("jabo_score")));
										fprintf(fp, "(+) CAPTURE score: %10.3f\n", Com_Atof(Com_MyRow("capt_score")));
										fprintf(fp, "(-) COSTS   score: %10.3f\n", Com_Atof(Com_MyRow("cost_score")));
										fprintf(fp, "(=) TOTAL   score: %10.3f\n", Com_Atof(Com_MyRow("fighter_score")) + Com_Atof(Com_MyRow("jabo_score")) + Com_Atof(Com_MyRow("capt_score")) - Com_Atof(Com_MyRow("cost_score")));

										debug_querytime = Sys_Milliseconds();
										sprintf(
												my_query,
												"SELECT players.longnick \
															FROM players \
															RIGHT JOIN score_kills ON players.id = score_kills.victim_id \
															WHERE score_kills.player_id = '%u' AND score_kills.player_pltype = '1' \
															ORDER BY score_kills.id DESC LIMIT 6",
												player_id);

										fprintf(fp, "\nLast Air Victories:   ");
										if (!d_mysql_query(&my_sock, my_query))
										{
											Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[5]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
											if ((temp_result = mysql_store_result(&my_sock))) // returned a non-NULL value
											{
												if ((num_rows = mysql_num_rows(temp_result)) > 0)
												{
													for (i = 0; i < num_rows; i++)
													{
														if ((temp_row = mysql_fetch_row(temp_result)))
														{
															fprintf(fp, " %s", temp_row[0] ? temp_row[0] : "-HOST-");
														}
														else
														{
															Com_Printf(VERBOSE_WARNING, "Cmd_Score(fikills): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
														}
													}
												}

												mysql_free_result(temp_result);
												temp_result = NULL;
												temp_row = NULL;
											}
											else
											{
												Com_Printf(VERBOSE_WARNING, "Cmd_Score(fikills): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
											}
										}
										else
										{
											Com_Printf(VERBOSE_WARNING, "Cmd_Score(fikills): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
										}

										debug_querytime = Sys_Milliseconds();

										sprintf(
												my_query,
												"SELECT players.longnick \
															FROM players \
															RIGHT JOIN score_kills ON players.id = score_kills.player_id \
															WHERE score_kills.victim_id = '%u' AND score_kills.victim_pltype = '1' \
															ORDER BY score_kills.id DESC LIMIT 6",
												player_id);

										fprintf(fp, "\nLast Air Defeats for: ");
										if (!d_mysql_query(&my_sock, my_query))
										{
											Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[6]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
											if ((temp_result = mysql_store_result(&my_sock))) // returned a non-NULL value
											{
												if ((num_rows = mysql_num_rows(temp_result)) > 0)
												{
													for (i = 0; i < num_rows; i++)
													{
														if ((temp_row = mysql_fetch_row(temp_result)))
														{
															fprintf(fp, " %s", temp_row[0] ? temp_row[0] : "-HOST-");
														}
														else
														{
															Com_Printf(VERBOSE_WARNING, "Cmd_Score(fikilleds): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
														}
													}
												}

												mysql_free_result(temp_result);
												temp_result = NULL;
												temp_row = NULL;
											}
											else
											{
												Com_Printf(VERBOSE_WARNING, "Cmd_Score(fikilleds): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
											}
										}
										else
										{
											Com_Printf(VERBOSE_WARNING, "Cmd_Score(fikilleds): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
										}

										ratio1 = Com_Atou(Com_MyRow("gunhits"));
										if (Com_Atou(Com_MyRow("gunused")))
											ratio1 /= Com_Atou(Com_MyRow("gunused"));
										else
											ratio1 = 0;

										ratio2 = Com_Atou(Com_MyRow("bombhits"));
										if (Com_Atou(Com_MyRow("bombused")))
											ratio2 /= Com_Atou(Com_MyRow("bombused"));
										else
											ratio2 = 0;

										ratio3 = Com_Atou(Com_MyRow("rockethits"));
										if (Com_Atou(Com_MyRow("rocketused")))
											ratio3 /= Com_Atou(Com_MyRow("rocketused"));
										else
											ratio3 = 0;

										fprintf(fp, "\n\nSorties : %4u  Planes Downed      : %4u\n", Com_Atou(Com_MyRow("sorties")), Com_Atou(Com_MyRow("kills")));
										fprintf(fp, "Landed  : %4u  Tank Destroyed     : %4u\n", Com_Atou(Com_MyRow("landed")), Com_Atoi(Com_MyRow("killtank")));
										fprintf(fp, "Ditched : %4u  Assists            : %4u\n", Com_Atou(Com_MyRow("ditched")), Com_Atou(Com_MyRow("assists")));
										fprintf(fp, "Bailed  : %4u  Current Streak     : %4u\n", Com_Atou(Com_MyRow("bailed")), Com_Atou(Com_MyRow("curr_streak")));
										fprintf(fp, "Captured: %4u  Longest Streak     : %4u\n", Com_Atou(Com_MyRow("captured")), Com_Atou(Com_MyRow("long_streak")));
										fprintf(fp, "Killed  : %4u  Collided           : %4u\n\n", Com_Atou(Com_MyRow("killed")), Com_Atou(Com_MyRow("collided")));
										fprintf(fp, "                Network Discos     : %4d\n", Com_Atou(Com_MyRow("disco")));
										fprintf(fp, "                V-1 Down           : %4d\n", Com_Atoi(Com_MyRow("killfau")));
										fprintf(fp, "                Hmack Destroyed    : %4d\n", Com_Atoi(Com_MyRow("killhmack")));
										fprintf(fp, "                Katyusha Destroyed : %4d\n", Com_Atoi(Com_MyRow("killkaty")));
										fprintf(fp, "                Commandos Killed   : %4d\n", Com_Atoi(Com_MyRow("killcommandos")));
										fprintf(fp, "                Acks Destroyed     : %4d\n", Com_Atoi(Com_MyRow("acks")));
										fprintf(fp, "                Buildings Destroyed: %4d\n", Com_Atoi(Com_MyRow("buildings")));
										fprintf(fp, "                Ships Sunk         : %4d\n", Com_Atoi(Com_MyRow("ships")));
										fprintf(fp, "                CVs Sunk           : %4d\n\n", Com_Atoi(Com_MyRow("cvs")));
										fprintf(fp, "ACCURACY\n\n         GUNS    BOMBS  ROCKETS\n");
										fprintf(fp, "HIT  :    %4u    %4u    %4u\n", Com_Atou(Com_MyRow("gunhits")), Com_Atou(Com_MyRow("bombhits")), Com_Atou(Com_MyRow("rockethits")));
										fprintf(fp, "USED :    %4u    %4u    %4u\n", Com_Atou(Com_MyRow("gunused")), Com_Atou(Com_MyRow("bombused")), Com_Atou(Com_MyRow("rocketused")));
										fprintf(fp, "RATIO:   %.3f   %.3f   %.3f\n", ratio1, ratio2, ratio3);
										fprintf(fp, "=========================================\n\n");
									}
								}

								mysql_free_result(my_result);
								my_result = NULL;
								my_row = NULL;

								if (!mysql_errno(&my_sock))
								{
									debug_querytime = Sys_Milliseconds();
									sprintf(my_query, "SELECT * FROM score_bomber WHERE player_id = '%u'", player_id);

									if (!d_mysql_query(&my_sock, my_query))
									{
										Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[7]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
										if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
										{
											if ((my_row = mysql_fetch_row(my_result)))
											{
												if (Com_Atof(Com_MyRow("bomber_score")) || Com_Atof(Com_MyRow("capt_score")) || Com_Atof(Com_MyRow("rescue_score")) || Com_Atof(Com_MyRow("cost_score")))
												{
													fprintf(fp, "(+) BOMBER  score: %10.3f\n", Com_Atof(Com_MyRow("bomber_score")));
													fprintf(fp, "(+) CAPTURE score: %10.3f\n", Com_Atof(Com_MyRow("capt_score")));
													fprintf(fp, "(+) RESCUE  score: %10.3f\n", Com_Atof(Com_MyRow("rescue_score")));
													fprintf(fp, "(-) COSTS   score: %10.3f\n", Com_Atof(Com_MyRow("cost_score")));
													fprintf(fp, "(=) TOTAL   score: %10.3f\n", Com_Atof(Com_MyRow("bomber_score")) + Com_Atof(Com_MyRow("rescue_score")) + Com_Atof(Com_MyRow("capt_score")) - Com_Atof(Com_MyRow("cost_score")));

													debug_querytime = Sys_Milliseconds();

													sprintf(
															my_query,
															"SELECT players.longnick \
																		FROM players \
																		RIGHT JOIN score_kills ON players.id = score_kills.victim_id \
																		WHERE score_kills.player_id = '%u' AND score_kills.player_pltype = '2' \
																		ORDER BY score_kills.id DESC LIMIT 6",
															player_id);

													fprintf(fp, "\nLast Air Victories:   ");
													if (!d_mysql_query(&my_sock, my_query))
													{
														Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[8]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
														if ((temp_result = mysql_store_result(&my_sock))) // returned a non-NULL value
														{
															if ((num_rows = mysql_num_rows(temp_result)) > 0)
															{
																for (i = 0; i < num_rows; i++)
																{
																	if ((temp_row = mysql_fetch_row(temp_result)))
																	{
																		fprintf(fp, " %s", temp_row[0] ? temp_row[0] : "-HOST-");
																	}
																	else
																	{
																		Com_Printf(VERBOSE_WARNING, "Cmd_Score(bokills): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																	}
																}
															}

															mysql_free_result(temp_result);
															temp_result = NULL;
															temp_row = NULL;
														}
														else
														{
															Com_Printf(VERBOSE_WARNING, "Cmd_Score(bokills): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
														}
													}
													else
													{
														Com_Printf(VERBOSE_WARNING, "Cmd_Score(bokills): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
													}

													debug_querytime = Sys_Milliseconds();

													sprintf(
															my_query,
															"SELECT players.longnick \
																		FROM players \
																		RIGHT JOIN score_kills ON players.id = score_kills.player_id \
																		WHERE score_kills.victim_id = '%u' AND score_kills.victim_pltype = '2' \
																		ORDER BY score_kills.id DESC LIMIT 6",
															player_id);

													fprintf(fp, "\nLast Air Defeats for: ");
													if (!d_mysql_query(&my_sock, my_query))
													{
														Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[9]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
														if ((temp_result = mysql_store_result(&my_sock))) // returned a non-NULL value
														{
															if ((num_rows = mysql_num_rows(temp_result)) > 0)
															{
																for (i = 0; i < num_rows; i++)
																{
																	if ((temp_row = mysql_fetch_row(temp_result)))
																	{
																		fprintf(fp, " %s", temp_row[0] ? temp_row[0] : "-HOST-");
																	}
																	else
																	{
																		Com_Printf(VERBOSE_WARNING, "Cmd_Score(bokilleds): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																	}
																}
															}

															mysql_free_result(temp_result);
															temp_result = NULL;
															temp_row = NULL;
														}
														else
														{
															Com_Printf(VERBOSE_WARNING, "Cmd_Score(bokilleds): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
														}
													}
													else
													{
														Com_Printf(VERBOSE_WARNING, "Cmd_Score(bokilleds): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
													}

													ratio1 = Com_Atou(Com_MyRow("gunhits"));
													if (Com_Atou(Com_MyRow("gunused")))
														ratio1 /= Com_Atou(Com_MyRow("gunused"));
													else
														ratio1 = 0;

													ratio2 = Com_Atou(Com_MyRow("bombhits"));
													if (Com_Atou(Com_MyRow("bombused")))
														ratio2 /= Com_Atou(Com_MyRow("bombused"));
													else
														ratio2 = 0;

													ratio3 = Com_Atou(Com_MyRow("rockethits"));
													if (Com_Atou(Com_MyRow("rocketused")))
														ratio3 /= Com_Atou(Com_MyRow("rocketused"));
													else
														ratio3 = 0;

													ratio4 = Com_Atou(Com_MyRow("torphits"));
													if (Com_Atou(Com_MyRow("torpused")))
														ratio4 /= Com_Atou(Com_MyRow("torpused"));
													else
														ratio4 = 0;

													fprintf(fp, "\n\nSorties : %4u  Planes Downed      : %4u\n", Com_Atou(Com_MyRow("sorties")), Com_Atou(Com_MyRow("kills")));
													fprintf(fp, "Landed  : %4u  Tank Destroyed     : %4u\n", Com_Atou(Com_MyRow("landed")), Com_Atoi(Com_MyRow("killtank")));
													fprintf(fp, "Ditched : %4u  Assists            : %4u\n", Com_Atou(Com_MyRow("ditched")), Com_Atou(Com_MyRow("assists")));
													fprintf(fp, "Bailed  : %4u  Current Streak     : %4u\n", Com_Atou(Com_MyRow("bailed")), Com_Atou(Com_MyRow("curr_streak")));
													fprintf(fp, "Captured: %4u  Longest Streak     : %4u\n", Com_Atou(Com_MyRow("captured")), Com_Atou(Com_MyRow("long_streak")));
													fprintf(fp, "Killed  : %4u  Collided           : %4u\n\n", Com_Atou(Com_MyRow("killed")), Com_Atou(Com_MyRow("collided")));
													fprintf(fp, "                Network Discos     : %4d\n", Com_Atou(Com_MyRow("disco")));
													fprintf(fp, "                Rescued Comrades   : %4d\n", Com_Atoi(Com_MyRow("rescue")));
													fprintf(fp, "                Hmack Destroyed    : %4d\n", Com_Atoi(Com_MyRow("killhmack")));
													fprintf(fp, "                Katyusha Destroyed : %4d\n", Com_Atoi(Com_MyRow("killkaty")));
													fprintf(fp, "                Commandos Killed   : %4d\n", Com_Atoi(Com_MyRow("killcommandos")));
													fprintf(fp, "                Acks Destroyed     : %4d\n", Com_Atoi(Com_MyRow("acks")));
													fprintf(fp, "                Buildings Destroyed: %4d\n", Com_Atoi(Com_MyRow("buildings")));
													fprintf(fp, "                Fields Captured    : %4d\n", Com_Atoi(Com_MyRow("fieldscapt")));
													fprintf(fp, "                Ships Sunk         : %4d\n", Com_Atoi(Com_MyRow("ships")));
													fprintf(fp, "                CVs Sunk           : %4d\n\n", Com_Atoi(Com_MyRow("cvs")));
													fprintf(fp, "ACCURACY\n\n         GUNS    BOMBS  ROCKETS  TORPEDOES\n");
													fprintf(fp, "HIT  :    %4u    %4u    %4u    %4u\n", Com_Atou(Com_MyRow("gunhits")), Com_Atou(Com_MyRow("bombhits")),
															Com_Atou(Com_MyRow("rockethits")), Com_Atou(Com_MyRow("torphits")));
													fprintf(fp, "USED :    %4u    %4u    %4u    %4u\n", Com_Atou(Com_MyRow("gunused")), Com_Atou(Com_MyRow("bombused")),
															Com_Atou(Com_MyRow("rocketused")), Com_Atou(Com_MyRow("torpused")));
													fprintf(fp, "RATIO:   %.3f   %.3f   %.3f   %.3f\n", ratio1, ratio2, ratio3, ratio4);
													fprintf(fp, "=========================================\n\n");
												}
											}

											mysql_free_result(my_result);
											my_result = NULL;
											my_row = NULL;

											if (!mysql_errno(&my_sock))
											{
												debug_querytime = Sys_Milliseconds();
												sprintf(my_query, "SELECT * FROM score_ground WHERE player_id = '%u'", player_id);

												if (!d_mysql_query(&my_sock, my_query))
												{
													Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[10]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
													if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
													{
														if ((my_row = mysql_fetch_row(my_result)))
														{
															if (Com_Atof(Com_MyRow("ground_score")) || Com_Atof(Com_MyRow("capt_score")) || Com_Atof(Com_MyRow("cost_score")) || Com_Atof(Com_MyRow("rescue_score")))
															{
																fprintf(fp, "(+) GROUND  score: %10.3f\n", Com_Atof(Com_MyRow("ground_score")));
																if (wb3->value)
																{
																	fprintf(fp, "(+) CAPTURE score: %10.3f\n", Com_Atof(Com_MyRow("capt_score")));
																	fprintf(fp, "(+) RESCUE  score: %10.3f\n", Com_Atof(Com_MyRow("rescue_score")));
																	fprintf(fp, "(-) COSTS   score: %10.3f\n", Com_Atof(Com_MyRow("cost_score")));
																	fprintf(fp, "(=) TOTAL   score: %10.3f\n", Com_Atof(Com_MyRow("ground_score")) + Com_Atof(Com_MyRow("rescue_score")) + Com_Atof(Com_MyRow("capt_score")) - Com_Atof(Com_MyRow("cost_score")));
																}

																debug_querytime = Sys_Milliseconds();

																sprintf(
																		my_query,
																		"SELECT players.longnick \
																					FROM players \
																					RIGHT JOIN score_kills ON players.id = score_kills.victim_id \
																					WHERE score_kills.player_id = '%u' AND score_kills.player_pltype = '3' \
																					ORDER BY score_kills.id DESC LIMIT 6",
																		player_id);

																fprintf(fp, "\nLast Air Victories:   ");
																if (!d_mysql_query(&my_sock, my_query))
																{
																	Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[11]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
																	if ((temp_result = mysql_store_result(&my_sock))) // returned a non-NULL value
																	{
																		if ((num_rows = mysql_num_rows(temp_result)) > 0)
																		{
																			for (i = 0; i < num_rows; i++)
																			{
																				if ((temp_row = mysql_fetch_row(temp_result)))
																				{
																					fprintf(fp, " %s", temp_row[0] ? temp_row[0] : "-HOST-");
																				}
																				else
																				{
																					Com_Printf(VERBOSE_WARNING, "Cmd_Score(grkills): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock),
																							mysql_error(&my_sock));
																				}
																			}
																		}

																		mysql_free_result(temp_result);
																		temp_result = NULL;
																		temp_row = NULL;
																	}
																	else
																	{
																		Com_Printf(VERBOSE_WARNING, "Cmd_Score(grkills): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																	}
																}
																else
																{
																	Com_Printf(VERBOSE_WARNING, "Cmd_Score(grkills): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																}

																debug_querytime = Sys_Milliseconds();

																sprintf(
																		my_query,
																		"SELECT players.longnick \
																					FROM players \
																					RIGHT JOIN score_kills ON players.id = score_kills.player_id \
																					WHERE score_kills.victim_id = '%u' AND score_kills.victim_pltype = '3' \
																					ORDER BY score_kills.id DESC LIMIT 6",
																		player_id);

																fprintf(fp, "\nLast Air Defeats for: ");
																if (!d_mysql_query(&my_sock, my_query))
																{
																	Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[12]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
																	if ((temp_result = mysql_store_result(&my_sock))) // returned a non-NULL value
																	{
																		if ((num_rows = mysql_num_rows(temp_result)) > 0)
																		{
																			for (i = 0; i < num_rows; i++)
																			{
																				if ((temp_row = mysql_fetch_row(temp_result)))
																				{
																					fprintf(fp, " %s", temp_row[0] ? temp_row[0] : "-HOST-");
																				}
																				else
																				{
																					Com_Printf(VERBOSE_WARNING, "Cmd_Score(grkilleds): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock),
																							mysql_error(&my_sock));
																				}
																			}
																		}

																		mysql_free_result(temp_result);
																		temp_result = NULL;
																		temp_row = NULL;
																	}
																	else
																	{
																		Com_Printf(VERBOSE_WARNING, "Cmd_Score(grkilleds): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																	}
																}
																else
																{
																	Com_Printf(VERBOSE_WARNING, "Cmd_Score(grkilleds): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																}

																ratio1 = Com_Atou(Com_MyRow("gunhits"));
																if (Com_Atou(Com_MyRow("gunused")))
																	ratio1 /= Com_Atou(Com_MyRow("gunused"));
																else
																	ratio1 = 0;

																fprintf(fp, "\n\nSorties : %4u  Planes Downed      : %4u\n", Com_Atou(Com_MyRow("sorties")), Com_Atou(Com_MyRow("kills")));
																fprintf(fp, "Ended   : %4u  Tank Destroyed     : %4u\n", Com_Atou(Com_MyRow("landed")), Com_Atoi(Com_MyRow("killtank")));
																fprintf(fp, "Ditched : %4u  Assists            : %4u\n", Com_Atou(Com_MyRow("ditched")), Com_Atou(Com_MyRow("assists")));
																fprintf(fp, "Captured: %4u  Current Streak     : %4u\n", Com_Atou(Com_MyRow("captured")), Com_Atou(Com_MyRow("curr_streak")));
																fprintf(fp, "Killed  : %4u  Longest Streak     : %4u\n", Com_Atou(Com_MyRow("killed")), Com_Atou(Com_MyRow("long_streak")));
																fprintf(fp, "                Collided           : %4u\n\n", Com_Atou(Com_MyRow("collided")));
																fprintf(fp, "                Network Discos     : %4u\n\n", Com_Atou(Com_MyRow("disco")));
																fprintf(fp, "                Rescued Comrades   : %4d\n", Com_Atoi(Com_MyRow("rescue")));
																fprintf(fp, "                Hmack Destroyed    : %4d\n", Com_Atoi(Com_MyRow("killhmack")));
																fprintf(fp, "                Katyusha Destroyed : %4d\n", Com_Atoi(Com_MyRow("killkaty")));
																fprintf(fp, "                Commandos Killed   : %4d\n", Com_Atoi(Com_MyRow("killcommandos")));
																fprintf(fp, "                Acks Destroyed     : %4d\n", Com_Atoi(Com_MyRow("acks")));
																fprintf(fp, "                Buildings Destroyed: %4d\n", Com_Atoi(Com_MyRow("buildings")));
																fprintf(fp, "                Fields Captured    : %4d\n", Com_Atoi(Com_MyRow("fieldscapt")));
																fprintf(fp, "                Ships Sunk         : %4d\n", Com_Atoi(Com_MyRow("ships")));
																fprintf(fp, "                CVs Sunk           : %4d\n\n", Com_Atoi(Com_MyRow("cvs")));
																fprintf(fp, "ACCURACY\n\n         GUNS\n");
																fprintf(fp, "HIT  :    %4u\n", Com_Atou(Com_MyRow("gunhits")));
																fprintf(fp, "USED :    %4u\n", Com_Atou(Com_MyRow("gunused")));
																fprintf(fp, "RATIO:   %.3f\n", ratio1);
																fprintf(fp, "=========================================\n\n");
															}
														}

														mysql_free_result(my_result);
														my_result = NULL;
														my_row = NULL;

														if (!mysql_errno(&my_sock))
														{
															debug_querytime = Sys_Milliseconds();
															sprintf(my_query, "SELECT * FROM score_penalty WHERE player_id = '%u'", player_id);

															if (!d_mysql_query(&my_sock, my_query))
															{
																Com_Printf(VERBOSE_DEBUG, "Cmd_Score() Query[13]: %u milliseconds\n", Sys_Milliseconds() - debug_querytime);
																if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
																{
																	if ((my_row = mysql_fetch_row(my_result)))
																	{
																		if (Com_Atof(Com_MyRow("penalty_score")))
																		{
																			fprintf(fp, "      *** FRIENDLY CASUALITIES ***\n");
																			fprintf(fp, "===(will be debited from Total score)===\n");
																			fprintf(fp, "PENALTY            %10.3f\n\n", Com_Atof(Com_MyRow("penalty_score")));

																			fprintf(fp, "Friendly Planes Downed             : %4u\n", Com_Atou(Com_MyRow("kills")));
																			fprintf(fp, "Friendly Tank Destroyed            : %4u\n", Com_Atou(Com_MyRow("killtank")));
																			fprintf(fp, "Friendly V-1 Downed                : %4u\n", Com_Atou(Com_MyRow("killfau")));
																			fprintf(fp, "Friendly Hmack Destroyed           : %4u\n", Com_Atou(Com_MyRow("killhmack")));
																			fprintf(fp, "Friendly Katyusha Destroyed        : %4u\n", Com_Atou(Com_MyRow("killkaty")));
																			fprintf(fp, "Friendly Commandos Killed          : %4u\n", Com_Atou(Com_MyRow("killcommandos")));
																			fprintf(fp, "Friendly Acks Destroyed            : %4u\n", Com_Atou(Com_MyRow("acks")));
																			fprintf(fp, "Friendly Buildings Destroyed       : %4u\n", Com_Atou(Com_MyRow("buildings")));
																			fprintf(fp, "Friendly Ships Sunk                : %4u\n", Com_Atou(Com_MyRow("ships")));
																			fprintf(fp, "Friendly CVs Sunk                  : %4u\n\n", Com_Atou(Com_MyRow("cvs")));
																		}
																	}

																	mysql_free_result(my_result);
																	my_result = NULL;
																	my_row = NULL;

																	if (!mysql_errno(&my_sock))
																	{
																		fprintf(fp, "                   Score System by Sonicx\n");
																		fprintf(fp, "==========\n");

																		SendFileSeq1(filename, "scores.asc", client);
																	}
																	else
																	{
																		Com_Printf(VERBOSE_WARNING, "Cmd_Score(penalty): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																		PPrintf(client, RADIO_YELLOW, "Cmd_Score(penalty): Couldn't Fetch Row, please contact admin");
																	}
																}
																else
																{
																	Com_Printf(VERBOSE_WARNING, "Cmd_Score(penalty): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																	PPrintf(client, RADIO_YELLOW, "Cmd_Score(penalty, NULL): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
																}
															}
															else
															{
																Com_Printf(VERBOSE_WARNING, "Cmd_Score(penalty): Query error error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
																PPrintf(client, RADIO_YELLOW, "Cmd_Score(penalty): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
															}
														}
														else
														{
															Com_Printf(VERBOSE_WARNING, "Cmd_Score(ground): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
															PPrintf(client, RADIO_YELLOW, "Cmd_Score(ground): Couldn't Fetch Row, please contact admin");
														}
													}
													else
													{
														Com_Printf(VERBOSE_WARNING, "Cmd_Score(ground): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
														PPrintf(client, RADIO_YELLOW, "Cmd_Score(ground, NULL): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
													}
												}
												else
												{
													Com_Printf(VERBOSE_WARNING, "Cmd_Score(ground): Query error error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
													PPrintf(client, RADIO_YELLOW, "Cmd_Score(ground): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
												}

											}
											else
											{
												Com_Printf(VERBOSE_WARNING, "Cmd_Score(bomber): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
												PPrintf(client, RADIO_YELLOW, "Cmd_Score(bomber): Couldn't Fetch Row, please contact admin");
											}
										}
										else
										{
											Com_Printf(VERBOSE_WARNING, "Cmd_Score(bomber): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
											PPrintf(client, RADIO_YELLOW, "Cmd_Score(bomber, NULL): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
										}

									}
									else
									{
										Com_Printf(VERBOSE_WARNING, "Cmd_Score(bomber): Query error error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
										PPrintf(client, RADIO_YELLOW, "Cmd_Score(bomber): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
									}
								}
								else
								{
									Com_Printf(VERBOSE_WARNING, "Cmd_Score(fighter): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
									PPrintf(client, RADIO_YELLOW, "Cmd_Score(fighter): Couldn't Fetch Row, please contact admin");
								}
							}
							else
							{
								Com_Printf(VERBOSE_WARNING, "Cmd_Score(fighter): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
								PPrintf(client, RADIO_YELLOW, "Cmd_Score(fighter, NULL): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
							}
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Cmd_Score(fighter): Query error error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							PPrintf(client, RADIO_YELLOW, "Cmd_Score(fighter): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
						}
					}
					else
					{
						/*********************************************************************/
						Com_Printf(VERBOSE_WARNING, "Cmd_Score(header): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						PPrintf(client, RADIO_YELLOW, "Cmd_Score(header): Couldn't Fetch Row, please contact admin");
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Score(header): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					PPrintf(client, RADIO_YELLOW, "Cmd_Score(header, NULL): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
				}
			}
			else
			{
				if (mysql_errno(&my_sock))
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Score(header): Query error error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					PPrintf(client, RADIO_YELLOW, "Cmd_Score(header): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
				}
			}
		}
		else
		{
			unlink(filename);
			PPrintf(client, RADIO_YELLOW, "Player not found");
		}

		fclose(fp);
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Cmd_Score(): Couldn't open file \"%s\"\n", filename);
		PPrintf(client, RADIO_YELLOW, "Cmd_Score(): Couldn't open score file, please contact admin", mysql_errno(&my_sock));
	}

	Sys_UnlockFile(strcat(filename, ".LOCK"));
}

/*************
 Cmd_Clear

 Clear client's score
 *************/

void Cmd_Clear(client_t *client)
{
	sprintf(my_query, "DELETE FROM score_fighter WHERE player_id = '%u'", client->id);
	sprintf(my_query, "%s; DELETE FROM score_bomber WHERE player_id = '%u'", my_query, client->id);
	sprintf(my_query, "%s; DELETE FROM score_ground WHERE player_id = '%u'", my_query, client->id);
	sprintf(my_query, "%s; DELETE FROM score_penalty WHERE player_id = '%u'", my_query, client->id);
	sprintf(my_query, "%s; DELETE FROM score_common WHERE player_id = '%u'", my_query, client->id);
	sprintf(my_query, "%s; DELETE FROM medals WHERE player_id = '%u'", my_query, client->id);
	//??	sprintf(my_query, "%s; DELETE FROM score_kills WHERE player_id = '%u'", my_query, client->id);

	sprintf(my_query, "%s; INSERT INTO score_fighter (player_id) VALUES ('%u')", my_query, client->id);
	sprintf(my_query, "%s; INSERT INTO score_bomber (player_id) VALUES ('%u')", my_query, client->id);
	sprintf(my_query, "%s; INSERT INTO score_ground (player_id) VALUES ('%u')", my_query, client->id);
	sprintf(my_query, "%s; INSERT INTO score_penalty (player_id) VALUES ('%u')", my_query, client->id);
	sprintf(my_query, "%s; INSERT INTO score_common (player_id) VALUES ('%u')", my_query, client->id);

	if (mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_ON))
		Com_Printf(VERBOSE_ERROR, "%d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

	if (!Com_MySQL_Query(client, &my_sock, my_query))
	{
		client->lastscore = client->streakscore = client->killstod = client->structstod = client->ranking = 0;

		Com_MySQL_Flush(&my_sock, __FILE__, __LINE__);

		PPrintf(client, RADIO_LIGHTYELLOW, "Your score is cleared");
		Com_Printf(VERBOSE_DEBUG, "Score cleared\n");
	}
	else
	{
		if(mysql_errno(&my_sock))
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_Clear(): couldn't query DELETE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}

#ifdef  _WIN32
	sleep(140);
#else			
	usleep(140000);
#endif	

	mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
}

/*************
 Cmd_Whoare

 Prints who are hearing given channel
 *************/

void Cmd_Whoare(u_int8_t radio, client_t *client)
{
	u_int8_t i;

	if (!radio || radio > 99)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid radio channel");
		return;
	}

	PPrintf(client, RADIO_YELLOW, "Who are in channel %d:", radio);

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone && clients[i].ready && client != &clients[i])
		{
			if (radio < 50)
			{
				if (clients[i].radio[0] == radio && clients[i].country == client->country)
				{
					PPrintf(client, RADIO_YELLOW, "%s, country %s", clients[i].longnick, GetCountry(clients[i].country));
				}
			}
			else
			{
				if (clients[i].radio[0] == radio)
				{
					PPrintf(client, RADIO_YELLOW, "%s, country %s", clients[i].longnick, GetCountry(clients[i].country));
				}
			}
		}
	}
}

/*************
 Cmd_Invite

 Invite a player to join client's squadron
 *************/

void Cmd_Invite(char *nick, client_t *client)
{
	client_t *invited;

	if (client->attr == 1)
	{
		PPrintf(client, RADIO_YELLOW, "You're an Admin, you can't create a squadron");
		return;
	}

	if (!(invited = FindLClient(nick)))
	{
		PPrintf(client, RADIO_YELLOW, "User is not online");
		return;
	}

	if (invited == client)
	{
		PPrintf(client, RADIO_YELLOW, "You can't invite yourself");
		return;
	}

	if (invited->attr == 1)
	{
		PPrintf(client, RADIO_YELLOW, "You can't invite Administrators");
		return;
	}

	if (!client->squadron)
	{
		invited->invite = client;
	}
	else // join existing squad
	{
		if (client->squad_flag & SQUADRON_INVITE)
		{
			invited->invite = client;
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "You don't have permission to invite players");
			return;
		}
	}

	PPrintf(invited, RADIO_YELLOW, "%s invited you to join his squadron", client->longnick);
	PPrintf(client, RADIO_YELLOW, "You invited %s to join your squadron", invited->longnick);
}

/*************
 Cmd_Jsquad

 Join to an invited squadron
 *************/

void Cmd_Jsquad(client_t *client)
{
	if (client->attr == 1)
	{
		PPrintf(client, RADIO_YELLOW, "You're an Admin, you can't join a squadron");
		return;
	}

	if (client->invite && !client->squadron)
	{
		if (client->invite->squadron) // squadron already exists
		{
			sprintf(my_query, "UPDATE players SET squad_owner = '%u', squad_flag = '%u' WHERE id = '%u'", client->invite->squadron, 
			SQUADRON_INVITE, client->id);

			if (!Com_MySQL_Query(client, &my_sock, my_query))
			{
				client->squadron = client->invite->squadron;
				client->squad_flag = SQUADRON_INVITE;

				PPrintf(client->invite, RADIO_YELLOW, "%s joined your squadron", client->longnick);
				PPrintf(client, RADIO_YELLOW, "You have joined a squadron");
				PPrintf(client, RADIO_YELLOW, "%s", Com_SquadronName(client->squadron));
			}
			else
			{
				if (mysql_errno(&my_sock))
				{
					PPrintf(client, RADIO_YELLOW, "Cmd_Jsquad(join): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
					Com_Printf(VERBOSE_WARNING, "Cmd_Jsquad(join): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
				else
					return;
			}
		}
		else // create new squad
		{
			sprintf(my_query, "INSERT INTO squads (owner) VALUES ('%u')", client->invite->id);

			if (!Com_MySQL_Query(client, &my_sock, my_query))
			{
				//				id = (u_int32_t)mysql_insert_id(&my_sock); // from old DB model

				sprintf(my_query, "UPDATE players SET squad_owner = '%u', squad_flag = '%d' WHERE id = '%u'; UPDATE players SET squad_owner = '%u', squad_flag = '%u' WHERE id = '%u'",
						client->invite->id, SQUADRON_INVITE, client->id, client->invite->id, (SQUADRON_INVITE | SQUADRON_REMOVE), client->invite->id);

				if (mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_ON))
					Com_Printf(VERBOSE_ERROR, "%d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

				if (!d_mysql_query(&my_sock, my_query)) // query succeeded
				{
					client->invite->squadron = client->invite->id;
					client->invite->squad_flag = SQUADRON_INVITE | SQUADRON_REMOVE;
					client->squadron = client->invite->id;
					client->squad_flag = SQUADRON_INVITE;

					Com_MySQL_Flush(&my_sock, __FILE__, __LINE__);

					PPrintf(client->invite, RADIO_YELLOW,"%s joined your squadron" , client->longnick);
					PPrintf(client, RADIO_YELLOW, "You have joined a squadron");
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Jsquad(new): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}

#ifdef  _WIN32
				sleep(140);
#else			
				usleep(140000);
#endif								
				mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
			}
			else
			{
				if(mysql_errno(&my_sock))
				{
					PPrintf(client, RADIO_YELLOW, "Cmd_Jsquad(new): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
					Com_Printf(VERBOSE_WARNING, "Cmd_Jsquad(new): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
				else
				return;
			}

		}

		client->invite = NULL;
	}
	else if(client->squadron)
	{
		PPrintf(client, RADIO_YELLOW, "You already have a squadron");
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "You are not invited yet");
	}
}

/*************
 Cmd_Name

 Assign a name to Squadron
 *************/

void Cmd_Name(char *name, client_t *client) // twin of Cmd_Slogan
{
	if (!client->squadron)
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
		return;
	}

	if (client->squadron != client->id)
	{
		PPrintf(client, RADIO_YELLOW, "You are not the squadron leader");
		return;
	}

	sprintf(my_query, "UPDATE squads SET name = '%s' WHERE owner = '%u'", name, client->id);

	if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Squadron name updated");
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "Cmd_Name(): SQL Error(%d), please contact admin", mysql_errno(&my_sock));
		Com_Printf(VERBOSE_WARNING, "Cmd_Name(update): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/*************
 Cmd_Slogan

 Assign a motto to Squadron
 *************/

void Cmd_Slogan(char *motto, client_t *client) // twin of Cmd_Name
{
	if (!client->squadron)
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
		return;
	}

	if (client->squadron != client->id)
	{
		PPrintf(client, RADIO_YELLOW, "You are not the squadron leader");
		return;
	}

	sprintf(my_query, "UPDATE squads SET motto = '%s' WHERE owner = '%u'", motto, client->id);

	if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Squadron motto updated");
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "Cmd_Slogan(): SQL Error(%d), please contact admin", mysql_errno(&my_sock));
		Com_Printf(VERBOSE_WARNING, "Cmd_Slogan(update): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/*************
 Cmd_Remove

 Remove a client from squadron
 *************/

void Cmd_Remove(char *nick, client_t *client)
{
	client_t *removed;
	u_int32_t remid;
	u_int32_t remsquad_owner;

	if (!client || client->squadron)
	{
		if (!client || (client->squad_flag & SQUADRON_REMOVE))
		{
			sprintf(my_query, "SELECT id, squad_owner FROM players WHERE longnick = '%s'", nick);

			if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((my_row = mysql_fetch_row(my_result))) // player 'found'
					{
						remid = Com_Atou(Com_MyRow("id"));
						remsquad_owner = Com_Atou(Com_MyRow("squad_owner"));

						mysql_free_result(my_result);
						my_result = NULL;
						my_row = NULL;

						if (client)
						{
							if (client->squadron != remsquad_owner)
							{
								PPrintf(client, RADIO_LIGHTYELLOW, "Player %s not found", nick); // if player is not from the same squadron (and not a console command), so, player 'is not found'
								return;
							}
						}

						if (remid == remsquad_owner)
						{
							PPrintf(client, RADIO_LIGHTYELLOW, "You cannot remove squadron leader");
						}
						else
						{
							sprintf(my_query, "UPDATE players SET squad_owner = '0', squad_flag = '0' WHERE id = '%u'", remid);

							if (!d_mysql_query(&my_sock, my_query)) // query succeeded
							{
								PPrintf(client, RADIO_LIGHTYELLOW, "%s has been removed from squadron", nick);

								if ((removed = FindLClient(nick)))
								{
									removed->squadron = 0;
									removed->squad_flag = 0;

									if (client != removed)
										PPrintf(removed, RADIO_YELLOW, "You've been excluded from your squadron by %s", client ? client->longnick : "-HOST-");
								}
							}
							else
							{
								Com_Printf(VERBOSE_WARNING, "Cmd_Remove(update): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							}
						}
					}
					else
					{
						if (!mysql_errno(&my_sock))
						{
							PPrintf(client, RADIO_LIGHTYELLOW, "Player %s not found", nick);
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Cmd_Remove(find): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}

						mysql_free_result(my_result);
						my_result = NULL;
						my_row = NULL;
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Remove(find): my_result == NULL, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				if (mysql_errno(&my_sock))
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Remove(find): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "You have no permissions to remove a member");
		}
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
	}

	if (mysql_errno(&my_sock))
		PPrintf(client, RADIO_YELLOW, "Cmd_Remove(): SQL Error(%d), please contact admin", mysql_errno(&my_sock));
}

/*************
 Cmd_Withdraw

 Withdraw a client from squadron
 *************/

void Cmd_Withdraw(client_t *client)
{
	if (!client || !client->squadron)
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
		return;
	}

	if (client->squadron == client->id) // squad leader
	{
		PPrintf(client, RADIO_YELLOW, "You are the squadron leader, you can only disband");
	}
	else // not squad leader
	{
		sprintf(my_query, "UPDATE players SET squad_owner = '0', squad_flag = '0' WHERE id = '%u'", client->id);

		if (!Com_MySQL_Query(client, &my_sock, my_query))
		{
			client->squadron = 0;
			client->squad_flag = 0;

			PPrintf(client, RADIO_YELLOW, "You have withdrawed your squadron");
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_Withdraw(member): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}

	if (mysql_errno(&my_sock))
		PPrintf(client, RADIO_YELLOW, "Cmd_Withdraw(): SQL Error(%d), please contact admin", mysql_errno(&my_sock));
}

/*************
 Cmd_Disband

 Disband squadron
 *************/

void Cmd_Disband(client_t *client)
{
	u_int32_t i;
	int16_t num_rows;
	client_t *member;

	if (!client || !client->squadron)
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
		return;
	}

	if (client->squadron == client->id) // squad leader
	{
		sprintf(my_query, "SELECT players.id, players.longnick FROM players, squads WHERE squads.owner = '%u' AND players.id != '%u' AND players.squad_owner = squads.owner", client->squadron,
				client->id);

		// remove all members
		if (!d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
			{
				if ((num_rows = mysql_num_rows(my_result)) > 0)
				{
					sprintf(my_query, "UPDATE players SET squad_owner='0', squad_flag='0' WHERE id IN(");

					for (i = 0; i < num_rows; i++)
					{
						if ((my_row = mysql_fetch_row(my_result)))
						{
							sprintf(my_query, "%s'%u'", my_query, Com_Atou(Com_MyRow("id")));

							if ((member = FindLClient(Com_MyRow("longnick"))))
							{
								member->squadron = 0;
								member->squad_flag = 0;

								PPrintf(member, RADIO_YELLOW, "Your squadron has been disbanded by leader");
							}
						}
						else
						{
							mysql_free_result(my_result);
							my_result = NULL;
							Com_Printf(VERBOSE_WARNING, "Cmd_Disband(remallup): Couldn't Fetch Row %d, error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
							break;
						}

						if (i == (num_rows - 1))
						{
							strcat(my_query, ")");

							if (d_mysql_query(&my_sock, my_query))
							{
								Com_Printf(VERBOSE_WARNING, "Cmd_Disband(remallup): Couldn't query UPDATE id %d error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
								return;
							}
						}
						else
						{
							strcat(my_query, ",");
						}
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Disband(remall): num_rows = %u\n", num_rows);
				}

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Disband(remall): my_result == NULL, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_Disband(remall): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}

		// remove squadron
		sprintf(my_query, "DELETE FROM squads WHERE owner = '%u'", client->squadron);

		if (!d_mysql_query(&my_sock, my_query))
		{
			PPrintf(client, RADIO_YELLOW, "Your squadron has been disbanded");
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_Disband(delete): couldn't query DELETE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}

		// clear leader ids					
		sprintf(my_query, "UPDATE players SET squad_owner='0', squad_flag='0' WHERE id ='%u'", client->id);

		if (!d_mysql_query(&my_sock, my_query))
		{
			client->squadron = 0;
			client->squad_flag = 0;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Cmd_Disband(delete): couldn't query DELETE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else // not squad leader
	{
		PPrintf(client, RADIO_YELLOW, "You aren't squadron leader, you cannot disband squadron");
	}
}

/*************
 Cmd_Psq

 Change Squad Members permissions
 *************/

void Cmd_Psq(char *nick, u_int8_t attr, client_t *client)
{
	char *id;
	client_t *cnick;

	if (!strcmp(nick, client->longnick))
	{
		PPrintf(client, RADIO_YELLOW, "You cannot change your own privileges");
		return;
	}

	if (client->squadron)
	{
		if (client->id == client->squadron) // player is squadron leader
		{
			sprintf(my_query, "SELECT id FROM players WHERE squad_owner = '%u' AND longnick = '%s'", client->id, nick);

			if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((my_row = mysql_fetch_row(my_result)))
					{
						if ((id = Com_MyRow("id")))
						{
							sprintf(my_query, "UPDATE players SET squad_flag = '%u' WHERE id = '%s'", attr, id);

							if (!d_mysql_query(&my_sock, my_query))
							{
								if ((cnick = FindLClient(nick)))
								{
									cnick->squad_flag = attr;
								}
							}
							else
							{
								Com_Printf(VERBOSE_WARNING, "Cmd_Psq(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							}
						}
						else
						{
							PPrintf(client, RADIO_YELLOW, "Player not found or not in your squadron");
						}
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "Cmd_Psq(): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Psq(): my_result == NULL, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				if (mysql_errno(&my_sock))
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Psq(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "You are not the squadron leader");
		}

		if (mysql_errno(&my_sock))
			PPrintf(client, RADIO_YELLOW, "Cmd_Psq(): SQL Error(%d), please contact admin", mysql_errno(&my_sock));
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
	}
}

/*************
 Cmd_Hls

 Send name of squadrons members to show their nicks in white
 *************/

void Cmd_Hls(client_t *client)
{
	u_int8_t buffer[1032];
	u_int16_t i, j;
	hls_t *hls;

	if (!client->squadron)
	{
		PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
		return;
	}

	for (i = 0, j = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			if (clients[i].squadron == client->squadron)
				j++;
		}
	}

	memset(buffer, 0, sizeof(buffer));

	hls = (hls_t *) buffer;

	hls->packetid = htons(Com_WBhton(0x0419));
	hls->num = j; // free 'j' variable

	for (i = 0, j = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			if (clients[i].squadron == client->squadron)
			{
				hls = (hls_t *) (buffer+(4*j));

				if (!client->hls)
					hls->nick = htonl(clients[i].shortnick);
				else
					hls->nick = 0;

				j++;

				if (j == 256)
					break;
			}
		}
	}

	hls = (hls_t *) (buffer+(4*j));

	if (!client->hls)
		hls->nick = htonl(client->shortnick);
	else
		hls->nick = 0;

	if (client->hls)
	{
		PPrintf(client, RADIO_YELLOW, "hightlightsquad mode is off");
		client->hls = 0;
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "hightlightsquad mode is on");
		client->hls = 1;
	}

	SendPacket(buffer, 7+(4*j), client);
}

/*************
 Cmd_Listavail

 List available planes in given airfield
 *************/

void Cmd_Listavail(u_int8_t field, client_t *client)
{
	u_int8_t i, j;
	u_int32_t time;
	u_int32_t rpsreplace;
	FILE *fp;
	char buffer[32];

	if (field > fields->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid Field F%u", field);
		return;
	}

	j = field - 1;

	if (client)
	{
		if (arena->fields[j].country != client->country && !(client->attr & (FLAG_OP | FLAG_ADMIN)))
		{
			PPrintf(client, RADIO_YELLOW, "This field is not from your country");
			return;
		}
	}

	memset(buffer, 0, sizeof(buffer));

	sprintf(buffer, "./fields/availf%d.txt", field);

	if (!(fp = fopen(buffer, "wb")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: Cmd_Listavail() Couldn't open file \"%s\"", buffer);
	}
	else
	{
		fprintf(fp, "          Available planes for F%d\n", field);
		fprintf(fp, "==============================================\n");

		for (i = 1; i < maxplanes; i++)
		{
			if (arena->fields[j].rps[i])
			{
				if (GetSmallPlaneName(i))
				{
					if (rps->value && !arcade->value)
					{
						rpsreplace = (rps->value * 6000) / arena->rps[i].pool[arena->fields[j].type - 1];

						if(!((arena->fields[j].country == 1 && arena->rps[i].country & 0x01) ||
							(arena->fields[j].country == 2 && arena->rps[i].country & 0x02) ||
							(arena->fields[j].country == 3 && arena->rps[i].country & 0x04) ||
							(arena->fields[j].country == 4 && arena->rps[i].country & 0x08)))
						{
							rpsreplace = 0 ;
						}

						if(rpsreplace)
						{
							rpsreplace -= (arena->frame % rpsreplace);

							fprintf(fp, "%-11s(N%d)%s %3d avail%8s to replace\n", GetSmallPlaneName(i), i, (i < 10)?"  ":(i < 100)?" ":"",(int16_t)arena->fields[j].rps[i], Com_TimeSeconds(rpsreplace/100));
						}
						else if (arena->fields[j].rps[i] >= 1)
						{
							fprintf(fp, "%-11s(N%d)%s %3d avail  no replacement\n", GetSmallPlaneName(i), i, (i < 10)?"  ":(i < 100)?" ":"", (int16_t)arena->fields[j].rps[i]);
						}
					}
					else if (arena->fields[j].rps[i] >= 1 || arena->fields[j].rps[i] == -1)
					{
						fprintf(fp, "%-11s(N%d)%s %3d avail\n", GetSmallPlaneName(i), i, (i < 10)?"  ":(i < 100)?" ":"", (int16_t)arena->fields[j].rps[i]);
					}
				}
			}
		}

		fclose(fp);

		if(client)
			SendFileSeq1(buffer, "avail.asc", client);
		else
			Sys_Printfile(buffer);
	}

//	if (rps->value)
//	{
//		time = (u_int32_t)((rps->value * 6000) - (arena->frame % (u_int32_t)(rps->value * 6000)))/100;
//		PPrintf(client, RADIO_YELLOW, "Time for replace: %s", Com_TimeSeconds(time));
//	}
}

/*************
 Cmd_Wings

 Enable/disable wings mode
 *************/

void Cmd_Wings(u_int8_t mode, client_t *client)
{
	u_int8_t i;
	int16_t oldangle = 0;
	int16_t newangle;

	if (client->tkstatus)
	{
		PPrintf(client, RADIO_YELLOW, "You are a team killer, you cannot use Wingmen");
		return;
	}

	switch (mode)
	{
		case FORMATION_VWING:
			newangle = 1350;
			PPrintf(client, RADIO_YELLOW, "Wings mode set to Vee Wing");
			break;
		case FORMATION_LABREAST:
			newangle = 2700;
			PPrintf(client, RADIO_YELLOW, "Wings mode set to Line Abreast");
			break;
		case FORMATION_LASTERN:
			newangle = 1800;
			PPrintf(client, RADIO_YELLOW, "Wings mode set to Line Astern");
			break;
		case FORMATION_ECHELON:
			newangle = 2250;
			PPrintf(client, RADIO_YELLOW, "Wings mode set to Echelon");
			break;
		default:
			if (client->infly && !client->attached)
			{
				PPrintf(client, RADIO_YELLOW, "You cannot disable wings while flying");
				return;
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Wings mode disabled");
				client->wings = 0;
				return;
			}
	}

	client->wings = 1;

	switch (client->droneformation)
	{
		case FORMATION_VWING:
			oldangle = 1350;
			break;
		case FORMATION_LABREAST:
			oldangle = 2700;
			break;
		case FORMATION_LASTERN:
			oldangle = 1800;
			break;
		case FORMATION_ECHELON:
			oldangle = 2250;
			break;
		default:
			break;
	}

	for (i = 0; i < MAX_RELATED; i++)
	{
		if (client->related[i] && (client->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
		{
			client->related[i]->droneformation = mode;

			if (client->related[i]->drone == DRONE_WINGS2)
			{
				if (client->droneformation == FORMATION_VWING)
				{
					oldangle = 2250;
				}

				if (mode == FORMATION_VWING)
				{
					newangle = 2250;
				}
			}

			if (!client->related[i]->droneformchanged)
			{
				client->related[i]->droneformchanged = oldangle - newangle;
			}
			else
			{
				client->related[i]->droneformchanged = oldangle + client->related[i]->droneformchanged - newangle;
			}
		}
	}

	client->droneformation = mode;
}

/*************
 Cmd_Hmack

 start hmack
 *************/

void Cmd_Hmack(client_t *client, char *command, u_int8_t tank)
{
	client_t *drone;
	u_int8_t i;
	int16_t angle;

	/*	if(notanks->value)
	 {
	 PPrintf(client, RADIO_YELLOW, "You're not allowed to use this command");
	 return;		
	 }
	 */

	if (client->infly && !command)
	{
		PPrintf(client, RADIO_YELLOW, "You're already flying");
		return;
	}

	if (client->attr == 1)
	{
		PPrintf(client, RADIO_YELLOW, "You're an Admin, you can't start a %s", tank ? "tank" : "hmack");
		return;
	}

	if (!command)
	{
		if (arena->fields[client->field-1].closed)
		{
			PPrintf(client, RADIO_YELLOW, "This field is closed");
			return;
		}

		if (arena->fields[client->field-1].country != client->country)
		{
			PPrintf(client, RADIO_YELLOW, "This field is not from your country");
			return;
		}

		if (client->tkstatus)
		{
			PPrintf(client, RADIO_YELLOW, "You are a team killer, you must fly bomber");
			return;
		}

		if (arena->fields[client->field-1].type == FIELD_CV)
		{
			PPrintf(client, RADIO_YELLOW, "You can't start a %s from CV", tank ? "TANK" : "Human Ack");
			return;
		}

		if (client->hmackpenalty && client->hmackpenaltyfield == client->field && client->attr != 1)
		{
			PPrintf(client, RADIO_YELLOW, "You can't take off from this field for %s", Com_TimeSeconds(client->hmackpenalty / 100));
			return;
		}

		if (tank)
			drone
					= AddDrone(DRONE_HTANK, arena->fields[client->field - 1].posxyz[0], arena->fields[client->field - 1].posxyz[1], arena->fields[client->field - 1].posxyz[2], client->country, 101/*TANK*/, client);
		else
		{
			if (client->mapper)
				drone = AddDrone(DRONE_HMACK, client->posxy[0][0], client->posxy[1][0], 0, client->country, 79/*OPEL*/,client);
			else
				drone
						= AddDrone(DRONE_HMACK, arena->fields[client->field - 1].posxyz[0], arena->fields[client->field - 1].posxyz[1], arena->fields[client->field - 1].posxyz[2], client->country, 85/*JEEP*/,client);
		}

		if (!drone)
			return;

		client->attached = drone;

		SendGunnerStatusChange(drone, 2, client); // define position to be attached in client
		SendAttachList(NULL, client);
		client->visible[MAX_SCREEN - 1].client = drone;
		AddRemovePlaneScreen(drone, client, FALSE);

		/*
		 if(!client->visible[MAX_SCREEN - 1].client)
		 {
		 client->visible[MAX_SCREEN - 1].client = drone;
		 AddRemovePlaneScreen(drone, client, FALSE);
		 }
		 else
		 {
		 PPrintf(client, RADIO_YELLOW, "You are already attached to someone");
		 RemoveDrone(drone);
		 return;
		 }
		 */

		client->speedxyz[0][0] = client->speedxyz[1][0] = client->angles[2][0] = 0;
		Cmd_Fly(2, client);
	}
	else
	{
		for (i = 0; i < MAX_RELATED; i++)
		{
			if (client->related[i] && (client->related[i]->drone & (DRONE_HMACK | DRONE_HTANK)))
			{
				if (!Com_Stricmp(command, "speed"))
				{
					angle = WBtoHdg(client->angles[2][0]);

					if (client->posalt[0])
					{
						client->speedxyz[0][0] = (client->hmackgear + 1) * -25 * sin(Com_Rad(angle));
						client->speedxyz[1][0] = (client->hmackgear + 1) * 25 * cos(Com_Rad(angle));
					}
				}
				else if (!Com_Stricmp(command, "run"))
				{
					if (client->speedxyz[0][0] || client->speedxyz[1][0])
						client->hmackgear = 1;

					Cmd_Hmack(client, "speed", 0);
				}
				else if (!Com_Stricmp(command, "stop"))
				{
					client->speedxyz[0][0] = 0;
					client->speedxyz[1][0] = 0;
					client->hmackgear = 0;
				}
				else if (command[0] == '+')
				{
					command++;
					if (client->angles[2][0] < 0)
						client->angles[2][0] += 3600;

					if ((client->angles[2][0] += (Com_Atof(command) * 10)) >= 3600)
						client->angles[2][0] %= 3600;

					if (client->angles[2][0] > 901)
					{
						client->angles[2][0] -= 3600;
					}

					if (client->speedxyz[0][0] || client->speedxyz[1][0])
						Cmd_Hmack(client, "speed", 0);
				}
				else if (command[0] == '-')
				{
					command++;
					if (client->angles[2][0] < 0)
						client->angles[2][0] += 3600;

					if ((client->angles[2][0] -= (Com_Atof(command) * 10)) < 0)
						client->angles[2][0] = (3600 + client->angles[2][0]);

					if (client->angles[2][0] > 901)
					{
						client->angles[2][0] -= 3600;
					}

					if (client->speedxyz[0][0] || client->speedxyz[1][0])
						Cmd_Hmack(client, "speed", 0);
				}
				else
				{
					if ((angle = (Com_Atof(command) * 10)) > 3599)
					{
						PPrintf(client, RADIO_YELLOW, "angle: 0<->359.9 degrees");
						return;
					}

					if (angle > 901)
					{
						angle -= 3600;
					}

					client->angles[2][0] = angle;

					if (client->speedxyz[0][0] || client->speedxyz[1][0])
						Cmd_Hmack(client, "speed", 0);
				}
				break;
			}
		}

		if (i == MAX_RELATED)
		{
			PPrintf(client, RADIO_YELLOW, "You are not in a human ack");
		}
	}
}

/*************
 Cmd_Commandos

 Drop Commandos
 *************/

void Cmd_Commandos(client_t *client, u_int32_t height)
{
	client_t *drone;
	u_int8_t i;
	int32_t x, y;
	u_int32_t dist;

	dist = 0;

	if (!IsCargo(client))
	{
		PPrintf(client, RADIO_YELLOW, "Commandos are available only for cargo planes");
		return;
	}

	if (client->tkstatus)
	{
		PPrintf(client, RADIO_YELLOW, "You are a team killer, you cannot drop commandos");
		return;
	}

	if (!height)
	{
		PPrintf(client, RADIO_YELLOW, "You are above water, commandos is not a SEAL");
		return;
	}

	for (i = 0; i < fields->value; i++)
	{
		x = client->posxy[0][0] - arena->fields[i].posxyz[0];
		y = client->posxy[1][0] - arena->fields[i].posxyz[1];

		if ((x >= -10000 && x <= 10000) && (y >= -10000 && y <= 10000))
		{
			dist = sqrt(Com_Pow(x, 2) + Com_Pow(y, 2));

			if ((arena->fields[i].type == FIELD_WB3POST && dist < 1000) || (arena->fields[i].type == FIELD_WB3VILLAGE && dist < 2700) || (arena->fields[i].type == FIELD_WB3TOWN && dist < 3600)
					|| (arena->fields[i].type <= FIELD_MAIN && dist < arena->fields[i].radius))
			{
				PPrintf(client, RADIO_YELLOW, "Too near to field f%d, acks would kill Commandos", i+1);
				return;
			}
			if (dist < 10000)
				break;
		}
	}

	if (i < fields->value)
	{
		if (arena->fields[i].country == client->country)
		{
			PPrintf(client, RADIO_YELLOW, "You can't drop commandos at your own airfield");
			return;
		}

		if (client->commandos)
		{
			client->commandos--;
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "You cannot drop more commandos");
			return;
		}

		drone = AddDrone(DRONE_COMMANDOS, client->posxy[0][0], client->posxy[1][0], height, client->country, 61/*chute*/, client);

		if (!drone)
			return;

		drone->dronefield = i;

		PPrintf(client, RADIO_YELLOW, "Commandos dropped at field f%d (dist %d)", i+1, dist);
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "Too far from field");
	}

}

/*************
 Cmd_Info

 Get user info
 *************/

void Cmd_Info(char *nick, client_t *client)
{
	client_t *info;

	if (!(info = FindLClient(nick)))
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "User not found");
		return;
	}

	if (!client || client->attr & (FLAG_ADMIN | FLAG_OP))
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "User:%s IP:%s(%s) Country:%s Field:%d", info->longnick, info->ip, GeoIP_country_name_by_addr(gi, info->ip), GetCountry(info->country), info->field);
		PPrintf(client, RADIO_LIGHTYELLOW, "Situation:%s Plane:%s Mod:%s ", info->infly ? IsGround(info) ? "In Ground" : "In Flight" : info->field ? !info->hq ? "Tower" : "HQ" : "Connecting",
				GetPlaneName(info->plane), info->attr == 2 ? "OP" : info->attr == 1 ? "Admin" : "Normal");
		PPrintf(client, RADIO_LIGHTYELLOW, "Squadron:%s", info->squadron ? Com_SquadronName(info->squadron) : "None");
	}
	else
	{
		if (client->country != info->country)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "This user is not from your country");
		}
		else
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "User:%s Field:%d Situation:%s Plane:%s", info->longnick, info->field, info->infly ? IsGround(info) ? "In Ground" : "In Flight"
					: info->field ? !info->hq ? "Tower" : "HQ" : "Connecting", GetPlaneName(info->plane));
			PPrintf(client, RADIO_LIGHTYELLOW, "Squadron:%s", info->squadron ? Com_SquadronName(info->squadron) : "None");
		}
	}
}

/*************
 Cmd_Ban

 Ban someone
 *************/

void Cmd_Ban(char *nick, u_int8_t newvalue, client_t *client) // twin of Cmd_TK
{
	client_t *cban;
	u_int32_t id = 0;
	int16_t num_rows;
	u_int8_t oldvalue;
	u_int16_t i;

	if (!strcmp(nick, "franz-") || !strcmp(nick, "vibora"))
	{
		PPrintf(client, RADIO_YELLOW, "%s is a good guy and cannot be banned", nick);
		return;
	}

	if (!(cban = FindLClient(nick)))
	{
		sprintf(my_query, "SELECT id, banned FROM players WHERE longnick = '%s'", nick);

		if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
		{
			if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
			{
				if ((my_row = mysql_fetch_row(my_result)))
				{
					id = Com_Atou(Com_MyRow("id"));
					oldvalue = Com_Atou(Com_MyRow("banned"));
				}
				else
				{
					if (!mysql_errno(&my_sock))
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "Player %s not found", nick);
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "Cmd_Ban(find): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						PPrintf(client, RADIO_YELLOW, "Cmd_Ban(find): Couldn't Fetch Row, please contact admin");
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
					return;
				}

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Ban(find): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				return;
			}
		}
		else
		{
			if (mysql_errno(&my_sock))
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Ban(find): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			return;
		}
	}
	else
	{
		oldvalue = 0;
		id = cban->id;
	}

	if (id)
	{
		if (oldvalue == newvalue)
		{
			if (oldvalue)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Player %s is already banned", nick);
			}
			else
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Player %s is not banned yet", nick);
			}

			return;
		}
		else
		{
			if (!cban && client) // clear SQL flood check if player id was get from DB
			{
				client->lastsql = 0;
			}

			sprintf(my_query, "UPDATE players SET banned = %u WHERE id = '%u'", newvalue, id);

			if (!Com_MySQL_Query(client, &my_sock, my_query))
			{
				if (newvalue)
				{
					BPrintf(RADIO_LIGHTYELLOW, "%s was banned by %s", nick, client?client->longnick:"-HOST-");
					Com_Printf(VERBOSE_ATTENTION, "%s was banned by %s\n", nick, client ? client->longnick : "-HOST-");
				}
				else
				{
					BPrintf(RADIO_LIGHTYELLOW, "%s was unbanned by %s", nick, client?client->longnick:"-HOST-");
					Com_Printf(VERBOSE_ATTENTION, "%s was unbanned by %s\n", nick, client ? client->longnick : "-HOST-");
				}
				/*
				 RemoveClient(cban);
				 */
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Ban(banid): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			sprintf(
					my_query,
					"SELECT hdserials.id FROM players, players_hdserials, hdserials WHERE players.id = '%u' AND players.id = players_hdserials.player_id AND hdserials.id = players_hdserials.hdserial_id",
					id);

			if (!d_mysql_query(&my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((num_rows = mysql_num_rows(my_result)) > 0)
					{
						sprintf(my_query, "UPDATE hdserials SET banned='%u' WHERE id IN(", newvalue);

						for (i = 0; i < num_rows; i++)
						{
							if ((my_row = mysql_fetch_row(my_result)))
							{
								sprintf(my_query, "%s'%u'", my_query, Com_Atou(Com_MyRow("id")));
							}
							else
							{
								mysql_free_result(my_result);
								my_result = NULL;
								Com_Printf(VERBOSE_WARNING, "Cmd_Ban(banhd): Couldn't Fetch Row %d, error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
								break;
							}

							if (i == (num_rows - 1))
							{
								strcat(my_query, ")");

								if (d_mysql_query(&my_sock, my_query))
								{
									Com_Printf(VERBOSE_WARNING, "Cmd_Ban(banhd): Couldn't query UPDATE id %d error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
									return;
								}
							}
							else
							{
								strcat(my_query, ",");
							}
						}
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "Cmd_Ban(banhd): num_rows = %u\n", num_rows);
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "Cmd_Ban(banhd): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Ban(banhd): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Cmd_Ban(): id == NULL\n");
	}
}

/*************
 Cmd_Gclear

 Clears Gunners list
 *************/

void Cmd_Gclear(char *nick, client_t *client)
{
	u_int8_t i;
	u_int8_t buffer[8];
	reqgunnerlist_t *gunner;

	if (strcmp(nick, "0"))
	{
		for (i = 0; i < 7; i++)
		{
			if (client->gunners[i])
			{
				if (!strcmp(client->gunners[i]->longnick, nick))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s removed from gunner's list", client->gunners[i]->longnick);
					client->gunners[i] = 0;
					break;
				}
			}
		}

		for (i = 7; i < 14; i++)
		{
			if (client->gunners[i])
				break;
		}

		if (i == 14) // no gunners found in request list
		{
			gunner = (reqgunnerlist_t *) buffer;
			gunner->unknown1 = htons(0x0006);
			gunner->nick = htonl(client->shortnick);
			SendAttachList(buffer, client);
		}

	}
	else
	{
		for (i = 0; i < 14; i++)
		{
			client->gunners[i] = 0;
		}
		PPrintf(client, RADIO_LIGHTYELLOW, "All gunner list cleared");
	}
}

/*************
 Cmd_Shanghai

 Set a client (student) as oberver of trainer's plane
 *************/

void Cmd_Shanghai(u_int8_t *buffer, client_t *client)
{
	shanghai_t *shanghai;
	client_t *student;

	shanghai = (shanghai_t *) buffer;

	student = FindSClient(ntohl(shanghai->nick));

	if (student)
	{
		if (student->infly)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "%s is already flying", student->longnick);
			return;
		}

		if (student->country != client->country)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "%s is not from your country", student->longnick);
			return;
		}

		PPrintf(client, RADIO_LIGHTYELLOW, "%s has been added to shanghai", student->longnick);
		PPrintf(student, RADIO_LIGHTYELLOW, "%s added you as shanghai", client->longnick);

		if (debug->value)
			PPrintf(client, RADIO_RED, "DEBUG: %d", ntohs(shanghai->unknown1));
		client->shanghai = student;
	}
	else
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Shanghai has been cleared");
		client->shanghai = NULL;
	}
}

/*************
 Cmd_View

 View what other player is doing
 *************/

void Cmd_View(client_t *victim, client_t *client)
{
	u_int8_t i;

	if (victim)
	{
		if (victim->view)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "%s is already viewing %s", victim->view->longnick, victim->longnick);
			return;
		}
		else
		{
			if (client->infly)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "You are already in flight");
				return;
			}

			if (!victim->infly)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Player is not in flight");
				return;
			}

			for (i = 0; i < MAX_SCREEN; i++)
			{
				if (client->visible[i].client == victim)
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "Player is in visible list, change field and try again");
					return;
				}
			}

			victim->view = client;
			client->attached = victim;
			SendGunnerStatusChange(victim, 2, client); // define position to be attached in client
			SendAttachList(NULL, client);
			client->visible[MAX_SCREEN - 1].client = victim;
			AddRemovePlaneScreen(victim, client, FALSE);
			Cmd_Fly(2, client);
		}
	}
	else
	{
		if (client->attached)
		{
			if (client->attached->view == client)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "You have stopped viewing %s", client->attached->longnick);
				client->attached->view = NULL;
				ForceEndFlight(FALSE, client);
			}
		}
		else
			PPrintf(client, RADIO_LIGHTYELLOW, "User not found");
	}
}

/*************
 Cmd_View

 View what other player is doing
 *************/

void Cmd_Minen(u_int32_t dist, float angle, client_t *client)
{
	int32_t destx, desty;

	if (client->mortars)
	{
		if (dist > 6000)
		{
			PPrintf(client, RADIO_YELLOW, "Mortar's maximum range is 6000 ft");
			return;
		}

		client->mortars--;
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "You don't have any mortar");
		return;
	}

	if (wb3->value)
		destx = client->posxy[0][0] + (dist * sin(Com_Rad(angle)));
	else
		destx = client->posxy[0][0] - (dist * sin(Com_Rad(angle)));
	desty = client->posxy[1][0] + (dist * cos(Com_Rad(angle)));

	ThrowBomb(FALSE, client->posxy[0][0], client->posxy[1][0], client->posalt[0], destx, desty, 0, client);
	//	ThrowBomb(TRUE, client->posxy[0][0], client->posxy[1][0], client->posalt[0], destx, desty, 0, client);

	PPrintf(client, RADIO_YELLOW, "Mortar fired dist %u azimuth %.2f (%d left)", dist, angle, client->mortars);
}

/*************
 Cmd_Tanks

 Send a tank column to field
 *************/

void Cmd_Tanks(char *field, client_t *client)
{
	u_int8_t fieldn;

	if (client->tkstatus)
	{
		PPrintf(client, RADIO_YELLOW, "You are a team killer, you cannot use tanks");
		return;
	}

	if (tolower(*field) == 'f')
	{
		field++;
		fieldn = Com_Atoi(field)-1;

		if (fieldn >= fields->value)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Invalid field");
			return;
		}

		if (arena->fields[fieldn].type >= FIELD_CV && arena->fields[fieldn].type <= FIELD_SUBMARINE)
		{
			PPrintf(client, RADIO_YELLOW, "You can't launch a column to a CV");
			return;
		}

		if (arena->fields[client->field - 1].type >= FIELD_CV && arena->fields[client->field - 1].type <= FIELD_SUBMARINE)// && !client->attr) // !client->attr only for JAVA SCENARIO
		{
			PPrintf(client, RADIO_YELLOW, "You can't launch a column from a CV");
			return;
		}

		if (arena->fields[fieldn].country != client->country)
		{
			Com_LogEvent(EVENT_TANKS, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
			Com_LogDescription(EVENT_DESC_FIELD, client->field, NULL);
			Com_LogDescription(EVENT_DESC_TOFIELD, Com_Atoi(field), NULL);

			LaunchTanks(client->field - 1, fieldn, client->country, client);
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "This field is from your country");
		}
	}
	else
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Invalid Field");
	}
}

/*************
 Cmd_Pos

 Send client pos
 *************/

void Cmd_Pos(u_int32_t freq, client_t *client, client_t *peek)
{
	char buffer[128];

	if (!client)
		return;

	if (client->infly)
	{
		sprintf(buffer, "I'm a %s at %s angels%.0f(%.0fm) %s to %s(%.0f)", GetSmallPlaneName(client->plane), Com_Padloc(client->posxy[0][0], client->posxy[1][0]), WBAngels(client->posalt[0]),
				WBAltMeters(client->posalt[0]), WBVSI(client->speedxyz[2][0], 0), WBRhumb(WBtoHdg(client->angles[2][0])), WBHeading(WBtoHdg(client->angles[2][0])) );
	}
	else
	{
		sprintf(buffer, "I'm at tower");

		if (!client->hq)
		{
			sprintf(buffer, "%s F%d", buffer, client->field);
		}
		else
		{
			sprintf(buffer, "%s HQ", buffer);
		}
	}

	if (peek)
	{
		PPrintf(peek, RADIO_LIGHTYELLOW, "%s %s", client->longnick, buffer);
		PPrintf(client, RADIO_LIGHTYELLOW, "%s got your position", peek->longnick);
	}
	else
	{
		if (!freq)
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "(echo) %s", buffer);
		}
		else
		{
			PrintRadioMessage(freq, client->shortnick, buffer, strlen(buffer), client);
		}
	}
}

/*************
 Cmd_Thanks

 Thanks a friend with 10% of last score owned
 *************/

void Cmd_Thanks(char *argv[], u_int8_t argc, client_t *client)
{
	u_int8_t i;
	float score;
	client_t *pclient;

	if (argc > 3)
		argc = 3;

	if (!client->infly)
	{
		PPrintf(client, RADIO_YELLOW, "You cannot spawn thanks while flying");
		return;
	}
	if (client->lastscore > 0)
		score = client->lastscore / 10;
	else
	{
		PPrintf(client, RADIO_YELLOW, "You have no score to award to anyone");
		return;
	}

	for (i = 0; i < argc; i++)
	{
		if ((pclient = FindLClient(argv[i])))
		{
			Com_LogEvent(EVENT_THANKS, client->id, pclient->id);
			Com_LogDescription(EVENT_THANKS, score, NULL);

			pclient->streakscore += score;
			sprintf(my_query, "UPDATE score_common SET totalscore = totalscore + '%.3f', streakscore = '%.3f' WHERE player_id = '%u'", score, pclient->streakscore, pclient->id);

			if (d_mysql_query(&my_sock, my_query))
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Thanks(sum): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			client->streakscore -= score;
			client->lastscore -= score;
			sprintf(my_query, "UPDATE score_common SET totalscore = totalscore + '%.3f', streakscore = '%.3f', lastscore = '%.3f' WHERE player_id = '%u'", score, pclient->streakscore,
					client->lastscore, pclient->id);

			if (d_mysql_query(&my_sock, my_query))
			{
				Com_Printf(VERBOSE_WARNING, "Cmd_Thanks(sub): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			PPrintf(client, RADIO_YELLOW, "You rewarded %.3f score to %s", score, pclient->longnick);
			PPrintf(pclient, RADIO_YELLOW, "%s rewarded you with %.3f score", client->longnick, score);
		}
	}
}

/*************
 Cmd_Restore

 restore field to all up structures
 *************/

void Cmd_Restore(u_int8_t field, client_t *client)
{
	u_int16_t i;

	if (!field || field > fields->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid Field");
		return;
	}

	arena->fields[field - 1].tonnage = 0;
	arena->fields[field - 1].paras = 0;
	
	for (i = 0; i < MAX_BUILDINGS; i++)
	{
		if (arena->fields[field - 1].buildings[i].field)
		{
			if (arena->fields[field - 1].buildings[i].status)
				arena->fields[field - 1].buildings[i].timer = 1;
		}
		else
		{
			break;
		}
	}
}

/*************
 Cmd_Destroy

 Destroy all structured at field for 10 minutes
 *************/

void Cmd_Destroy(u_int8_t field, int32_t time, client_t *client)
{
	u_int16_t i;

	if (!field || field > fields->value)
	{
		PPrintf(client, RADIO_YELLOW, "Invalid Field");
		return;
	}

	if(time > 0)
	{
		time *= 6000; // convert minutes to frames
	}
	else
	{
		time = 0;
	}
	
	for (i = 0; i < MAX_BUILDINGS; i++)
	{
		if (arena->fields[field - 1].buildings[i].field)
		{
			arena->fields[field - 1].buildings[i].status = 2;
			arena->fields[field - 1].buildings[i].timer = time;
		}
		else
		{
			break;
		}
	}

	SendFieldStatus(field - 1, NULL);
}

/*************
 Cmd_ChangeCVRoute

 Changes CV Route by command
 *************/

void Cmd_ChangeCVRoute(double angle, u_int16_t distance, client_t *client)
{
	if (arena->fields[client->field - 1].type >= FIELD_CV && arena->fields[client->field - 1].type <= FIELD_SUBMARINE)
	{
		if (distance <= 20000 && distance >= 5000)
			ChangeCVRoute(arena->fields[client->field - 1].cv, angle, distance, client);
		else
			PPrintf(client, RADIO_LIGHTYELLOW, "Distance limited to 5000 - 20000 feets");
	}
	else
		PPrintf(client, RADIO_LIGHTYELLOW, "You are not in a convoy");
}

/*************
 Cmd_UTC

 Prints ZULU time
 *************/

void Cmd_UTC(client_t *client)
{
	time_t ltime;
	struct tm *timeptr;

	time(&ltime);
	timeptr = gmtime(&ltime);

	PPrintf(client, RADIO_YELLOW, "%d-%02d-%02d %.2d:%.2d:%.2dZ", 1900 + timeptr->tm_year, timeptr->tm_mon + 1, timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);
}

/*************
 Cmd_Lives

 Set amount of lives
 *************/

void Cmd_Lives(char *nick, int8_t amount, client_t *client)
{
	u_int32_t i;
	char buffer[64];
	client_t *player;

	sprintf(buffer, "UPDATE players SET lives = %d", amount);

	if (nick)
	{
		if ((player = FindLClient(nick)))
		{
			sprintf(buffer, "%s WHERE longnick = '%s'", buffer, nick);
			PPrintf(client, RADIO_LIGHTYELLOW, "Lives of %s set to %d", nick, amount);
			player->lives = amount;
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "%s not online", nick);
			return;
		}
	}
	else
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Lives of all set to %d", amount);
		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && clients[i].ready && !clients[i].drone)
			{
				clients[i].lives = amount;
			}
		}
	}

	if (d_mysql_query(&my_sock, buffer)) // query succeeded
	{
		Com_Printf(VERBOSE_WARNING, "Cmd_Life(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
}

/*************
 Cmd_Reload

 Just restart fly to reload fuel and weapons
 *************/

void Cmd_Reload(client_t *client)
{
	int8_t field;
	u_int32_t distance;

	if (!canreload->value)
	{
		PPrintf(client, RADIO_YELLOW, "Command disabled by Administrator");
		return;
	}

	if ((FLIGHT_TIME(client)/1000) < 600)
	{
		if (!(client->speedxyz[0][0] + client->speedxyz[1][0] + client->speedxyz[2][0]))
		{
			field = NearestField(client->posxy[0][0], client->posxy[1][0], 0, FALSE, FALSE, &distance);

			if (field >= 0 && arena->fields[field].country == client->country)
			{
				if (distance < arena->fields[field].radius)
				{
					if (!(client->status_damage))
					{
						client->field = field + 1;

						Cmd_Fly(100, client);
						PPrintf(client, RADIO_YELLOW, "Your plane has been reloaded");
					}
					else
					{
						PPrintf(client, RADIO_YELLOW, "Your plane is too damaged, exit and restart fly");
					}
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "You are too far from field");
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "You cannot reload at this field");
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "You must stop your plane to reload it");
		}
	}
	else
	{
		PPrintf(client, RADIO_YELLOW, "You must wait %s to reload", Com_TimeSeconds(600 - distance));
	}
}

/*************
 Cmd_CheckWaypoints

 Verify CV waypoints for stuck
 *************/

void Cmd_CheckWaypoints(client_t *client)
{
	u_int8_t i, j;

	for (i = 0; i < cvs->value; i++)
	{
		ReadCVWaypoints(i);

		for (j = 1; j < arena->cv[i].wptotal; j++)
		{
			if (!IsVisible(arena->cv[i].wp[j][0], arena->cv[i].wp[j][1], 5, arena->cv[i].wp[j - 1][0], arena->cv[i].wp[j - 1][1], 5))
			{
				PPrintf(client, RADIO_YELLOW, "CV[%u] WP [%u -> %u] error", i, j - 1, j);
			}
		}

		if (!IsVisible(arena->cv[i].wp[j - 1][0], arena->cv[i].wp[j - 1][1], 5, arena->cv[i].wp[1][0], arena->cv[i].wp[1][1], 5))
			PPrintf(client, RADIO_YELLOW, "CV[%u] WP [%u -> 1] error", i, j - 1);
	}
}

void Cmd_Flare(client_t *client)
{
	rocketbomb_t *drop;
	u_int8_t i, j;
	u_int8_t buffer[31];

	memset(buffer, 0, sizeof(buffer));

	PPrintf(client, RADIO_YELLOW, "You fired a flare!");

	drop = (rocketbomb_t *) buffer;

	drop->packetid = htons(Com_WBhton(0x1900));
	drop->item = 61;//155, 61
	drop->id = htons(0x01F9);
	drop->posx = htonl(client->posxy[0][0]);
	drop->posy = htonl(client->posxy[1][0]);
	drop->alt = htonl(client->posalt[0]);
	drop->xspeed = htons(0);
	drop->yspeed = htons(-10);
	drop->zspeed = htons(70);
	drop->unknown1 = htonl(0x20);
	drop->shortnick = htonl(client->shortnick);

	
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
					SendPacket(buffer, sizeof(buffer), &clients[i]);
					break;
				}
			}
		}
	}
	
	SendPacket(buffer, sizeof(buffer), client);
}

void Cmd_Rocket(int32_t y, double angle, double angle2, client_t *client)
{
	rocketbomb_t *drop;
	u_int8_t buffer[31];
	int16_t velx, vely, velz;

	memset(buffer, 0, sizeof(buffer));

	velz = 70 * cos(Com_Rad(angle));
	velx = velz * sin(Com_Rad(angle2));
	vely = velz * cos(Com_Rad(angle2));
	velz = 70 * sin(Com_Rad(angle));

	PPrintf(client, RADIO_YELLOW, "X %d Y %d Z %d", velx, vely, velz);

	drop = (rocketbomb_t *) buffer;

	drop->packetid = htons(Com_WBhton(0x1900));
	drop->item = y;//155, 61
	drop->id = htons(0x01F9);
	drop->posx = htonl(client->posxy[0][0]);
	drop->posy = htonl(client->posxy[1][0]);// - y);
	drop->alt = htonl(client->posalt[0]);
	drop->xspeed = htons(velx);
	drop->yspeed = htons(vely);
	drop->zspeed = htons(velz);
	drop->unknown1 = htonl(0x20);
	drop->shortnick = htonl(client->shortnick);

	SendPacket(buffer, sizeof(buffer), client);
}

void Cmd_Sink(u_int16_t a, u_int16_t b, client_t *client)
{
	sinkboat_t *sink;
	u_int8_t buffer[6];

	memset(buffer, 0, sizeof(buffer));

	sink = (sinkboat_t *) buffer;

	sink->packetid = htons(Com_WBhton(0x0304));
	sink->task = htons(a);
	sink->boat = htons(b);

	SendPacket(buffer, sizeof(buffer), client);
}

void Cmd_CheckBuildings(client_t *client)
{
	u_int16_t i, j;

	for (i = 0; i < fields->value; i++)
	{
		for (j = 0; j < MAX_BUILDINGS; j++)
		{
			if (arena->fields[i].buildings[j].field)
			{
				if (arena->fields[i].buildings[j].field != (i + 1))
					PPrintf(client, RADIO_LIGHTYELLOW, "Building id %d f%d field %d", arena->fields[i].buildings[j].id, i + 1, arena->fields[i].buildings[j].field);
			}
		}
	}
}
