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
  * @file		/include/sys.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		system header file
  */

#ifndef SYS_H_
#define SYS_H_

u_int32_t Sys_Milliseconds (void);
u_int32_t Sys_ResetMilliseconds(void);
void	Sys_Init(void);
void	Sys_PrintTrace (void);
void	Sys_RemoveFiles(const char *file);
void	Sys_SQL_Init(void);
void	Sys_SQL_Close(void);
void	Sys_GeoIP_Init(void);
void	Sys_GeoIP_Close(void);
void	Sys_SigHandler(int s);
char	*Sys_ConsoleInput(void);
int8_t	Sys_LockFile(const char *file);
int8_t	Sys_UnlockFile(const char *file);
void	Sys_WaitForLock(const char *file);
void	Sys_Printfile(const char *file);

#endif /* SYS_H_ */
