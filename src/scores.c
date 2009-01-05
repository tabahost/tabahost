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
 ScoresEvent

 nononono
 *************/

void ScoresEvent(u_int16_t event, client_t *client)
{
	u_int8_t captured = 0;
	float score = 0.0;

	if(event & SCORE_CAPTURED)
	{
		event ^ SCORE_CAPTURED
		captured = 1;
	}

	switch (event)
	{
		case SCORE_TAKEOFF:
				score += takeoff_cost;
			break;
		case SCORE_LANDED:
				if(captured)
					score += ScorePlaneCost(client) + newpilot_cost + informationlost_cost + technologylost_cost - ScoreDamageCost(client);
				else
					score += flighttime_cost + ScoreDamageCost(client);
			break;
		case SCORE_DITCHED:
				if(captured)
					score += ScorePlaneCost(client) + newpilot_cost + informationlost_cost + technologylost_cost - ScoreDamageCost(client);
				else
					score += flighttime_cost + ScoreDamageCost(client) + ScoreTransportCost(client);
			break;
		case SCORE_BAILED:
			break;
		case SCORE_KILLED:
			break;
	}

	ScoresCheckKiller(client);
}

/*************
 ScoresEndFlight

 Check the type of end flight, guns used and hit, and credit scores
 *************/

void ScoresEndFlight(u_int16_t end, int8_t land, u_int16_t gunused, u_int16_t totalhits, client_t *client)
{
	u_int16_t i;
	u_int8_t j;
	char buffer[100];

	switch (end)
	{
		case ENDFLIGHT_LANDED:
			if(ScoresCheckCaptured(client))
			{
				ScoresEvent((SCORE_LANDED | SCORE_CAPTURED)); // TODO: give piece points before clean killers
			}
			else
			{
				ScoresEvent(SCORE_LANDED); // TODO: give piece points before clean killers
			}
			break;
		case ENDFLIGHT_DITCHED:
			if(ScoresCheckCaptured(client))
			{
				ScoresEvent((SCORE_DITCHED | SCORE_CAPTURED));
			}
			else
			{
				ScoresEvent(SCORE_DITCHED);
			}
			break;
		case ENDFLIGHT_BAILED:
			if(ScoresCheckCaptured(client))
			{
				ScoresEvent((SCORE_BAILED | SCORE_CAPTURED));
			}
			else
			{
				ScoresEvent(SCORE_BAILED);
			}
			break;
		case ENDFLIGHT_PILOTKILL:
		case ENDFLIGHT_CRASHED:
		case ENDFLIGHT_DITCHFAILED:
		case ENDFLIGHT_PANCAKE:
			ScoresEvent(SCORE_KILLED);
			break;
//		case ENDFLIGHT_COLLIDED:
	}

	// check if time penalized
	if (!(end == ENDFLIGHT_LANDED || end == ENDFLIGHT_DITCHED) && !(end == ENDFLIGHT_BAILED && client->attached))
	{
		if (client->dronetimer < arena->time)
		{
			i = (arena->time - client->dronetimer)/10;
		}
		else
		{
			i = (0xFFFFFFFF - (client->dronetimer - arena->time))/10;
		}

		if (i < (flypenalty->value * 100))
		{
			client->flypenalty = (flypenalty->value * 100) - i;
			client->flypenaltyfield = client->field;
		}
	}

	// debite guns used from client
	
	if (IsGround(client))
	{
		client->score.groundscore += (float)(SCORE_BULLET * gunused);
	}
	else
	{
		client->score.airscore += (float)(SCORE_BULLET * gunused);
	}

/////////////////////
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
		Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
		sprintf(my_query, "UPDATE score_fighter SET");
	}

	if (end == ENDFLIGHT_BAILED)
	{
		if (IsFighter(client) || IsBomber(client))
		{
			strcat(my_query, " bailed = bailed + '1'");
			if (client->score.airscore > 0)
				client->score.airscore /= 2;
			else
				client->score.airscore *= 2;

			if (client->score.groundscore > 0)
				client->score.groundscore /= 2;
			else
				client->score.groundscore *= 2;

			if (client->score.captscore > 0)
				client->score.captscore /= 2;
			else
				client->score.captscore *= 2;

			if (client->score.rescuescore > 0)
				client->score.rescuescore /= 2;
			else
				client->score.rescuescore *= 2;
		}
		else
		{
			strcat(my_query, " landed = landed + '1'");
			if (client->score.airscore > 0)
				client->score.airscore *= 2;
			else
				client->score.airscore /= 2;

			if (client->score.groundscore > 0)
				client->score.groundscore *= 2;
			else
				client->score.groundscore /= 2;

			if (client->score.captscore > 0)
				client->score.captscore *= 2;
			else
				client->score.captscore /= 2;

			if (client->score.rescuescore > 0)
				client->score.rescuescore *= 2;
			else
				client->score.rescuescore /= 2;
		}
	}
	else if (end == ENDFLIGHT_DITCHED)
	{
		if (IsFighter(client) || IsBomber(client) || (wb3->value && IsGround(client)))
			strcat(my_query, " ditched = ditched + '1'");
		else
		{
			strcat(my_query, " landed = landed + '1'");
			if (client->score.airscore > 0)
				client->score.airscore *= 2;
			else
				client->score.airscore /= 2;

			if (client->score.groundscore > 0)
				client->score.groundscore *= 2;
			else
				client->score.groundscore /= 2;

			if (client->score.captscore > 0)
				client->score.captscore *= 2;
			else
				client->score.captscore /= 2;

			if (client->score.rescuescore > 0)
				client->score.rescuescore *= 2;
			else
				client->score.rescuescore /= 2;
		}
	}
	else if ((end == ENDFLIGHT_PILOTKILL && !client->chute) || (end == ENDFLIGHT_CRASHED && !client->chute) || (end == ENDFLIGHT_DITCHFAILED
		&& !client->chute) || (end == ENDFLIGHT_COLLIDED) || (end == ENDFLIGHT_PANCAKE)) // TODO: remove kill + 1 when collided
	{
		if (end == ENDFLIGHT_COLLIDED)
		{
			strcat(my_query, " collided = collided + '1',");
		}

		if (client->lives > 0)
		{
			client->lives--;
			Cmd_Lives(client->longnick, client->lives, NULL);
		}

		strcat(my_query, " killed = killed + '1', curr_streak = '0'");
		client->streakscore = 0;
		if (client->score.airscore > 0)
			client->score.airscore /= 10;
		else
			client->score.airscore *= 10;

		if (client->score.groundscore > 0)
			client->score.groundscore /= 10;
		else
			client->score.groundscore *= 10;

		if (client->score.captscore > 0)
			client->score.captscore /= 10;
		else
			client->score.captscore *= 10;

		if (client->score.rescuescore > 0)
			client->score.rescuescore /= 10;
		else
			client->score.rescuescore *= 10;
	}
	else// if (end == ENDFLIGHT_LANDED)
	{
		strcat(my_query, " landed = landed + '1'");
		if (client->score.airscore > 0)
			client->score.airscore *= 2;
		else
			client->score.airscore /= 2;

		if (client->score.groundscore > 0)
			client->score.groundscore *= 2;
		else
			client->score.groundscore /= 2;

		if (client->score.captscore > 0)
			client->score.captscore *= 2;
		else
			client->score.captscore /= 2;

		if (client->score.rescuescore > 0)
			client->score.rescuescore *= 2;
		else
			client->score.rescuescore /= 2;
	}

	client->lastscore = client->score.airscore + client->score.groundscore + client->score.captscore + client->score.rescuescore - client->score.penaltyscore;

	if (IsFighter(client))
	{
		if (client->score.airscore)
			sprintf(my_query, "%s, fighter_score = fighter_score + '%.3f'", my_query, client->score.airscore);
		if (client->score.groundscore)
			sprintf(my_query, "%s, jabo_score = jabo_score + '%.3f'", my_query, client->score.groundscore);
	}
	else if (IsBomber(client))
	{
		if (client->score.airscore + client->score.groundscore)
			sprintf(my_query, "%s, bomber_score = bomber_score + '%.3f'", my_query, client->score.airscore + client->score.groundscore);
		if (client->score.captscore)
			sprintf(my_query, "%s, capt_score = capt_score + '%.3f'", my_query, client->score.captscore);
		if (client->score.rescuescore)
			sprintf(my_query, "%s, resc_score = resc_score + '%.3f'", my_query, client->score.rescuescore);
	}
	else if (IsGround(client))
	{
		if (client->score.airscore + client->score.groundscore)
			sprintf(my_query, "%s, ground_score = ground_score + '%.3f'", my_query, client->score.airscore + client->score.groundscore);
		if (client->score.captscore)
			sprintf(my_query, "%s, capt_score = capt_score + '%.3f'", my_query, client->score.captscore);
		if (client->score.rescuescore)
			sprintf(my_query, "%s, resc_score = resc_score + '%.3f'", my_query, client->score.rescuescore);
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
		if (client->score.airscore)
			sprintf(my_query, "%s, fighter_score = fighter_score + '%.3f'", my_query, client->score.airscore);
		if (client->score.groundscore)
			sprintf(my_query, "%s, jabo_score = jabo_score + '%.3f'", my_query, client->score.groundscore);
	}

	if (gunused)
		sprintf(my_query, "%s, gunused = gunused + '%u'", my_query, gunused);
	if (totalhits)
		sprintf(my_query, "%s, gunhits = gunhits + '%u'", my_query, totalhits);

	sprintf(my_query, "%s WHERE player_id = '%u'", my_query, client->id);

	if (d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		Com_Printf(VERBOSE_WARNING, "PEndFlight(update): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		Com_Printf(VERBOSE_WARNING, "Query: %s\n", my_query);
	}

	if (client->score.penaltyscore)
	{
		sprintf(my_query, "UPDATE score_penalty SET penalty_score = penalty_score + '%.3f' WHERE player_id = '%u'", client->score.penaltyscore, client->id);

		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "PEndFlight(penalty): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}

	client->streakscore += client->lastscore;

	if (client->dronetimer < arena->time)
	{
		end = (arena->time - client->dronetimer)/1000;
	}
	else
	{
		end = (0xFFFFFFFF - (client->dronetimer - arena->time))/1000;
	}

	sprintf(my_query, "UPDATE score_common SET killstod = '%u', structstod = '%u', lastscore = '%.3f', totalscore = totalscore + '%.3f', streakscore = '%.3f', flighttime = flighttime + '%u' WHERE player_id = '%u'",
			client->killstod, client->structstod, client->lastscore, client->lastscore, client->streakscore, end, client->id);

	if (d_mysql_query(&my_sock, my_query)) // query succeeded
	{
		Com_Printf(VERBOSE_WARNING, "PEndFlight(updplayer): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}
///////////////////////////////////////////

	// Print Flight Defrief

	PPrintf(client, RADIO_WHITE, "==================================================");
	PPrintf(client, RADIO_WHITE, "Flight time: %s", Com_TimeSeconds(end));
	PPrintf(client, RADIO_WHITE, "Last mission score: %11.3f - %s mission.", client->lastscore, IsFighter(client) ? "Fighter" : IsBomber(client) ? "Bomber" : "Ground");
	PPrintf(client, RADIO_WHITE, "You've shot %d rounds, hit %d (acc:%.3f%%)", gunused, totalhits, gunused ? (float)totalhits*100/gunused : 0);
	if(totalhits)
	{
		memset(buffer, 0, sizeof(buffer));

		for(i = 0, j = 0; i < 6; i++)
		{
			if(client->hits[i])
			{
				sprintf(buffer, "%s%4d [%s]", buffer, client->hits[i],
									i == 0?"7mm":
									i == 1?"13mm":
									i == 2?"20mm":
									i == 3?"30-37mm":
									i == 4?"40-57mm":"75-88mm");

				if(++j >= 3)
				{
					PPrintf(client, RADIO_WHITE, "%s", buffer);
					memset(buffer, 0, sizeof(buffer));
					j = 0;
				}
			}
		}

		if(j)
			PPrintf(client, RADIO_WHITE, "%s", buffer);
	}
	totalhits = client->hitstaken[0] + client->hitstaken[1] + client->hitstaken[2] + client->hitstaken[3] + client->hitstaken[4] + client->hitstaken[5];
	PPrintf(client, RADIO_WHITE, "You've got %d shots", totalhits);
	if(totalhits)
	{
		memset(buffer, 0, sizeof(buffer));

		for(i = 0, j = 0; i < 6; i++)
		{
			if(client->hitstaken[i])
			{
				sprintf(buffer, "%s%4d [%s]", buffer, client->hitstaken[i],
									i == 0?"7mm":
									i == 1?"13mm":
									i == 2?"20mm":
									i == 3?"30-37mm":
									i == 4?"40-57mm":"75-88mm");

				if(++j >= 3)
				{
					PPrintf(client, RADIO_WHITE, "%s", buffer);
					memset(buffer, 0, sizeof(buffer));
					j = 0;
				}
			}
		}

		if(j)
			PPrintf(client, RADIO_WHITE, "%s", buffer);
	}
	PPrintf(client, RADIO_WHITE, "==================================================");

//	if ((i = ScoresCheckMedals(client)))
//	{
//		PPrintf(client, RADIO_GREEN, "You are awarded with %u medal%s!", i, i > 1 ? "s" : "");
//		PClientMedals(NULL, client);
//	}
}

/*************
 ScoresCheckKiller

 Check who killed client

 DEBUG: CHANGE IsFighter and IsBomber for a plane number function
 *************/

void ScoresCheckKiller(client_t *client)
{
	int8_t i, j;
	char buffer[128];
	char query_bomber[512];
	char query_ground[512];
	float damage;
	client_t *nearplane= NULL;

	if (client->infly) // if not infly, cant be killed
	{
		if(!client->damaged) // maneuver kill or nothing
		{
			Com_Printf(VERBOSE_DEBUG, "Check nearest player (3000 feets radius)\n");
			if ((client->posalt[0] - GetHeightAt(client->posxy[0][0], client->posxy[1][0])) <= 164) // explosions above 50mts are not maneuver kill
			{
				nearplane = NearPlane(client, client->country, 3000);
	
				if (nearplane)
				{
					Com_Printf(VERBOSE_DEBUG, "Found nearest player (%s)\n", nearplane->longnick);
					
					if (printkills->value)
					{
						BPrintf(RADIO_YELLOW, "Maneuver kill of %s(%s) by %s(%s)", client->longnick, GetSmallPlaneName(client->plane), nearplane->longnick, GetSmallPlaneName(nearplane->plane));
						
						if(nearplane->squadron)
							BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(nearplane->squadron));
					}
					else
					{
						Com_Printf(VERBOSE_CHAT, "Maneuver kill of %s(%s) by %s(%s)\n", client->longnick, GetSmallPlaneName(client->plane), nearplane->longnick, GetSmallPlaneName(nearplane->plane));
						
						if(nearplane->squadron)
							Com_Printf(VERBOSE_CHAT, "`-> from %s\n", Com_SquadronName(nearplane->squadron));
					}
					
					Com_Printf(VERBOSE_KILL, "%s Maneuver Killed by %s at %s\n", client->longnick, nearplane->longnick, Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
					
					if(IsFighter(nearplane) && IsFighter(client))
						CalcEloRating(nearplane /*winner*/, client /*looser*/, ELO_BOTH);
		
					if (IsFighter(nearplane))
						sprintf(my_query, "UPDATE score_fighter SET");
					else if (IsBomber(nearplane))
						sprintf(my_query, "UPDATE score_bomber SET");
					else if (IsGround(nearplane))
						sprintf(my_query, "UPDATE score_ground SET");
					else
					{
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", nearplane->plane);
						sprintf(my_query, "UPDATE score_fighter SET");
					}
					
					//score
					switch (client->plane)
					{
						case 61:
							strcat(my_query, " killcommandos = killcommandos + '1'");
		
							if (nearplane->country != client->country)
							{
								nearplane->score.groundscore += SCORE_COMMANDO;
							}
							else
							{
								nearplane->score.penaltyscore += 2 * SCORE_COMMANDO;
							}
							break;
						case PLANE_FAU:
							strcat(my_query, " killfau = killfau + '1'");
		
							if (nearplane->country != client->country)
							{
								nearplane->score.airscore += SCORE_FAU;
							}
							else
							{
								nearplane->score.penaltyscore += 2 * SCORE_FAU;
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
		
									if (nearplane->country != client->country)
									{
										nearplane->score.groundscore += SCORE_HMACK;
									}
									else
									{
										nearplane->score.penaltyscore += 2 * SCORE_HMACK;
									}
								}
								else if (client->drone & DRONE_KATY)
								{
									strcat(my_query, " killkaty = killkaty + '1'");
		
									if (nearplane->country != client->country)
									{
										nearplane->score.groundscore += SCORE_KATY;
									}
									else
									{
										nearplane->score.penaltyscore += 2 * SCORE_KATY;
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
		
								if (nearplane->country != client->country)
								{
									nearplane->score.groundscore += SCORE_TANK;
								}
								else
								{
									nearplane->score.penaltyscore += 2 * SCORE_TANK;
								}
								break;
							}
						default:
							if (client->infly)
							{
								if (wb3->value && IsGround(client))
								{
									strcat(my_query, " killtank = killtank + '1'");
		
									nearplane->killssortie++;
									nearplane->killstod++;
		
									if (nearplane->country != client->country)
									{
										strcat(my_query, ", curr_streak = curr_streak + '1', long_streak = IF(curr_streak > long_streak, curr_streak, long_streak)");
										nearplane->score.groundscore += SCORE_TANK;
									}
									else
									{
										nearplane->score.penaltyscore += 2 * SCORE_TANK;
									}
								}
								else
								{
									strcat(my_query, " kills = kills + '1'");
		
									nearplane->killssortie++;
									nearplane->killstod++;
		
									if (nearplane->country != client->country)
									{
										strcat(my_query, ", curr_streak = curr_streak + '1', long_streak = IF(curr_streak > long_streak, curr_streak, long_streak)");
										nearplane->score.airscore += SCORE_KILL;
									}
									else
									{
										nearplane->score.penaltyscore += 2 * SCORE_KILL;
									}
								}
							}
					}
		
					sprintf(my_query, "%s WHERE player_id = '%u'", my_query, nearplane->id);
		
					if (d_mysql_query(&my_sock, my_query))
					{
						PPrintf(client, RADIO_YELLOW, "ScoresCheckKiller(): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
						Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query UPDATE error %d: %s query %s\n", mysql_errno(&my_sock), mysql_error(&my_sock), my_query);
					}
					
					// EVENT LOGS
					
					Com_LogEvent(EVENT_KILL, nearplane->id, client->id);
					Com_LogDescription(EVENT_DESC_PLPLANE, nearplane->plane, NULL);
					if (IsFighter(nearplane))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
					else if (IsBomber(nearplane))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 2, NULL);
					else if (IsGround(nearplane))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 3, NULL);
					else
					{
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", nearplane->plane);
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
					}
		
					Com_LogDescription(EVENT_DESC_PLCTRY, nearplane->country, NULL);
					Com_LogDescription(EVENT_DESC_PLORD, nearplane->ord, NULL);
					Com_LogDescription(EVENT_DESC_VCPLANE, client->plane, NULL);
					if (IsFighter(client))
						Com_LogDescription(EVENT_DESC_VCPLTYPE, 1, NULL);
					else if (IsBomber(client))
						Com_LogDescription(EVENT_DESC_VCPLTYPE, 2, NULL);
					else if (IsGround(client))
						Com_LogDescription(EVENT_DESC_VCPLTYPE, 3, NULL);
					else
					{
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
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
			
					sprintf(my_query, "%s'', '%u', '%u', '%u', '%u',", my_query, nearplane->id, nearplane->plane,
						IsFighter(nearplane) ? 1 : IsBomber(nearplane) ? 2 : 3, nearplane->country);
			
					sprintf(my_query, "%s '%u', '%u', '%u', '%u')", my_query, client->id, client->plane, IsFighter(client) ? 1 : IsBomber(client) ? 2 : 3, client->country);
			
					if (d_mysql_query(&my_sock, my_query))
					{
						Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
		
					// ASSISTS
					
					j = 0;
			
					sprintf(my_query, "UPDATE score_fighter SET assists = assists + '1' WHERE player_id IN(");
					sprintf(query_bomber, "UPDATE score_bomber SET assists = assists + '1' WHERE player_id IN(");
					sprintf(query_ground, "UPDATE score_ground SET assists = assists + '1' WHERE player_id IN(");
			
					for (i = 0; i < MAX_HITBY; i++) // check who hit player
					{
						if (client->hitby[i] && client->damby[i] && !client->hitby[i]->drone && client->hitby[i]->inuse && client->hitby[i]->infly)
						{
							if ((client->hitby[i] != client) && (client->hitby[i] != nearplane)) // if not the maneuver killed nor ack damage (dont give piece do acks please, they don't deserves hehehe)
							{
								if (client->hitby[i]->country != client->country) // if enemy
								{
									if (printkills->value)
									{
										PPrintf(client->hitby[i], RADIO_YELLOW, "You've got a piece of %s", client->longnick);
									}

									Com_Printf(VERBOSE_KILL, "%s got a piece of %s\n", client->hitby[i]->longnick, client->longnick);

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
										Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
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
										Com_Printf(VERBOSE_KILL, "%s got a piece of his friend %s\n", client->hitby[i]->longnick, client->longnick);
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
								Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(assists): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
								Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(assists2): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
								Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(assists3): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							}
						}
					}
				}
			}
		}
		else // Damaged, valid kill
		{
			Com_Printf(VERBOSE_DEBUG, "Now check who did more damage\n");
			for (i = 0, j = -1, damage = 0; i < MAX_HITBY; i++) // check who inflicted more damage
			{
				if (client->hitby[i]) // if client in list
				{
					if (client->hitby[i]->inuse) // if still connected
					{
						Com_Printf(VERBOSE_DEBUG, "%s - %f\n", client->hitby[i]->longnick, client->damby[i]);
						if (client->damby[i] > damage) // if damage > current damage
						{
							damage = client->damby[i]; // set current damage as attacker damage
							client->damby[i] = 0; // clear damby value
							j = i; // set j as index of max damage
						}
					}
					else // else, clear from list
					{
						Com_Printf(VERBOSE_DEBUG, "Clearing offline player from array[%u]\n", i);
						client->hitby[i] = NULL;
						client->damby[i] = 0;
					}
				}
			}
	
			if(!(j < 0)) // found a killer
			{
				Com_Printf(VERBOSE_DEBUG, "Server has chosen %s as killer!!!\n", client->hitby[j]->longnick);
				
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
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->planeby[j]);
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
					Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
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
					Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
							; // TODO: FIXME: BAN CLIENT UNTIL END OF TOD
					}
		
					if (client->hitby[j]->squadron)
						sprintf(buffer, "%s from %s", buffer, Com_SquadronName(client->hitby[j]->squadron));
		
					if (printkills->value)
					{
						BPrintf(RADIO_GREEN, buffer);
					}
		
					Com_Printf(VERBOSE_KILL, "%s Teamkilled by %s at %s\n", client->longnick, client->hitby[j]->longnick, Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
		
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
								Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
								sprintf(my_query, "UPDATE score_fighter SET");
							}
						}
					}
				}
				else // enemy kill
				{
					if ((client->status_damage & STATUS_PILOT) && client->chute)
						sprintf(buffer, "Chutekill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j] == client ? "Ack Weenies" : client->hitby[j]->longnick);
					else if (client->cancollide < 0)
						sprintf(buffer, "Collision kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j] == client ? "Ack Weenies" : client->hitby[j]->longnick);
					else
						sprintf(buffer, "Kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j] == client ? "Ack Weenies" : client->hitby[j]->longnick);
		
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
								Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
								sprintf(my_query, "UPDATE score_fighter SET");
							}
						}
					}
		
					if (printkills->value)
					{
						BPrintf(RADIO_YELLOW, buffer);
					}
		
					Com_Printf(VERBOSE_KILL, "%s Killed by %s at %s\n", client->longnick, client->hitby[j]->longnick, Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
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
						PPrintf(client, RADIO_YELLOW, "ScoresCheckKiller(): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
						Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query UPDATE error %d: %s query %s\n", mysql_errno(&my_sock), mysql_error(&my_sock), my_query);
					}
				}
		
				if ((client->hitby[j] != client) && (client->hitby[j]->country != client->country))
				{
					if (client->hitby[j]->squadron)
					{
						if (printkills->value)
							BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(client->hitby[j]->squadron));
						else
							Com_Printf(VERBOSE_CHAT, "`-> from %s\n", Com_SquadronName(client->hitby[j]->squadron));
					}
				}
		
				client->hitby[j] = NULL; // clear killer from list
		
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
		
								Com_Printf(VERBOSE_KILL, "%s got a piece of %s\n", client->hitby[i]->longnick, client->longnick);
		
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
									Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
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
									Com_Printf(VERBOSE_KILL, "%s got a piece of his friend %s\n", client->hitby[i]->longnick, client->longnick);
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
							Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(assists): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
							Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(assists2): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
							Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(assists3): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}
				}
			}
		}
	}

	ClearKillers(client);
}

/*************
 ScoresCheckMedals

 Check if client receive a new medal
 *************/

u_int8_t ScoresCheckMedals(client_t *client)
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
							new += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_GRANDSTAR, value, client);
						}
						else
							new += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_GRANDCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_ORDERFALCON, value, client);
				}
				else
					new += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_SILVERSTAR, value, client);
			}
			else
				new += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_COMBATCROSS, value, client);
		}
		else
			new += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_COMBATMEDAL, value, client);
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
							new += ScoresAddMedal(DEED_KILLSTOD, MEDAL_GRANDSTAR, value, client);
						}
						else
							new += ScoresAddMedal(DEED_KILLSTOD, MEDAL_GRANDCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_KILLSTOD, MEDAL_SILVERSTAR, value, client);
				}
				else
					new += ScoresAddMedal(DEED_KILLSTOD, MEDAL_ORDERFALCON, value, client);
			}
			else
				new += ScoresAddMedal(DEED_KILLSTOD, MEDAL_COMBATCROSS, value, client);
		}
		else
			new += ScoresAddMedal(DEED_KILLSTOD, MEDAL_COMBATMEDAL, value, client);
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
							new += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_GRANDSTAR, value, client);
						}
						else
							new += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_GRANDCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_SILVERSTAR, value, client);
				}
				else
					new += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_ORDERFALCON, value, client);
			}
			else
				new += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_COMBATCROSS, value, client);
		}
		else
			new += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_COMBATMEDAL, value, client);
	}

	// streak kills
	sprintf(
			my_query,
			"SELECT t1.curr_streak as fighter_streak, t2.curr_streak as bomber_streak, (t1.kills + t2.kills + t3.kills + t1.killfau) as kills, (t1.acks + t1.buildings + t1.ships + t1.cvs + t2.acks + t2.buildings + t2.ships + t2.cvs + t3.acks + t3.buildings + t3.ships + t3.cvs) as buildings, (t2.fieldscapt + t3.fieldscapt) as fieldscapt FROM score_fighter as t1, score_bomber as t2, score_ground as t3 WHERE t1.player_id = '%u' AND t1.player_id = t2.player_id AND t2.player_id = t3.player_id",
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
										new += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_ORDERFALCON, value, client);
							}
							else
								new += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_SILVERSTAR, value, client);
						}
						else
							new += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_COMBATMEDAL, value, client);
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
										new += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_ORDERFALCON, value, client);
							}
							else
								new += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_SILVERSTAR, value, client);
						}
						else
							new += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_COMBATMEDAL, value, client);
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
										new += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_SILVERSTAR, value, client);
							}
							else
								new += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_ORDERFALCON, value, client);
						}
						else
							new += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_COMBATMEDAL, value, client);
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
										new += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_SILVERSTAR, value, client);
							}
							else
								new += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_ORDERFALCON, value, client);
						}
						else
							new += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_COMBATMEDAL, value, client);
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
										new += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_GRANDSTAR, value, client);
									}
									else
										new += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_GRANDCROSS, value, client);
								}
								else
									new += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_SILVERSTAR, value, client);
							}
							else
								new += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_ORDERFALCON, value, client);
						}
						else
							new += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_COMBATCROSS, value, client);
					}
					else
						new += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_COMBATMEDAL, value, client);
				}

			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "ScoresCheckMedals(): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "ScoresCheckMedals(): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "UNNAMED: couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
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
					Com_Printf(VERBOSE_WARNING, "ScoresCheckMedals(rank): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "ScoresCheckMedals(rank): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "ScoresCheckMedals(rank): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "ScoresCheckMedals(rank): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	return new;
}

/*************
 new = ScoresAddMedal

 Add a new medal to client if needed
 *************/

u_int8_t ScoresAddMedal(u_int8_t deed, u_int8_t medal, u_int16_t value, client_t *client)
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
			Com_Printf(VERBOSE_WARNING, "ScoresAddMedal(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}

		return 0;
	}

	return 1;
}

/*************
 ScoresCheckCaptured

 Check if clients has been captured on land/ditch/bail
 *************/

void ScoresCheckCaptured(client_t *client)
{
	int16_t i, k;
	//	int32_t x, y;
	u_int32_t dist, nearplane;

	k = NearestField(client->posxy[0][0], client->posxy[1][0], client->country, 
	TRUE, TRUE, &nearplane);

	if (k >= 0)
	{
		if (k < fields->value)
		{
			i = arena->fields[k].country;
		}
		else
		{
			i = arena->cities[k - (int16_t)fields->value].country;
		}

		dist = i == 1 ? radarrange1->value : i == 2 ? radarrange2->value : i == 3 ? radarrange3->value : i == 4 ? radarrange4->value : 0;

		if (dist)
			dist /= 22;

		if (nearplane < dist)
		{
			Com_LogEvent(EVENT_CAPTURED, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
			Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
			//score
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
				Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
				sprintf(my_query, "UPDATE score_fighter SET");
			}

			sprintf(my_query, "%s captured = captured + '1' WHERE player_id = '%u'", my_query, client->id);

			if (d_mysql_query(&my_sock, my_query))
			{
				Com_Printf(VERBOSE_WARNING, "ScoresCheckCaptured(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			client->score.airscore /= 2;
			client->score.groundscore /= 2;
			client->score.captscore /= 2;
			client->score.rescuescore /= 2;

			PPrintf(client, RADIO_YELLOW, "You have been captured by %s", GetCountry(i));
		}
	}
}

/*************
 ScoresCreate

 Create Score rows
 *************/

void ScoresCreate(client_t *client)
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
							Com_Printf(VERBOSE_WARNING, "ScoresCreate(penalty): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}
				}
				else
				{
					if (mysql_errno(&my_sock) != 1062)
					{
						Com_Printf(VERBOSE_WARNING, "ScoresCreate(ground): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
			}
			else
			{
				if (mysql_errno(&my_sock) != 1062)
				{
					Com_Printf(VERBOSE_WARNING, "ScoresCreate(ground): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
		}
		else
		{
			if (mysql_errno(&my_sock) != 1062)
			{
				Com_Printf(VERBOSE_WARNING, "ScoresCreate(bomber): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
	}
	else
	{
		if (mysql_errno(&my_sock) != 1062)
		{
			Com_Printf(VERBOSE_WARNING, "ScoresCreate(fighter): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
}