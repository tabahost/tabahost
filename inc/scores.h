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
  * @file		/include/scores.h
  * @author		Francisco Bischoff
  * @version	11.2011
  * @date		2011.11.28
  * @brief		scores header file
  */

#ifndef SCORES_H_
#define SCORES_H_

void	ScoresEvent(u_int16_t event, client_t *client, int32_t misc);
double	ScorePlaneCost(client_t *client);
double	ScoreFixPlaneCost(double plane_life, double plane_cost);
double	ScorePlaneTransportCost(client_t *client);
double	ScorePilotTransportCost(client_t *client);
double	ScoreFlightTimeCost(client_t *client);
double	ScoreDamageCost(client_t *client);
double	ScorePlaneLife(client_t *client);
void	ScoresEndFlight(u_int16_t end, int8_t land, u_int16_t gunused, u_int16_t totalhits, client_t *client);
int8_t	ScoresCheckKiller(client_t *client, int32_t *maneuver);
u_int8_t ScoresCheckMedals(client_t *client);
u_int8_t ScoresAddMedal(u_int8_t deed, u_int8_t medal, u_int16_t value, client_t *client);
u_int8_t ScoresCheckCaptured(client_t *client);
void	ScoresCreate(client_t *client);
void	ResetScores(void);
void	BackupScores(u_int8_t collect_type);
double	ScoreTechnologyCost(client_t *client);
double	GetBuildingCost(u_int8_t type);
double	GetAmmoCost(u_int8_t type);
double	GetFieldCost(u_int8_t field);
void	ScoreFieldCapture(u_int8_t field);
double	ScorePieceDamage(int8_t killer, double event_cost, client_t *client);
void	ScoreLoadCosts(void);

#endif /* SCORES_H_ */
