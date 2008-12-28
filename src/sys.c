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

#ifdef _WIN32
#include "mmsystem.h"
#include "io.h"
#else
#include "unistd.h"
#include "sys/time.h"
#endif

GeoIP *gi; //extern

/*************
 Sys_Milliseconds

 Return a relative milliseconds since first call
 *************/
#ifndef _WIN32
static u_int32_t secbase;
#endif
u_int32_t Sys_Milliseconds(void)
#ifdef _WIN32
{
	u_int32_t curtime;
	static int base;
	static u_short initialized = 0;

	if (!initialized)
	{
		base = timeGetTime(); // & 0xffff0000;
		initialized = 1;
	}

	curtime = timeGetTime() - base;

	return curtime;
}
#else
{
	struct timeval tp;
	struct timezone tzp;
	u_int32_t curtime;

	gettimeofday(&tp, &tzp);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - secbase)*1000 + tp.tv_usec/1000;

	return curtime;
}
#endif

u_int32_t Sys_ResetMilliseconds(void)
{
	struct timeval tp;
	struct timezone tzp;

	gettimeofday(&tp, &tzp);

	secbase = tp.tv_sec;

	return tp.tv_usec/1000;
}

#ifdef _WIN32
static HANDLE hinput, houtput;
static char console_text[256];
static int console_textlen;
#endif

/*************
 Sys_Init

 Initializes System functions
 *************/

void Sys_Init(void)
{
	u_int8_t i;
	//remove LOCK files
#ifndef _WIN32
	Sys_RemoveFiles("./.LOCK");
	Sys_RemoveFiles("./players/.LOCK");
	Sys_RemoveFiles("./squads/.LOCK");

#else
	system("del *.LOCK");
	system("del .\\players\\*.LOCK");
	system("del .\\squads\\*.LOCK");
	// 	system("del .\\fields\\*.LOCK"); // not needed because Cmd_Field() doesnt share info

	//unlink(FILE_INGAME_LOCK);
#endif

#ifdef _WIN32
	if(!FreeConsole())
	Com_Printf(VERBOSE_WARNING, "Couldn't detach console (%u)\n", GetLastError());
	if(!AllocConsole())
	Com_Printf(VERBOSE_WARNING, "Couldn't create dedicated server console (%u)\n", GetLastError());

	hinput = GetStdHandle(STD_INPUT_HANDLE);
	houtput = GetStdHandle(STD_OUTPUT_HANDLE);

	signal(SIGBREAK, Sys_SigHandler); /* Ctrl-Break */
#else
	signal(SIGQUIT, Sys_SigHandler); /* Quit */
	signal(SIGKILL, Sys_SigHandler); /* Killed */
	signal(SIGHUP, Sys_SigHandler); /* Hangup */
	signal(SIGPIPE, Sys_SigHandler); /* Broken pipe */
#endif
	signal(SIGFPE, Sys_SigHandler); /* floating point problem */
	signal(SIGSEGV, Sys_SigHandler); /* segment violation trap 13 */
	signal(SIGABRT, Sys_SigHandler); /* abnormal termination */
	signal(SIGILL, Sys_SigHandler); /* illegal instruction. (Bug in the code generator) */
	signal(SIGTERM, Sys_SigHandler); /* termination signal from kill */
	signal(SIGINT, Sys_SigHandler); /* Interrupt, normally Ctrl-C */
	
	for(i = 0; i < MAX_LOGFILE; i++)
	{
		logfile[i] = NULL;
	}
}

/*************
 Sys_RemoveFiles

 Delete files. Syntaxe: "path/file"
 *************/

void Sys_RemoveFiles(char *pathfile)
{
	FILE *fp;
	DIR *dp;
	struct dirent *ep;
	char path[256];
	char file[64];
	char *temp;

	strncpy(path, pathfile, 256);

	temp = strrchr(path, '/');

	if (!temp)
	{
		Com_Printf(VERBOSE_WARNING, "RemoveFiles(): Invalid pointer\n");
		return;
	}

	strncpy(file, temp+1, 64);

	*temp = 0;
	dp = opendir(path);

	if (dp != NULL)
	{
		while (ep = readdir(dp))
		{
			if (strstr(ep->d_name, file))
			{
				Com_Printf(VERBOSE_ALWAYS, "Deleting file %s/%s\n", path, ep->d_name);
				sprintf(temp, "/%s", ep->d_name);
				unlink(path);
				*temp = 0;
			}
		}
		closedir(dp);
	}
	else
		Com_Printf(VERBOSE_WARNING, "RemoveFiles(): Couldn't open the directory\n");
}

/*************
 Sys_SQL_Init

 Init SQL system
 *************/

void Sys_SQL_Init(void)
{
	u_int32_t port = 3306; /*default = 3306*/
	u_int32_t flags = 0; // CLIENT_MULTI_STATEMENTS

	//	mysql_library_init();

	if (!mysql_init(&my_sock))
	{
		Com_Printf(VERBOSE_ERROR, "Sys_SQL_Init(): Error initializing my_sock\n");
		ExitServer(1);
	}
	else
	{
		Com_Printf(VERBOSE_ALWAYS, "MySQL Initialized\n");
	}

	if (mysql_options(&my_sock, MYSQL_OPT_RECONNECT, "1"))
		Com_Printf(VERBOSE_WARNING, "Sys_SQL_Init(): MYSQL_OPT_RECONNECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

	if (!mysql_real_connect(&my_sock, sqlserver->string, dbuser->string, dbpasswd->string, database->string, port, NULL /*unix_socket*/, flags))
	{
		Com_Printf(VERBOSE_ERROR, "Sys_SQL_Init(): Failed to connect to %s, Error %s \n", sqlserver->string, mysql_error(&my_sock));
		ExitServer(1);
	}
	else
	{
		Com_Printf(VERBOSE_ALWAYS, "MySQL connected successfully to %s:%s\n", sqlserver->string, database->string);
	}
}

/*************
 Sys_SQL_Close

 Stops SQL system
 *************/

void Sys_SQL_Close(void)
{
	Com_Printf(VERBOSE_ALWAYS, "Closing mySQL System\n");

	mysql_close(&my_sock);

	mysql_library_end();
}

/*************
 Sys_GeoIP_Init

 Init GeoIP Module
 *************/

void Sys_GeoIP_Init(void)
{
	if (!(gi = GeoIP_open("GeoIP.dat", GEOIP_MEMORY_CACHE)))//GEOIP_STANDARD)))
	{
		Com_Printf(VERBOSE_ERROR, "Sys_GeoIP_Init(): Error initializing gi\n");
		ExitServer(1);
	}
	else
	{
		Com_Printf(VERBOSE_ALWAYS, "GeoIP Module Initialized\n");
	}
}

void Sys_GeoIP_Close(void)
{
	if (gi)
	{
		Com_Printf(VERBOSE_ALWAYS, "Closing GeoIP Module\n");

		GeoIP_delete(gi);
	}
}

/*************
 Sys_SigHandler

 Handles every got signal
 *************/

void Sys_SigHandler(int s)
{
	int n = 0;

	switch (s)
	{
#ifdef _WIN32
		case SIGBREAK:
		signal(SIGBREAK, Sys_SigHandler);
		Com_Printf(VERBOSE_WARNING, "Got signal SIGBREAK\n");
		break;
#else
		case SIGQUIT:
			signal(SIGQUIT, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGQUIT\n");
			break;
		case SIGKILL:
			signal(SIGKILL, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGKILL\n");
			n = 1;
			break;
		case SIGHUP:
			signal(SIGHUP, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGHUP\n");
			return;
		case SIGPIPE:
			signal(SIGPIPE, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGPIPE\n");
			return;
#endif
		case SIGFPE:
			signal(SIGFPE, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGFPE\n");

#ifdef _WIN32
			//		    _fpreset(); //we can clear math proc state with this function under Win32,
			//if really needed with the view of "total cleaner" longjmp()
#endif
			longjmp(debug_buffer, 1);

			n = 1;
			break;
		case SIGSEGV:
			signal(SIGSEGV, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGSEGV\n");
			longjmp(debug_buffer, 1);
			n = 1;
			break;
		case SIGABRT:
			signal(SIGABRT, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGABRT\n");
			n = 1;
			break;
		case SIGILL:
			signal(SIGILL, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGILL\n");
			n = 1;
			break;
		case SIGTERM:
			signal(SIGTERM, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGTERM\n");
			n = 1;
			break;
		case SIGINT:
			signal(SIGINT, Sys_SigHandler);
			Com_Printf(VERBOSE_WARNING, "Got signal SIGINT\n");
			if (!consoleinput->value)
			{
				Var_Set("consoleinput", "1");
				return;
			}
			break;
	}

	Com_Printf(VERBOSE_ERROR, "Got signal (%d), shutting down...\n", s);

	ExitServer(n);

}

/*************
 Sys_ConsoleInput

 Get any typed data in stdin and return the string received newline
 *************/

#ifdef _WIN32
char *Sys_ConsoleInput (void)
{
	INPUT_RECORD records[1024];
	char ch;
	u_int32_t numevents, numread;
	u_int32_t temp;

	for(;;)
	{
		if(!GetNumberOfConsoleInputEvents (hinput, &numevents))
		{
			Com_Printf(VERBOSE_WARNING, "Error getting # of console events\n");
			ExitServer(1);
		}

		if(numevents <= 0)
		break;

		if(!ReadConsoleInput(hinput, records, 1, &numread))
		Com_Printf(VERBOSE_WARNING, "Error reading console input\n");

		if(numread != 1)
		Com_Printf(VERBOSE_WARNING, "Couldn't read console input\n");

		if(records[0].EventType == KEY_EVENT)
		{
			if(!records[0].Event.KeyEvent.bKeyDown)
			{
				ch = records[0].Event.KeyEvent.uChar.AsciiChar;

				switch(ch)
				{
					case '\r':
					WriteFile(houtput, "\r\n", 2, &temp, NULL);

					if(console_textlen)
					{
						console_text[console_textlen] = 0;
						console_textlen = 0;
						return console_text;
					}
					break;

					case '\b':
					if(console_textlen)
					{
						console_textlen--;
						WriteFile(houtput, "\b \b", 3, &temp, NULL);
					}
					break;

					default:
					if(ch >= ' ')
					{
						if(console_textlen < sizeof(console_text)-2)
						{
							WriteFile(houtput, &ch, 1, &temp, NULL);
							console_text[console_textlen] = ch;
							console_textlen++;
						}
					}
					break;
				}
			}
		}
	}

	return NULL;
}
#else
char *Sys_ConsoleInput(void)
{
	static char buff[256];
	int len;
	fd_set fdset;
	struct timeval timeout;

	FD_ZERO(&fdset);
	FD_SET(0, &fdset); // stdin

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	if (select(1, &fdset, NULL, NULL, &timeout) == -1 || !FD_ISSET(0, &fdset))
		return NULL;
	// http://www.opengroup.org/onlinepubs/000095399/functions/read.html

	len = read(0, buff, sizeof(buff));

	if (!len) // EOF
		return NULL;

	if (len < 1)
		return NULL;

	buff[len - 1] = 0; // cut off \n

	return buff;
}
#endif

/*************
 LockFile

 Create a Lock file
 *************/

int8_t Sys_LockFile(char *file)
{
	FILE *fp;

	if (!(fp = fopen(file, "w")))
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't create file \"%s\"\n", file);
		return -1;
	}
	else
	{
		fclose(fp);
		return 0;
	}
}

/*************
 UnlockFile

 Remove a Lock file
 *************/

int8_t Sys_UnlockFile(char *file)
{
	return unlink(file);
}

/*************
 WaitForLock

 Waits till LOCK file is gone
 *************/

void Sys_WaitForLock(char *file)
{
	FILE *fp;

	while ((fp = fopen(file, "r")))
	{
		fclose(fp);
#ifdef _WIN32
		sleep(1);
#else
		usleep(1);
#endif
	}
}

/*************
 Sys_Printfile

 Print a file in console
 *************/

void Sys_Printfile(char *file)
{
	char lock[128];
	char buffer[1024];
	FILE *fp;

	if (strlen(file)+6 > sizeof(lock))
		return;

	memset(lock, 0, sizeof(lock));
	memset(buffer, 0, sizeof(buffer));

	strcpy(lock, file);
	strcat(lock, ".LOCK");

	Sys_WaitForLock(lock);

	Sys_LockFile(lock);

	if (!(fp = fopen(file, "r")))
	{
		printf("WARNING: Couldn't open file \"%s\"", file);
		fflush(stdout);
		Sys_UnlockFile(lock);
		return;
	}
	else
	{
		while (fgets(buffer, sizeof(buffer), fp))
		{
			printf("%s", buffer);
			fflush(stdout);
		}
	}

	fclose(fp);

	Sys_UnlockFile(lock);
}

/*
 if(!d_mysql_query(&my_sock, my_query)) // query succeeded
 {
 if((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
 {
 if((my_row = mysql_fetch_row(my_result)))
 {
 DOTHINGS
 }
 else
 {
 Com_Printf(VERBOSE_WARNING, "UNNAMED: Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));						
 }
 
 mysql_free_result(my_result);
 my_result = NULL;
 my_row = NULL;
 }
 else
 {
 Com_Printf(VERBOSE_WARNING, "UNNAMED: my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
 }				
 }
 else
 {
 Com_Printf(VERBOSE_WARNING, "UNNAMED: couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
 }
 */
