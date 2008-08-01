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
 ******************************
 * 
 *  Server model originally from:
 *
 *  **  The following lines was sent by e-mail from Markus himself to add in this program
 *  *
 *  *  Copyright 2003 Markus "itmo" Mikkolainen, for educational purposes only
 *  *  released under BSD license (mention my name if you want to copy/use it as is)
 *  *  (guaranteed not to compile)
 *  *
 *  **
 *
 *  Author (Francisco "Franz" Bischoff) speech:
 *
 *     This software was made for educational and entertainment purposes only and must not
 *     be used for commercial exploitation, even releasing the source code. If you
 *     intend to modify this source code keep a citation of the original source and
 *     a link for download your and the original software/source.
 *
 *     The code is all licensed under GNU Affero General Public License as expressed above.
 *
 *     This program comes with ABSOLUTELY NO WARRANTY, so please don't annoy me if anything
 *     strange happened, keep sure that isn't your fault before contact me. If you have doubts
 *     about this software check out our homepage. If you're still with doubts you can
 *     contact me by e-mail.
 *
 *     Thanks to Markus Mikkolainen who taught me the bases of starting to code a server.
 *     (don't worry, other thanks you can find at our homepage).
 *
 *     OBS: This source code was compiled sucessfully with linux gcc and cygwin gcc, tested in
 *     Linux Slackware, Debian, RedHat, Ubuntu and Windows.
 *
 *     Our homepage: http://tabahost.dreamhosters.com
 *     How to contact me: tabahost@gmail.com
 *
 *
 *  **
 *  *  Some important thing: It's fundamental to say that no reverse engineering was
 *  *  used to develop this server. We used the so called "French Caffe" techique, as SAMBA
 *  *  project, and no source code was provided by either iEN or Russian FreeHost Team. It
 *  *  was built from scratch with only some theorical contributions from Itmo to create the
 *  *  server main loop (at beginning of project, since I had never created a server before)
 *  *  and lots of book reading.
 *  **
 *
 ***/

#include "shared.h"

int sockfd, udpfd; // local socket
u_int8_t stop = 0;

/*************
main

Here the program begins
*************/

int main(int argc, char *argv[])
{
	int32_t newsocket;
	u_int32_t clisize;
#ifdef _WIN32
	int32_t err;
#endif	
	u_int8_t sync;
	u_long ioctlv;
/*	u_int32_t time, oldtime;*/
	int32_t time;
	u_int32_t oldtime, checksync;
	struct sockaddr_in cli_addr;
	char buffer[128];
	

	InitVars(); // SV_Init --> sv_main.c
	Sys_Init();

	Cmd_LoadConfig("config", NULL);
	Cmd_LoadConfig("autoexec", NULL);

	for(newsocket = 1; newsocket < argc; newsocket++)
	{
		if(*argv[newsocket] == '+')
			Var_Set(argv[newsocket]+1, argv[newsocket+1]);

	}

//	Cmd_LoadConfig(mapname->string, client);

	// vars that cannot be changed after start
	Var_Get("port", port->string, VAR_NOSET);
	Var_Get("maxclients", maxclients->string, VAR_NOSET);
	Var_Get("maxentities", maxentities->string, VAR_NOSET);
	//Var_Get("sqlserver", sqlserver->string, VAR_NOSET);
	//Var_Get("database", database->string, VAR_NOSET);
	Var_Get("hostdomain", hostdomain->string, VAR_NOSET);	
	//// vars set in each config.cfg
	Var_Get("fields", fields->string, VAR_NOSET | VAR_ARCHIVE);
	Var_Get("cities", cities->string, VAR_NOSET | VAR_ARCHIVE);
	Var_Get("cvs", cvs->string, VAR_NOSET | VAR_ARCHIVE);
	Var_Get("mapname", mapname->string, VAR_NOSET | VAR_ARCHIVE);
	Var_Get("dirname", dirname->string, VAR_NOSET | VAR_ARCHIVE);
	Var_Get("wb3", wb3->string, VAR_NOSET);

	sockfd = InitTCPNet(port->value);
	udpfd = InitUDPNet(port->value);
	
	clisize = sizeof(cli_addr);
	ioctlv = 1;
	
	Com_Printf("***************************************************************\n");
	Com_Printf("Tabajara Host Copyright (C) 2004-2008 Francisco Bischoff\n");
	Com_Printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
	Com_Printf("This is free software, and you are welcome to redistribute\n");
	Com_Printf("it under certain conditions; For details type 'license' command\n");
	Com_Printf("***************************************************************\n");
	Com_Printf("Starting Server. Version: %s - Build %s\n", VERSION, __DATE__); //BUILD);
	Com_Printf("***************************************************************\n");

	Sys_SQL_Init(); // startting SQL after InitTCPNet 'cause WSAStartup() in windows
	Sys_GeoIP_Init(); // Starts GeoIP module

	InitClients();
	InitArena();
	ChangeArena(dirname->string, NULL);
	
	Cmd_LoadConfig("config", NULL);
	arena->day = currday->value;
	arena->month = currmonth->value;
	arena->year = curryear->value;
	
	LoadDamageModel(NULL);
	
	if(rps->value)
	{
		Cmd_Seta("f-1", 0, -1, 0); // set 0 to all planes in all fields
		sprintf(buffer, "./arenas/%s/planes", dirname->string);
		LoadRPS(buffer, NULL);
		UpdateRPS();
	}
	
	LoadAmmo(NULL);
	if(wb3->value)
		LoadMapcycle("wb3mapcycle", NULL);
	else
		LoadMapcycle("mapcycle", NULL);

	LoadArenaStatus("arena", NULL, 0);

#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess() , DEBUG_PROCESS);//REALTIME_PRIORITY_CLASS);
	Com_Printf("Process priority set to REAL TIME\n");
#else
	setpriority(PRIO_PROCESS, 0, -10); // Set priority to above normal
#endif

	oldtime = Sys_Milliseconds();
	sync = 10;

	while (!stop)
	{
		do
		{
			if(sync)
#ifdef _WIN32
				sleep(1);
#else
				usleep(1);
#endif
			arena->time = Sys_Milliseconds();
			if(oldtime > arena->time)
				Com_Printf("WARNING: oldtime > arena->time (%u, %u)\n", oldtime, arena->time);
				
			time = arena->time - oldtime;
		} while (time < sync); // server fps = 100

		if(arena->frame < (u_int32_t) 429496200UL /*49 days*/) // change: reset Sys_Milliseconds and frame, every changearena
		{
			arena->frame++;
		}
		else
		{
			arena->frame = 1; // reset frames when max divisible by one minute (600 decasecs)
			arena->time = Sys_ResetMilliseconds();
			Com_Printf("DEBUG: Time Reset\n");
		}

		checksync = FloorDiv(arena->time, 10);
			
		if(checksync > arena->frame)
		{
			sync = 0;
			if((checksync - arena->frame) > 30000)
			{
				Com_Printf("WARNING: clock warp\n");
				arena->frame= checksync;
			}
		}
		else
		{
			sync = 10;
		}

		if(arena->frame > checksync)
		{
			Com_Printf("WARNING: server frame unsynchronized\n");
			arena->frame = checksync;
		}

		if(time > overload->value)
		{
			Com_Printf("WARNING: possible server overload (%d ms / %d ms)\n", time, (int32_t)overload->value);
		}
			
/*		} while ((u_int8_t)time < sync); // server fps = 100 // change: (int32_t) time

		if(arena->frame < (u_int32_t) 4294962000UL) // change: reset Sys_Milliseconds and frame, every changearena
			arena->frame++;
		else
			arena->frame = 1; // reset frames when max divisible by one minute (600 decasecs)

		if((arena->time/10) > arena->frame) // change: floor()
			sync = 0;
		else
			sync = 10;

		if(arena->frame > (arena->time/10)) // change: floor()
			Com_Printf("WARNING: server frame unsynchronized\n");

		if(time > overload->value) // change: || < 0
		{
			Com_Printf("WARNING: possible server overload (%d ms / %d ms)\n", time, (int32_t)overload->value);
		}*/

		if ((newsocket = accept(sockfd, (struct sockaddr *) &cli_addr, &clisize)) == -1)
		{
#ifdef _WIN32
			if ((err = WSAGetLastError()) != WSAEWOULDBLOCK)
			{
				Com_Printf("ERROR: accept()\n");
//				ConnError(err);
				ExitServer(1);
			}
#else
			if (errno != EWOULDBLOCK)
			{
				Com_Printf("ERROR: accept()\n");
//				ConnError(errno);
				ExitServer(1);
			}
#endif
		}
		else
		{
#ifdef _WIN32
			if (ioctlsocket(newsocket, FIONBIO, &ioctlv) == SOCKET_ERROR)
			{
				Com_Printf("ERROR: setting nonblocking socket (newsocket) (code %d)\n", (err = WSAGetLastError()));
				ConnError(err);
				ExitServer(1);
			}
#else
			if (ioctl(newsocket, FIONBIO, &ioctlv) == -1)
			{
				Com_Printf("ERROR: setting nonblocking socket (newsocket) (code %d)\n", errno);
				ConnError(errno);
				ExitServer(1);
			}
#endif
			AddClient(newsocket, &cli_addr);
		}

		RunFrame();
		UpdateLocalArenaslist();
		oldtime = arena->time; // change: why this is here and not up there?
	}
#ifdef _WIN32
	sleep(1000);
#else
	sleep(1);
#endif
	return 0;
}


/*************
UpdateLocalArenaslist

Recv all UDP data and check if there is another arena running in the world
*************/
void UpdateLocalArenaslist(void)
{
	struct sockaddr_in cli_addr;
	u_int32_t cli_len;
	int16_t n;
	char buffer[MAX_UDPDATA];
	char *a_mapname, *a_hostdomain, *a_hostname;
	u_int32_t a_numplayers, a_maxclients, curr_time;
	u_int16_t offset;
	u_int8_t i, j;

	cli_len = (u_int32_t)sizeof(struct sockaddr_in);

	memset(buffer, 0, sizeof(buffer));

	if((n = recvfrom(udpfd, buffer, MAX_UDPDATA, 0, (struct sockaddr *)&cli_addr, &cli_len)) > 0)
	{
		if (ntohl(*(long *) buffer) == n)
		{
			offset = 4;

			a_mapname = (char *)(buffer + offset);
			offset += strlen(a_mapname) + 1;
			a_hostdomain = (char *)(buffer + offset);
			offset += strlen(a_hostdomain) + 1;
			a_hostname = (char *)(buffer + offset);
			offset += strlen(a_hostname) + 1;
			a_numplayers = ntohl(*(long *)(buffer + offset));
			offset += 4;
			a_maxclients = ntohl(*(long *)(buffer + offset));
			offset += 4;

			curr_time = time(NULL);

			for(i = 0; i < MAX_ARENASLIST; i++)
			{
				if((arenaslist[i].time || arenaslist[i].logintime) && ((curr_time - arenaslist[i].time) > 10)) // 10 seconds timeout
				{
					arenaslist[i].logintime = 0;
					arenaslist[i].time = 0;
				}
			}


			for(i = 0, j = 0; i < MAX_ARENASLIST; i++)
			{
				if(arenaslist[i].time) // slot filled
				{
					if(strlen(a_hostdomain) && strlen(arenaslist[i].hostdomain))
					{
						if(!strcmp(a_hostdomain, arenaslist[i].hostdomain)) // arena match
						{
							j = i;
							break;
						}
					}
				}
				else
				{
					j = i; // count empty slot
				}
			}

			if(!(!j && i)) // found arena or not found but still empty slot
			{
				// update arena status
				strcpy(arenaslist[j].mapname, a_mapname);
				strcpy(arenaslist[j].hostdomain, a_hostdomain);
				strcpy(arenaslist[j].hostname, a_hostname);
				arenaslist[j].numplayers = a_numplayers;
				arenaslist[j].maxclients = a_maxclients;
				memcpy(arenaslist[j].array, (buffer + offset), ((a_numplayers * 5) > 1000)?1000:(a_numplayers * 5));
				arenaslist[j].time = curr_time;
				if(!arenaslist[j].logintime)
					arenaslist[j].logintime = curr_time;
			}
		}
	}
}


/*************
RunFrame

Run server Frame, processing arena rules and every existing client (must be done in not more than 0.1 sec)
*************/

void RunFrame(void)
{
	char *console;
	int32_t i;
	static u_int32_t all, arn, cli;
	u_int32_t time_before = 0, time_after_arena = 0, time_after_clients = 0;

	if(server_speeds->value)
		time_before = Sys_Milliseconds();

	CheckVars();

	if(!(arena->frame % 3000))
	{
		if(mysql_ping(&my_sock))
		{
			Com_Printf("WARNING: RunFrame(): mysql_ping() error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

			Com_Printf("WARNING: RunFrame(): Resseting MySQL connection\n");

			sqlserver->modified = 1;
			
//			if(mysql_errno(&my_sock) == 2006)
//			{
//				if(!mysql_real_connect(&my_sock, sqlserver->string, dbuser->string, dbpasswd->string, database->string, 3306, NULL /*unix_socket*/, 0))
//				{
//					Com_Printf("ERROR: RunFrame(): Failed to connect to %s, Error %s \n", sqlserver->string, mysql_error(&my_sock));
//				}
//				else
//				{
//					Com_Printf("MySQL connection successfully restored\n");
//				}
//			}
		}
	}

	if(!(arena->frame % 12000))
	{
		UpdateArenaStatus(TRUE);
	}

//	if(!setjmp(debug_buffer))
//	{
		CheckArenaRules();
//	}
//	else
//	{
//		DebugArena(__FILE__, __LINE__);
//	}

	if(server_speeds->value)
		time_after_arena = Sys_Milliseconds();

	for(i = 0; i < maxentities->value; i++)
	{
		if(clients[i].inuse)
		{
			if(clients[i].drone)
			{
				if(!setjmp(debug_buffer))
				{
					if(ProcessDrone(&clients[i]) < 0)
					{
						RemoveDrone(&clients[i]);
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, &clients[i]);
				}
			}
			else
			{
				if(!setjmp(debug_buffer))
				{
					if(ProcessClient(&clients[i]) < 0)
					{
						if(clients[i].infly)
						{
							if(IsFighter(&clients[i]))
								sprintf(my_query, "UPDATE score_fighter");
							else if(IsBomber(&clients[i]))
								sprintf(my_query, "UPDATE score_bomber");
							else if(IsGround(&clients[i]))
								sprintf(my_query, "UPDATE score_ground");
							else
							{
								Com_Printf("WARNING: Plane not classified (N%d)\n", clients[i].plane);
								sprintf(my_query, "UPDATE score_fighter");
							}
							
							sprintf(my_query, "%s SET disco = disco + '1' WHERE player_id = '%u'", my_query, clients[i].id);
							
							if(d_mysql_query(&my_sock, my_query))
							{
								PPrintf(&clients[i], RADIO_YELLOW, "Disco: SQL Error (%d), please contact admin", mysql_errno(&my_sock));
								Com_Printf("WARNING: Disco: couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							}

							Com_LogEvent(EVENT_DISCO, clients[i].id, 0);
							Com_LogDescription(EVENT_DESC_PLIP, 0, clients[i].ip);
							Com_LogDescription(EVENT_DESC_PLCTRID, clients[i].ctrid, 0);
						}
						
						if(strlen(clients[i].longnick))
							UpdateClientFile(&clients[i]);
						
						RemoveClient(&clients[i]);
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, &clients[i]);
				}
			}
		}
	}

	if(consoleinput->value)
	{
		do
		{
			console = Sys_ConsoleInput();
			if (console)
			{
				ProcessCommands(console, NULL);
			}
		} while(console);
	}

	if(server_speeds->value)
	{
		time_after_clients = Sys_Milliseconds();	
		
		all += (time_after_clients - time_before);
		arn += (time_after_arena - time_before);
		cli += (time_after_clients - time_after_arena);

		if(!(arena->frame % (int)(100/server_speeds->value)))
		{
			printf("all:%3i arn:%3i cli:%3i sent: %.3f recv: %.3f time: %u\n", all, arn, cli, (float)arena->sent/1000, (float)arena->recv/1000, arena->time);
			fflush(stdout);

			all = arn = cli = 0;
			arena->sent = 0;
			arena->recv = 0;
		}
	}
}


/*************
BackupArenaStatus

Make a backup of arena status for system crashes
*************/

void BackupArenaStatus(void)
{
	char date[8];

	if(arena)
	{
		sprintf(date, "%d", arena->year);
		Var_Set("curryear", date);
		sprintf(date, "%d", arena->month);
		Var_Set("currmonth", date);
		sprintf(date, "%d", arena->day);
		Var_Set("currday", date);
		SaveArenaStatus("arena", NULL);
	}

	Var_WriteVariables("config", NULL);

	if(logfile)
	{
		fflush(logfile);
//		fclose(logfile);
//		logfile = NULL;
	}
}

/*************
ExitServer

Exits program and return status termination
*************/

void ExitServer(int status)
{
	u_int16_t i;
	var_t *var;

	if(!stop)
		stop = 1;
	else
		return;

	Com_Printf("Beginning shutdown sequence\n"); /*****************/

	Com_Printf("Writing variables table\n"); /*********/

	BackupArenaStatus();

	Sys_GeoIP_Close();
	
	Sys_SQL_Close();

	Com_Printf("Closing server socket\n"); /*********/

	if(sockfd)
		Com_Close(&sockfd);

#ifdef _WIN32
	WSACleanup();
#endif

	Com_Printf("Closing clients sockets\n"); /*********/

	if(clients)
	{
		for(i = 0; i < maxentities->value; i++)
		{
			if(clients[i].inuse && !clients[i].drone && clients[i].socket)
			{
				Com_Close(&(clients[i].socket));
			}
		}
	}

	Com_Printf("Freeing Memory\n"); /*********/

// free clients
	free(clients);

// free arena
	if(arena)
	{
		free(arena->fields);
		free(arena->cities);
	}

	if(arena)
	{
		free(arena);
	}

	// free vars

	while(var_vars)
	{
		free(var_vars->name);
		free(var_vars->string);
		var = var_vars->next;
		free(var_vars);
		var_vars = var;
	}

	/////////////

	Com_Printf("Closing opened files\n"); /*********/
	Com_Printf("Exiting Server\n");

	if(logfile)
	{
		fclose(logfile);
		logfile = NULL; // duh
	}

	printf("Server sucessfuly closed\n"); /*****************/
	fflush(stdout);

	exit(status);
}

/*
# logar quantas vezes o cliente foi atendido por segundo
*/
