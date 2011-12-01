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
  * @file		/include/vars.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		variables header file
  */

#ifndef VARS_H_
#define VARS_H_

void	InitVars(void);
void	CheckVars(void);
var_t	*Var_SetFlags(const char *var_name, int flags);
var_t	*Var_Get(const char *var_name, const char *var_value, const char *min, const char *max, const char *description, int flags);
var_t	*Var_FindVar(const char *var_name);
var_t	*Var_Set (const char *var_name, const char *value);
double	Var_VariableValue (const char *var_name);
const char	*Var_VariableString (const char *var_name);
void	Var_WriteVariables (const char *path, client_t *client);
u_int8_t UpdateArenaStatus(u_int8_t uptime);

#endif /* VARS_H_ */
