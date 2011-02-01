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
 ScoresEvent

 Calculate the score based on events
 */

void ScoresEvent(u_int16_t event, client_t *client, int32_t misc)
{
	int8_t killer;
	//u_int8_t i;
	u_int8_t captured = 0;
	u_int8_t collided = 0;
	int8_t penalty = 1;
	u_int32_t flighttime;
	munition_t *munition;
	double event_cost = 0.0;

	if(!client)
	{
		return;
	}

	if(misc < 0)
	{
		misc *= -1;
		penalty = -1;
	}

	if(event & SCORE_CAPTURED)
	{
		event ^= SCORE_CAPTURED;
		captured = 1;
	}

	if(event & SCORE_COLLIDED)
	{
		event ^= SCORE_COLLIDED;
		collided = 1;
	}

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

	Com_Printf(VERBOSE_DEBUG_SCORES, "EVENT: %d. Misc: %d\n", event, misc);

	switch (event)
	{
		case SCORE_TAKEOFF:
				 client->score.costscore = client->lastscore = client->score.airscore = client->score.groundscore = client->score.captscore = client->score.rescuescore = client->killssortie
						= client->hits[0] = client->hits[1] = client->hits[2] = client->hits[3] = client->hits[4] = client->hits[5] = client->hitstaken[0] = client->hitstaken[1] = client->hitstaken[2]
						= client->hitstaken[3] = client->hitstaken[4] = client->hitstaken[5] = client->score.penaltyscore = client->damaged = 0;
				event_cost += arena->costs.takeoff;
				strcat(my_query, " sorties = sorties + '1'");
			break;
		case SCORE_DROPITEM:
			client->score.costscore += GetAmmoCost(misc);

			munition = GetMunition(misc);

			if(munition)
			{
				switch (munition->type)
				{
					case MUNTYPE_ROCKET:
						strcat(my_query, " rocketused = rocketused + '1'");
						break;
					case MUNTYPE_BOMB:
						strcat(my_query, " bombused = bombused + '1'");
						break;
					case MUNTYPE_TORPEDO:
						strcat(my_query, " torpused = torpused + '1'");
						break;
					default:
						strcat(my_query, " gunused = gunused + '1'");
				}
			}
			break;
		case SCORE_HARDHIT:
			munition = GetMunition(misc);

			if(munition)
			{
				switch (munition->type)
				{
					case MUNTYPE_ROCKET:
						strcat(my_query, " rockethits = rockethits + '1'");
						break;
					case MUNTYPE_BOMB:
						strcat(my_query, " bombhits = bombhits + '1'");
						break;
					case MUNTYPE_TORPEDO:
						strcat(my_query, " torphits = torphits + '1'");
						break;
					default:
						strcat(my_query, " gunhits = gunhits + '1'");
				}
			}
			break;
		case SCORE_STRUCTDAMAGE:
			if(penalty > 0)
			{
				client->score.groundscore += (double)misc / 100;
			}
			else
			{
				client->score.penaltyscore += (double)misc / 100;
			}
			return; // dont do queries
			break;
		case SCORE_STRUCTURE:
			if (misc == BUILD_CV)
			{
				strcat(my_query, " cvs = cvs + '1'");
			}
			else if (misc >= BUILD_50CALACK && misc <= BUILD_88MMFLAK)
			{
				strcat(my_query, " acks = acks + '1'");
			}
			else if (misc >= BUILD_DESTROYER && misc <= BUILD_CARGO)
			{
				strcat(my_query, " ships = ships + '1'");
			}
			else
			{
				if ((misc != BUILD_TREE) && (misc != BUILD_ROCK) && (misc != BUILD_FENCE))
				{
					strcat(my_query, " buildings = buildings + '1'");
				}
				else
				{
					my_query[0] = '\0';
					return; // return if hit tree or rock or fence
				}
			}
			break;
		case SCORE_FIELDCAPT:
				ScoreFieldCapture(misc);
				strcat(my_query, " fieldscapt = fieldscapt + '1'");
			break;
		case SCORE_LANDED: //////// HERE BEGINS EVENTS THAT MAY HAVE KILLERS /////////
				strcat(my_query, " landed = landed + '1'");

				if(captured)
				{
					event_cost += ScorePlaneCost(client) + arena->costs.newpilot + arena->costs.informationlost + ScoreTechnologyCost(client);
					strcat(my_query, ", captured = captured + '1'");
				}
				else
				{
					if(FLIGHT_TIME(client) > 2000) // 20 secs
						event_cost += ScoreDamageCost(client);
				}
			break;
		case SCORE_DITCHED:
				strcat(my_query, " ditched = ditched + '1'");

				if(captured)
				{
					event_cost += ScorePlaneCost(client) + arena->costs.newpilot + arena->costs.informationlost + ScoreTechnologyCost(client);
					strcat(my_query, ", captured = captured + '1'");
				}
				else
				{
					event_cost += ScoreDamageCost(client) + ScorePilotTransportCost(client);
				}
			break;
		case SCORE_DISCO:
				strcat(my_query, " disco = disco + '1'");

				if(ScorePlaneLife(client) < 0.5)
				{ // kill
					misc = SCORE_KILLED;

					event_cost += ScorePlaneCost(client) + arena->costs.newpilot + arena->costs.life;

					strcat(my_query, ", killed = killed + '1', curr_streak = '0'");

					client->streakscore = 0;

					if (client->lives > 0)
					{
						client->lives--;
						Cmd_Lives(client->longnick, client->lives, NULL);
					}
					break;
				}
				else
				{
					misc = SCORE_BAILED;
					strcat(my_query, ","); // let it go to SCORE_BAILED
				}
//				else // bail
//				{
//					event_cost += ScorePlaneCost(client) + ScorePilotTransportCost(client);
//				}
		case SCORE_BAILED:
				if (IsFighter(client) || IsBomber(client))
				{
					strcat(my_query, " bailed = bailed + '1'");
				}
				else
				{
					strcat(my_query, " landed = landed + '1'");
				}

				if(captured)
				{
					event_cost += ScorePlaneCost(client) + arena->costs.newpilot + arena->costs.informationlost + ScoreTechnologyCost(client);
					strcat(my_query, ", captured = captured + '1'");
				}
				else
				{
					event_cost += ScorePlaneCost(client) + ScorePilotTransportCost(client);
				}
			break;
		case SCORE_KILLED:
				event_cost += ScorePlaneCost(client) + arena->costs.newpilot + arena->costs.life;

				strcat(my_query, " killed = killed + '1', curr_streak = '0'");

				client->streakscore = 0;

				if (client->lives > 0)
				{
					client->lives--;
					Cmd_Lives(client->longnick, client->lives, NULL);
				}
			break;
	}

	Com_Printf(VERBOSE_DEBUG_SCORES, "EVENT COST: %f\n", event_cost);

	if(event_cost < 0)
	{
		Com_Printf(VERBOSE_DEBUG_SCORES, "ScoresEvent(event_cost) < 0, event = %d, misc = %d\n", event, misc);
	}

	client->score.costscore += event_cost;

	if(event & (SCORE_BAILED | SCORE_CAPTURED | SCORE_DISCO | SCORE_DITCHED | SCORE_KILLED | SCORE_LANDED))
	{
		client->lastscore += client->score.airscore + client->score.groundscore + client->score.captscore + client->score.rescuescore - client->score.penaltyscore - client->score.costscore;

		if(!client->lastscore)
			Com_Printf(VERBOSE_DEBUG_SCORES, "lastscore == 0 for %s (airscore = %.3f, groundscore = %.3f, captscore = %.3f, rescuescore = %.3f, penaltyscore = %.3f)\n",
					client->longnick,
					client->score.airscore,
					client->score.groundscore,
					client->score.captscore,
					client->score.rescuescore,
					client->score.penaltyscore);

		if(event & SCORE_DISCO)
			sprintf(my_query, "%s, disco_score = disco_score + '%.3f'", my_query, client->lastscore);

		if (IsFighter(client))
		{
			if (client->score.airscore)
				sprintf(my_query, "%s, fighter_score = fighter_score + '%.3f'", my_query, client->score.airscore);
			if (client->score.captscore)
				sprintf(my_query, "%s, capt_score = capt_score + '%.3f'", my_query, client->score.captscore);
			if (client->score.groundscore)
				sprintf(my_query, "%s, jabo_score = jabo_score + '%.3f'", my_query, client->score.groundscore);
			if (client->score.costscore)
				sprintf(my_query, "%s, cost_score = cost_score + '%.3f'", my_query, client->score.costscore);
		}
		else if (IsBomber(client))
		{
			if (client->score.airscore + client->score.groundscore)
				sprintf(my_query, "%s, bomber_score = bomber_score + '%.3f'", my_query, client->score.airscore + client->score.groundscore);
			if (client->score.captscore)
				sprintf(my_query, "%s, capt_score = capt_score + '%.3f'", my_query, client->score.captscore);
			if (client->score.rescuescore)
				sprintf(my_query, "%s, resc_score = resc_score + '%.3f'", my_query, client->score.rescuescore);
			if (client->score.costscore)
				sprintf(my_query, "%s, cost_score = cost_score + '%.3f'", my_query, client->score.costscore);
		}
		else if (IsGround(client))
		{
			if (client->score.airscore + client->score.groundscore)
				sprintf(my_query, "%s, ground_score = ground_score + '%.3f'", my_query, client->score.airscore + client->score.groundscore);
			if (client->score.captscore)
				sprintf(my_query, "%s, capt_score = capt_score + '%.3f'", my_query, client->score.captscore);
			if (client->score.rescuescore)
				sprintf(my_query, "%s, resc_score = resc_score + '%.3f'", my_query, client->score.rescuescore);
			if (client->score.costscore)
				sprintf(my_query, "%s, cost_score = cost_score + '%.3f'", my_query, client->score.costscore);
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
			if (client->score.airscore)
				sprintf(my_query, "%s, fighter_score = fighter_score + '%.3f'", my_query, client->score.airscore);
			if (client->score.captscore)
				sprintf(my_query, "%s, capt_score = capt_score + '%.3f'", my_query, client->score.captscore);
			if (client->score.groundscore)
				sprintf(my_query, "%s, jabo_score = jabo_score + '%.3f'", my_query, client->score.groundscore);
			if (client->score.costscore)
				sprintf(my_query, "%s, cost_score = cost_score + '%.3f'", my_query, client->score.costscore);
		}
	}

	sprintf(my_query, "%s WHERE player_id = '%u'", my_query, client->id);

	if(!client->drone)
	{
		Com_Printf(VERBOSE_DEBUG_SCORES, "Event Query: %s\n", my_query);

		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "ScoresEvent(update): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			Com_Printf(VERBOSE_WARNING, "Query: %s\n", my_query);
		}
	}

	if(event & (SCORE_BAILED | SCORE_CAPTURED | SCORE_DISCO | SCORE_DITCHED | SCORE_KILLED | SCORE_LANDED))
	{
		if(misc != SCORE_KILLED)
			client->streakscore += client->lastscore;

		flighttime = FLIGHT_TIME(client)/1000;

		sprintf(my_query, "UPDATE score_common SET killstod = '%u', structstod = '%u', lastscore = '%.3f', totalscore = totalscore + '%.3f', streakscore = '%.3f', flighttime = flighttime + '%u', flighttime_month = flighttime_month + '%u' WHERE player_id = '%u'",
				client->killstod, client->structstod, client->lastscore, client->lastscore, client->streakscore, flighttime, flighttime, client->id);

		if(!client->drone)
		{
			Com_Printf(VERBOSE_DEBUG_SCORES, "Query: %s\n", my_query);
			if (d_mysql_query(&my_sock, my_query)) // query succeeded
			{
				Com_Printf(VERBOSE_WARNING, "ScoresEvent(updplayer): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}
		}
	}

	if(event & SCORE_TAKEOFF)
	{
		sprintf(my_query, "UPDATE score_common SET");

		if (client->country == 1)
			sprintf(my_query, "%s flyred = flyred + '1' WHERE player_id = '%u'", my_query, client->id);
		else if (client->country == 3)
			sprintf(my_query, "%s flygold = flygold + '1' WHERE player_id = '%u'", my_query, client->id);

		Com_Printf(VERBOSE_DEBUG_SCORES, "Query: %s\n", my_query);
		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "ScoresEvent(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
		return;
	}

	if(event & (SCORE_DROPITEM | SCORE_HARDHIT | SCORE_STRUCTURE | SCORE_FIELDCAPT))
		return; 

	if (client->score.penaltyscore)
	{
		sprintf(my_query, "UPDATE score_penalty SET penalty_score = penalty_score + '%.3f' WHERE player_id = '%u'", client->score.penaltyscore, client->id);

		Com_Printf(VERBOSE_DEBUG_SCORES, "Query: %s\n", my_query);
		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "ScoresEvent(penalty): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}

	if(event & SCORE_LANDED)
	{
		ScorePieceDamage(-1, event_cost, client);
		ClearKillers(client);
		return;
	}

//////////////////// KILLER SCORE ///////////////////

	killer = ScoresCheckKiller(client, &misc);

	if(misc) // maneuver kill
	{
		ScorePieceDamage(killer, (event_cost/2), client);
	}
	else
	{
		Com_Printf(VERBOSE_DEBUG_SCORES, "Piece event cost %f, killed %s\n", event_cost, client->longnick);
		ScorePieceDamage(-1, event_cost, client);
	}

	ClearKillers(client);
}

/**
 ScoreFieldCapture

 Give score to all capture bombers
 */

void ScoreFieldCapture(u_int8_t field)
{
	char sql_query[1024];
	double fieldcost, score;
	client_t *dbclient;
	u_int8_t numbombers, numcargos, i;
	u_int32_t bomberdamage, cargodamage;

	if(field < fields->value)
	{
		Com_Printf(VERBOSE_DEBUG_SCORES, "Field %d\n", field+1);
		Com_Printf(VERBOSE_DEBUG_SCORES, "Country %d\n", arena->fields[field].country);
		Com_Printf(VERBOSE_DEBUG_SCORES, "Type %d\n", arena->fields[field].type);

		fieldcost = GetFieldCost(field);

		bomberdamage = cargodamage = numbombers = numcargos = 0;

		Com_Printf(VERBOSE_DEBUG_SCORES, "Start search capturers\n");

		for(i = 0; i < MAX_HITBY; i++) // account bombers and cargos
		{
			if(arena->fields[field].hitby[i].damage)
			{
				if(IsCargo(NULL, arena->fields[field].hitby[i].plane))
				{
					Com_Printf(VERBOSE_DEBUG_SCORES, "Found Cargo damage %.2f\n", arena->fields[field].hitby[i].damage);
					cargodamage += arena->fields[field].hitby[i].damage;
					numcargos++;
				}
				else
				{
					Com_Printf(VERBOSE_DEBUG_SCORES, "Found Bomber damage %.2f\n", arena->fields[field].hitby[i].damage);
					bomberdamage += arena->fields[field].hitby[i].damage;
					numbombers++;
				}
			}
		}

		if (mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_ON))
		Com_Printf(VERBOSE_ERROR, "%d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

		sql_query[0] = '\0';

		Com_Printf(VERBOSE_DEBUG_SCORES, "Start giving points\n");

		for(i = 0; i < MAX_HITBY; i++) // give points
		{
			if(arena->fields[field].hitby[i].dbid && arena->fields[field].hitby[i].dbid < DRONE_DBID_BASE)
			{
				if(IsCargo(NULL, arena->fields[field].hitby[i].plane))
				{
					score = fieldcost * (arena->fields[field].hitby[i].damage / cargodamage) * ((double)numcargos / (numcargos + numbombers));
					Com_Printf(VERBOSE_DEBUG_SCORES, "Cargo: %f\n", score);
				}
				else
				{
					score = fieldcost * (arena->fields[field].hitby[i].damage / bomberdamage) * ((double)numbombers / (numcargos + numbombers));
					Com_Printf(VERBOSE_DEBUG_SCORES, "Bomber: %f\n", score);
				}

				if (arena->fields[field].hitby[i].country == arena->fields[field].country) // if it WAS enemy
				{
					if((dbclient = FindDBClient(arena->fields[field].hitby[i].dbid)) && dbclient->inflight)
					{
						Com_Printf(VERBOSE_DEBUG_SCORES, "InFlight\n");
						dbclient->score.captscore += score;
					}
					else
					{
						Com_Printf(VERBOSE_DEBUG_SCORES, "Not InFlight\n");
						if (IsFighter(NULL, arena->fields[field].hitby[i].plane))
						{
							sprintf(sql_query, "%sUPDATE score_fighter SET, capt_score = capt_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, arena->fields[field].hitby[i].dbid);
						}
						else if (IsBomber(NULL, arena->fields[field].hitby[i].plane))
						{
							sprintf(sql_query, "%sUPDATE score_bomber SET capt_score = capt_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, arena->fields[field].hitby[i].dbid);
						}
						else if (IsGround(NULL, arena->fields[field].hitby[i].plane))
						{
							sprintf(sql_query, "%sUPDATE score_ground SET capt_score = capt_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, arena->fields[field].hitby[i].dbid);
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", arena->fields[field].hitby[i].plane);
							sprintf(sql_query, "%sUPDATE score_fighter SET, capt_score = capt_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, arena->fields[field].hitby[i].dbid);
						}
					}
				}
				else if(friendlyfire->value)// friendly hit... tsc, tsc, tsc...
				{
					if((dbclient = FindDBClient(arena->fields[field].hitby[i].dbid)) && dbclient->inflight)
					{
						Com_Printf(VERBOSE_DEBUG_SCORES, "Field Penalty InFlight\n");
						dbclient->score.penaltyscore += score;
					}
					else
					{
						Com_Printf(VERBOSE_DEBUG_SCORES, "Field Penalty Not InFlight\n");
						sprintf(sql_query, "%sUPDATE score_penalty SET penalty_score = penalty_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, arena->fields[field].hitby[i].dbid);
					}
				}
				else
				{
					Com_Printf(VERBOSE_DEBUG_SCORES, "Field Penalty filtered '%.3f'\n", score);
				}
			}
		}

		ClearBombers(field);

		if(sql_query[0] != '\0')
		{
			if (d_mysql_query(&my_sock, sql_query))
			{
				Com_Printf(VERBOSE_WARNING, "ScoreFieldCapture(): couldn't query UPDATE id %d error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
				// TODO: FIXME: add sleep here?
				mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
				return;
			}
			else
			{
				Com_MySQL_Flush(&my_sock,__FILE__ , __LINE__);
			}
		}

#ifdef  _WIN32
		sleep(140);
#else
		usleep(140000);
#endif
		mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
	}

////////////////***** ////

//	for (i = 0; i < MAX_HITBY; i++) // check who hit player
//	{
//		if (client->hitby[i] && client->hitby[i].plane && !client->hitby[i]->drone && client->hitby[i]->inuse)
//		{
//			if ((client->hitby[i] != client)) // if not ack damage (dont give piece do acks please, they don't deserves hehehe)
//			{
//				score = (client->hitby[i].damage/totaldamage) * event_cost;
//
//				if(killer == i)
//					score += event_cost;
//
//				if (client->hitby[i]->country != client->country) // if enemy
//				{
//					if (IsFighter(NULL, client->hitby[i].plane))
//					{
//						sprintf(sql_query, "%sUPDATE score_fighter SET fighter_score = fighter_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, client->hitby[i]->id);
//					}
//					else if (IsBomber(NULL, client->hitby[i].plane))
//					{
//						sprintf(sql_query, "%sUPDATE score_bomber SET bomber_score = bomber_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, client->hitby[i]->id);
//					}
//					else if (IsGround(NULL, client->hitby[i].plane))
//					{
//						sprintf(sql_query, "%sUPDATE score_ground SET ground_score = ground_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, client->hitby[i]->id);
//					}
//					else
//					{
//						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
//						sprintf(sql_query, "%sUPDATE score_fighter SET fighter_score = fighter_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, client->hitby[i]->id);
//					}
//				}
//				else // friendly hit... tsc, tsc, tsc...
//				{
//					sprintf(sql_query, "%sUPDATE score_penalty SET penalty_score = penalty_score + '%.3f' WHERE player_id = '%u'; ", sql_query, score, client->hitby[i]->id);
//				}
//			}
//		}
//	}

}

/**
 ScorePieceDamage

 Give piece points to damagers or just return the totaldamage
 */

double ScorePieceDamage(int8_t killer, double event_cost, client_t *client)
{
	int8_t i;
	double totaldamage = 0.0;
	double score;
	client_t *dbclient;

	// calc pieces // enemy damage/totalDamage * planeLife/totalLife * cost

	for (i = 0; i < MAX_HITBY; i++) // check who hit player
	{
		if (client->hitby[i].dbid && client->hitby[i].damage && client->hitby[i].dbid < DRONE_DBID_BASE)
		{
			totaldamage += client->hitby[i].damage;
		}
	}

	Com_Printf(VERBOSE_DEBUG_SCORES, "Total damage %f\n", totaldamage);

	if(event_cost)
	{
		my_query[0] = '\0';

		if (mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_ON))
			Com_Printf(VERBOSE_ERROR, "%d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

		for (i = 0; i < MAX_HITBY; i++) // check who hit player
		{
			if (client->hitby[i].dbid && client->hitby[i].damage && client->hitby[i].dbid < DRONE_DBID_BASE)
			{
				if ((client->hitby[i].dbid != client->id)) // if not ack damage (dont give piece do acks please, they don't deserves hehehe)
				{
					score = (client->hitby[i].damage/totaldamage) * event_cost;

					Com_Printf(VERBOSE_DEBUG_SCORES, "Score %f, killer %s\n", score, client->hitby[i].longnick);

					if(killer == i)
						score += event_cost;

					if (client->hitby[i].country != client->country) // if enemy
					{
						if((dbclient = FindDBClient(client->hitby[i].dbid)) && dbclient->inflight)
						{
							dbclient->score.airscore += score;
						}
						else
						{
							if (IsFighter(NULL, client->hitby[i].plane))
							{
								sprintf(my_query, "%sUPDATE score_fighter SET fighter_score = fighter_score + '%.3f' WHERE player_id = '%u'; ", my_query, score, client->hitby[i].dbid);
							}
							else if (IsBomber(NULL, client->hitby[i].plane))
							{
								sprintf(my_query, "%sUPDATE score_bomber SET bomber_score = bomber_score + '%.3f' WHERE player_id = '%u'; ", my_query, score, client->hitby[i].dbid);
							}
							else if (IsGround(NULL, client->hitby[i].plane))
							{
								sprintf(my_query, "%sUPDATE score_ground SET ground_score = ground_score + '%.3f' WHERE player_id = '%u'; ", my_query, score, client->hitby[i].dbid);
							}
							else
							{
								Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
								sprintf(my_query, "%sUPDATE score_fighter SET fighter_score = fighter_score + '%.3f' WHERE player_id = '%u'; ", my_query, score, client->hitby[i].dbid);
							}
						}
					}
					else if (client->hitby[i].dbid != client->id)// friendly hit... tsc, tsc, tsc...
					{
						if(friendlyfire->value)
						{
							if((dbclient = FindDBClient(client->hitby[i].dbid)) && dbclient->inflight)
							{
								Com_Printf(VERBOSE_DEBUG_SCORES, "Kill Penalty InFlight\n");
								dbclient->score.penaltyscore += score;
							}
							else
							{
								Com_Printf(VERBOSE_DEBUG_SCORES, "Kill Penalty Not InFlight\n");
								sprintf(my_query, "%sUPDATE score_penalty SET penalty_score = penalty_score + '%.3f' WHERE player_id = '%u'; ", my_query, score, client->hitby[i].dbid);
							}
						}
						else
						{
							Com_Printf(VERBOSE_DEBUG_SCORES, "Kill Penalty filtered '%.3f'\n", score);
						}
					}
				}
			}
		}

		if(my_query[0] != '\0')
		{
			if (d_mysql_query(&my_sock, my_query))
			{
				Com_Printf(VERBOSE_WARNING, "ScorePieceDamage(): couldn't query UPDATE id %d error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
				// TODO: FIXME: add sleep here?
				mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
				return 0.0;
			}
			else
			{
				Com_MySQL_Flush(&my_sock,__FILE__ , __LINE__);
			}
		}

#ifdef  _WIN32
		sleep(140);
#else
		usleep(140000);
#endif
		mysql_set_server_option(&my_sock, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
	}

	return totaldamage;
}

/**
 ScorePlaneCost

 Returns the cost of a new plane
 */

double ScorePlaneCost(client_t *client)
{
	if(!client)
	{
		return 0.0;
	}

	if(!(client->plane < maxplanes))
	{
		Com_Printf(VERBOSE_WARNING, "ScoreDamageCost() %s invalid plane %d\n", client->longnick, client->plane);
		return 0.0;
	}

	return arena->costs.planemodel[client->plane];
}

/**
 ScoreFixPlaneCost

 Returns the cost to fix a damaged plane
 */

double ScoreFixPlaneCost(double plane_life, double plane_cost)
{
	return plane_cost * (0.01 + (1.0 - plane_life) * 0.99);
}

/**
 ScorePlaneTransportCost

 Returns the cost to transport a plane to nearest airfield
 */

double ScorePlaneTransportCost(client_t *client)
{
	u_int32_t distance;
	int16_t field;

	if(client->plane < maxplanes)
	{
		field = NearestField(client->posxy[0][0], client->posxy[1][0], (client->country == 1) ? 3 : 1, TRUE, FALSE, &distance);

		if(field >= 0)
		{
			if(distance > 3280) // 1 km
				return ((double)(distance - 3280)/3280) * arena->costs.planeweight[client->plane] * arena->costs.planetransport;
			else
				return 0.0;
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "ScorePlaneTransportCost() %s invalid plane %d\n", client->longnick, client->plane);
		return 0.0;
	}

	return 0.0;
}

/**
 ScorePilotTransportCost

 Returns the cost to transport a pilot to nearest airfield
 */

double ScorePilotTransportCost(client_t *client)
{
	int16_t field;
	u_int32_t distance;

	field = NearestField(client->posxy[0][0], client->posxy[1][0], (client->country == 1) ? 3 : 1, FALSE, FALSE, &distance);

	if(field >= 0)
	{
		if(distance > 32808) // 10 km
			return ((double)(distance - 32808)/3280) * arena->costs.pilottransport;
		else
			return 0.0;
	}

	Com_Printf(VERBOSE_WARNING, "ScorePilotTransportCost(field) < 0\n");
	return 0.0;
}

/**
 ScoreFlightTimeCost

 Returns the cost of flight based in flight time
 */

double ScoreFlightTimeCost(client_t *client)
{
	double flighttime;

	if(!client)
	{
		return 0.0;
	}

	flighttime = FLIGHT_TIME(client)/1000;

	flighttime /= 3600; // convert to hours

	return flighttime * arena->costs.flighthour;
}

/**
 ScoreDamageCost

 Check the cost of damaged plane. If damage > 50%, cost == total plane cost, else cost == plane fix + plane transport
 */

double ScoreDamageCost(client_t *client)
{
	//u_int8_t i;
	double plane_cost, plane_recover, plane_life;

	if(!client)
	{
		return 0.0;
	}

	if(!((client->plane - 1) < maxplanes))
	{
		Com_Printf(VERBOSE_WARNING, "ScoreDamageCost() %s invalid plane %d\n", client->longnick, client->plane);
		return 0.0;
	}

	plane_life = ScorePlaneLife(client);
	plane_cost = ScorePlaneCost(client);
	plane_recover = plane_life < 0.5 ? plane_cost : (ScoreFixPlaneCost(plane_life, plane_cost) + ScorePlaneTransportCost(client));

	return (plane_cost < plane_recover) ? plane_cost : plane_recover;
}

/**
 ScorePlaneLife

 Returns the percentage of life from plane (including flight time usage)
 */

double ScorePlaneLife(client_t *client)
{
	u_int8_t i;
	double totalpoints = 0.0;
	double pointsleft = 0.0;

	for(pointsleft = totalpoints = i = 0; i < 32; i++)
	{
		if(arena->planedamage[client->plane].points[i] > 0)
		{
			totalpoints += arena->planedamage[client->plane].points[i];

			if(!(client->status_damage & (1 << i)))
			{
				if(client->armor.points[i] > 0)
					pointsleft += client->armor.points[i];
			}
		}
	}

//	pointsleft = totalpoints - pointsleft;
	Com_Printf(VERBOSE_DEBUG_SCORES, "ScorePlaneLife() pointsleft = %.3f, TimeCost = %.3f, totalpoints = %.3f\n", pointsleft, ScoreFlightTimeCost(client), totalpoints);

	if((pointsleft -= ScoreFlightTimeCost(client)) < 0.0)
	{
		Com_Printf(VERBOSE_DEBUG_SCORES, "ScorePlaneLife(pointsleft) < 0, %f\n", pointsleft);
		return 0;
	}

	pointsleft /= totalpoints;

	if(pointsleft > 1.0)
	{
		Com_Printf(VERBOSE_DEBUG_SCORES, "ScorePlaneLife(pointsleft) > 100%, %f\n", pointsleft);
		return 1;
	}
	else
		return pointsleft;
}

/**
 ScoresEndFlight

 Check the type of end flight, guns used and hit, and credit scores
 */

void ScoresEndFlight(u_int16_t end, int8_t land, u_int16_t gunused, u_int16_t totalhits, client_t *client)
{
	u_int16_t i;
	u_int8_t j;
	char buffer[100];

	if(!client)
	{
		return;
	}

	// debite guns used from client

	client->score.costscore += (double)(GetAmmoCost(0) * gunused);

	switch (end)
	{
		case ENDFLIGHT_LANDED:
			if(ScoresCheckCaptured(client))
			{
				ScoresEvent((SCORE_LANDED | SCORE_CAPTURED), client, 0);
			}
			else
			{
				ScoresEvent(SCORE_LANDED, client, 0);
			}
			break;
		case ENDFLIGHT_DITCHED:
			if(ScoresCheckCaptured(client))
			{
				ScoresEvent((SCORE_DITCHED | SCORE_CAPTURED), client, 0);
			}
			else
			{
				ScoresEvent(SCORE_DITCHED, client, 0);
			}
			break;
		case ENDFLIGHT_BAILED:
			if(ScoresCheckCaptured(client))
			{
				ScoresEvent((SCORE_BAILED | SCORE_CAPTURED), client, 0);
			}
			else
			{
				ScoresEvent(SCORE_BAILED, client, 0);
			}
			break;
		case ENDFLIGHT_PILOTKILL:
		case ENDFLIGHT_CRASHED:
		case ENDFLIGHT_DITCHFAILED:
		case ENDFLIGHT_PANCAKE:
			ScoresEvent(SCORE_KILLED, client, 0);
			break;
		default:
			Com_Printf(VERBOSE_DEBUG_SCORES, "ScoresEndFlight(end) Unknown %u\n", end);
//		case ENDFLIGHT_COLLIDED:
	}

	// check if time penalized
	if (!(end == ENDFLIGHT_LANDED || end == ENDFLIGHT_DITCHED) && !(end == ENDFLIGHT_BAILED && client->attached))
	{
		i = (FLIGHT_TIME(client)/1000);
		if (i < flypenalty->value)
		{
			client->flypenalty = flypenalty->value - i;
			client->flypenaltyfield = client->field;
		}
	}

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

	if(gunused || totalhits)
	{
		if (gunused)
		{
			if(totalhits)
				sprintf(my_query, "%s gunused = gunused + '%u',", my_query, gunused);
			else
				sprintf(my_query, "%s gunused = gunused + '%u'", my_query, gunused);
		}
		if (totalhits)
			sprintf(my_query, "%s gunhits = gunhits + '%u'", my_query, totalhits);

		sprintf(my_query, "%s WHERE player_id = '%u'", my_query, client->id);

		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "ScoresEndFlight(update): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			Com_Printf(VERBOSE_WARNING, "Query: %s\n", my_query);
		}
	}

	// Print Flight Defrief

	PPrintf(client, RADIO_WHITE, "==================================================");
	PPrintf(client, RADIO_WHITE, "Flight time: %s", Com_TimeSeconds(FLIGHT_TIME(client)/1000));
		Com_Printf(VERBOSE_DEBUG_SCORES, "Flight Time = %u / 1000\n", FLIGHT_TIME(client));
	PPrintf(client, RADIO_WHITE, "Last mission score: %11.3f - %s mission.", client->lastscore, IsFighter(client) ? "Fighter" : IsBomber(client) ? "Bomber" : "Ground");
	PPrintf(client, RADIO_WHITE, "You've shot %d rounds, hit %d (acc:%.3f%%)", gunused, totalhits, gunused ? (double)totalhits*100/gunused : 0);
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

// TODO: Check medals rules
//	if ((i = ScoresCheckMedals(client)))
//	{
//		PPrintf(client, RADIO_GREEN, "You are awarded with %u medal%s!", i, i > 1 ? "s" : "");
//		PClientMedals(NULL, client);
//	}
}

/**
 ScoresCheckKiller

 Check who killed client

 TODO: huh? CHANGE IsFighter and IsBomber for a plane number function
 */

int8_t ScoresCheckKiller(client_t *client, int32_t *maneuver)
{
	int8_t i, j, k;
	char buffer[128];
	char query_bomber[512];
	char query_ground[512];
	double damage;
	client_t *killer = NULL;
	client_t *dbclient = NULL;

	if(!client)
	{
		return 0;
	}

	if(maneuver)
		*maneuver = 0;

	if (client->inflight) // if not inflight, cant be killed
	{
		if(!client->damaged) // maneuver kill or nothing
		{
			Com_Printf(VERBOSE_DEBUG_SCORES, "Check nearest player (3000 feets radius)\n");
			if ((client->posalt[0] - GetHeightAt(client->posxy[0][0], client->posxy[1][0])) <= 164) // explosions above 50mts are not maneuver kill
			{
				killer = NearPlane(client, client->country, 3000);

				if (killer)
				{
					AddKiller(client, killer);

					dbclient = killer;

					if(maneuver)
						*maneuver = 1;

					Com_Printf(VERBOSE_DEBUG_SCORES, "Found nearest player (%s)\n", killer->longnick);

					if (printkills->value)
					{
						BPrintf(RADIO_YELLOW, "Maneuver kill of %s(%s) by %s(%s)", client->longnick, GetSmallPlaneName(client->plane), killer->longnick, GetSmallPlaneName(killer->plane));

						if(killer->squadron)
							BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(killer->squadron));
					}
					else
					{
						Com_Printf(VERBOSE_CHAT, "Maneuver kill of %s(%s) by %s(%s)\n", client->longnick, GetSmallPlaneName(client->plane), killer->longnick, GetSmallPlaneName(killer->plane));

						if(killer->squadron)
							Com_Printf(VERBOSE_CHAT, "`-> from %s\n", Com_SquadronName(killer->squadron));
					}

					Com_Printf(VERBOSE_KILL, "%s(%s) Maneuver Killed by %s(%s) at %s\n", client->longnick, GetSmallPlaneName(client->plane), killer->longnick, GetSmallPlaneName(killer->plane), Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

					if(IsFighter(killer) && IsFighter(client))
						CalcEloRating(killer /*winner*/, client /*looser*/, ELO_BOTH);

					// EVENT LOGS

					Com_LogEvent(EVENT_KILL, killer->id, client->id);
					Com_LogDescription(EVENT_DESC_PLPLANE, killer->plane, NULL);
					if (IsFighter(killer))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
					else if (IsBomber(killer))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 2, NULL);
					else if (IsGround(killer))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 3, NULL);
					else
					{
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", killer->plane);
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
					}

					Com_LogDescription(EVENT_DESC_PLCTRY, killer->country, NULL);
					Com_LogDescription(EVENT_DESC_PLORD, killer->ord, NULL);
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

					sprintf(my_query, "%s'', '%u', '%u', '%u', '%u',", my_query, killer->id, killer->plane,
						IsFighter(killer) ? 1 : IsBomber(killer) ? 2 : 3, killer->country);

					sprintf(my_query, "%s '%u', '%u', '%u', '%u')", my_query, client->id, client->plane, IsFighter(client) ? 1 : IsBomber(client) ? 2 : 3, client->country);

					if (d_mysql_query(&my_sock, my_query))
					{
						Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
			}
		}
		else // Damaged, valid kill
		{
			Com_Printf(VERBOSE_DEBUG_SCORES, "Now check who did more damage\n");
			for (i = 0, j = -1, damage = 0; i < MAX_HITBY; i++) // check who inflicted more damage
			{
				if (client->hitby[i].dbid) // if client in list
				{
					Com_Printf(VERBOSE_DEBUG_SCORES, "%s - %f\n", client->hitby[i].longnick, client->hitby[i].damage);
					if (client->hitby[i].damage > damage) // if damage > current damage
					{
						damage = client->hitby[i].damage; // set current damage as attacker damage
						//client->hitby[i].damage = 0; // clear damage value
						j = i; // set j as index of max damage
					}
				}
			}

			if(!(j < 0)) // found a killer
			{
				//client->hitby[j] = NULL; // clear killer from list
				dbclient = FindDBClient(client->hitby[j].dbid);

				Com_Printf(VERBOSE_DEBUG_SCORES, "Server has chosen %s as killer!!!\n", client->hitby[j].longnick);

				Com_LogEvent(EVENT_KILL, client->hitby[j].dbid == client->id?0:client->hitby[j].dbid, client->id);
				if (client->hitby[j].dbid != client->id)
				{
					Com_LogDescription(EVENT_DESC_PLPLANE, client->hitby[j].plane, NULL);

					if (IsFighter(NULL, client->hitby[j].plane))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
					else if (IsBomber(NULL, client->hitby[j].plane))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 2, NULL);
					else if (IsGround(NULL, client->hitby[j].plane))
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 3, NULL);
					else
					{
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->hitby[j].plane);
						Com_LogDescription(EVENT_DESC_PLPLTYPE, 1, NULL);
					}

					Com_LogDescription(EVENT_DESC_PLCTRY, client->hitby[j].country, NULL);
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

				/*************************************************
				 Insert values in score_kills table
				 
				 Variables: player_id, player_plane, player_pltype, player_country
				 victim_id, victim_plane, victim_pltype, victim_country
				 *************************************************/

				sprintf(my_query, "INSERT INTO score_kills VALUES(");

				if (client->hitby[j].dbid == client->id)
				{
					strcat(my_query, "'', '0', '0', '0', '0',");
				}
				else
				{
					sprintf(my_query, "%s'', '%u', '%u', '%u', '%u',", my_query, client->hitby[j].dbid, client->hitby[j].plane,
							IsFighter(NULL, client->hitby[j].plane) ? 1 : IsBomber(NULL, client->hitby[j].plane) ? 2 : 3, client->hitby[j].country);
				}

				sprintf(my_query, "%s '%u', '%u', '%u', '%u')", my_query, client->id, client->plane, IsFighter(client) ? 1 : IsBomber(client) ? 2 : 3, client->country);

				if (d_mysql_query(&my_sock, my_query))
				{
					Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}

				if ((client->hitby[j].country == client->country) && (client->hitby[j].dbid != client->id)) // not ack, TK
				{
					if(dbclient)
					{
						// in TK, winner is the killed pilot.
						if(IsFighter(NULL, client->hitby[j].plane) && IsFighter(client))
							CalcEloRating(client /*winner*/, dbclient /*looser*/, ELO_LOOSER);

						if (!client->tkstatus) // if victim is not TK, add penalty to killer
						{
							if (teamkiller->value)
								dbclient->tklimit++;
						}

						if (dbclient->tklimit > 5)
						{
							if (!dbclient->tkstatus)
								Cmd_TK(dbclient->longnick, TRUE, NULL);
							else
								; // TODO: FIXME: BAN CLIENT UNTIL END OF TOD
						}
					}

					snprintf(buffer, sizeof(buffer), "TeamKill of %s(%s) by %s(%s)", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j].longnick, GetSmallPlaneName(client->hitby[j].plane));

					if (client->hitby[j].squadron)
						sprintf(buffer, "%s from %s", buffer, Com_SquadronName(client->hitby[j].squadron));

					if (printkills->value)
					{
						BPrintf(RADIO_GREEN, buffer);
					}

					Com_Printf(VERBOSE_KILL, "%s(%s) Teamkilled by %s(%s) at %s\n", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j].longnick, GetSmallPlaneName(client->hitby[j].plane), Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
				}
				else // enemy kill
				{
					if(dbclient)
					{
						if(IsFighter(NULL, client->hitby[j].plane) && IsFighter(client))
							CalcEloRating(dbclient /*winner*/, client /*looser*/, ELO_BOTH);
					}

					if ((client->status_damage & STATUS_PILOT) && client->chute)
						snprintf(buffer, sizeof(buffer), "Chutekill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j].dbid == client->id ? "Ack Weenies" : client->hitby[j].longnick);
					else if (client->cancollide < 0)
						snprintf(buffer, sizeof(buffer), "Collision kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j].dbid == client->id ? "Ack Weenies" : client->hitby[j].longnick);
					else
						snprintf(buffer, sizeof(buffer), "Kill of %s(%s) by %s", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j].dbid == client->id ? "Ack Weenies" : client->hitby[j].longnick);

					if(client->hitby[j].dbid != client->id)
						sprintf(buffer, "%s(%s)", buffer, GetSmallPlaneName(client->hitby[j].plane));

					if (printkills->value)
					{
						BPrintf(RADIO_YELLOW, buffer);
					}

					Com_Printf(VERBOSE_KILL, "%s(%s) Killed by %s(%s) at %s\n", client->longnick, GetSmallPlaneName(client->plane), client->hitby[j].longnick, GetSmallPlaneName(client->hitby[j].plane), Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
				}

				if ((client->hitby[j].dbid != client->id) && (client->hitby[j].country != client->country))
				{
					if (client->hitby[j].squadron)
					{
						if (printkills->value)
							BPrintf(RADIO_YELLOW, "`-> from %s", Com_SquadronName(client->hitby[j].squadron));
						else
							Com_Printf(VERBOSE_CHAT, "`-> from %s\n", Com_SquadronName(client->hitby[j].squadron));
					}
				}
			}
		}

		if(j >= 0)
		{
			if(client->hitby[j].dbid && client->hitby[j].dbid < DRONE_DBID_BASE && (client->hitby[j].dbid != client->id))
			{
				if (client->hitby[j].country == client->country) // not ack, TK
				{
					if (!client->tkstatus)
						sprintf(my_query, "UPDATE score_penalty SET");
					else
					{
						if (IsFighter(NULL, client->hitby[j].plane))
							sprintf(my_query, "UPDATE score_fighter SET");
						else if (IsBomber(NULL, client->hitby[j].plane))
							sprintf(my_query, "UPDATE score_bomber SET");
						else if (IsGround(NULL, client->hitby[j].plane))
							sprintf(my_query, "UPDATE score_ground SET");
						else
						{
							Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
							sprintf(my_query, "UPDATE score_fighter SET");
						}
					}
				}
				else // not ack kill
				{
					if (IsFighter(NULL, client->hitby[j].plane))
						sprintf(my_query, "UPDATE score_fighter SET");
					else if (IsBomber(NULL, client->hitby[j].plane))
						sprintf(my_query, "UPDATE score_bomber SET");
					else if (IsGround(NULL, client->hitby[j].plane))
						sprintf(my_query, "UPDATE score_ground SET");
					else
					{
						Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->hitby[j].plane);
						sprintf(my_query, "UPDATE score_fighter SET");
					}
				}

				//score
				switch (client->plane)
				{
					case 61:
						strcat(my_query, " killcommandos = killcommandos + '1'");
						break;
					case PLANE_FAU:
						strcat(my_query, " killfau = killfau + '1'");
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
							}
							else if (client->drone & DRONE_KATY)
							{
								strcat(my_query, " killkaty = killkaty + '1'");
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
							break;
						}
					default:
						if (IsGround(client))
						{
							strcat(my_query, " killtank = killtank + '1'");

							if(dbclient)
							{
								dbclient->killssortie++;
								dbclient->killstod++;
							}

							if (client->hitby[j].country != client->country)
							{
								strcat(my_query, ", curr_streak = curr_streak + '1', long_streak = IF(curr_streak > long_streak, curr_streak, long_streak)");
							}
						}
						else
						{
							strcat(my_query, " kills = kills + '1'");

							if(dbclient)
							{
								dbclient->killssortie++;
								dbclient->killstod++;
							}

							if (client->hitby[j].country != client->country)
							{
								strcat(my_query, ", curr_streak = curr_streak + '1', long_streak = IF(curr_streak > long_streak, curr_streak, long_streak)");
							}
						}
				}

				sprintf(my_query, "%s WHERE player_id = '%u'", my_query, client->hitby[j].dbid);

				Com_Printf(VERBOSE_DEBUG_SCORES, "Kill Query: %s\n", my_query);

				if (d_mysql_query(&my_sock, my_query))
				{
					PPrintf(client, RADIO_YELLOW, "ScoresCheckKiller(): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
					Com_Printf(VERBOSE_WARNING, "ScoresCheckKiller(): couldn't query UPDATE error %d: %s query %s\n", mysql_errno(&my_sock), mysql_error(&my_sock), my_query);
				}
			}
		}

		// ASSISTS

		k = 0;

		sprintf(my_query, "UPDATE score_fighter SET assists = assists + '1' WHERE player_id IN(");
		sprintf(query_bomber, "UPDATE score_bomber SET assists = assists + '1' WHERE player_id IN(");
		sprintf(query_ground, "UPDATE score_ground SET assists = assists + '1' WHERE player_id IN(");

		for (i = 0; i < MAX_HITBY; i++) // check who remain in list and give piece score
		{
			if (client->hitby[i].dbid && client->hitby[i].damage && client->hitby[i].dbid < DRONE_DBID_BASE)
			{
				if (client->hitby[i].dbid != client->id && client->hitby[i].dbid != client->hitby[j].dbid /*killer*/) // if not ack damage (dont give piece do acks please, they don't deserves hehehe)
				{
					if (client->hitby[i].country != client->country) // if enemy
					{
						if (printkills->value)
						{
							if((dbclient = FindDBClient(client->hitby[i].dbid)))
								PPrintf(dbclient, RADIO_YELLOW, "You've got a piece of %s", client->longnick);
						}

						Com_Printf(VERBOSE_KILL, "%s(%s) got a piece of %s\n", client->hitby[i].longnick, GetSmallPlaneName(client->hitby[j].plane), client->longnick);

						if (IsFighter(NULL, client->hitby[i].plane))
						{
							sprintf(my_query, "%s'%u',", my_query, client->hitby[i].dbid);
							k |= 1;
						}
						else if (IsBomber(NULL, client->hitby[i].plane))
						{
							sprintf(query_bomber, "%s'%u',", query_bomber, client->hitby[i].dbid);
							k |= 2;
						}
						else if (IsGround(NULL, client->hitby[i].plane))
						{
							sprintf(query_ground, "%s'%u',", query_ground, client->hitby[i].dbid);
							k |= 4;
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
							sprintf(my_query, "%s'%u',", my_query, client->hitby[i].dbid);
							k |= 1;
						}
					}
					else // friendly kill... tsc, tsc, tsc...
					{
						if (printkills->value)
						{
							if((dbclient = FindDBClient(client->hitby[i].dbid)))
							{
								PPrintf(dbclient, RADIO_YELLOW, "You've got a piece of your friend %s", client->longnick);
							}
						}
						else
						{
							Com_Printf(VERBOSE_KILL, "%s(%s) got a piece of his friend %s\n", client->hitby[i].longnick, GetSmallPlaneName(client->hitby[j].plane), client->longnick);
						}
					}
				}
			}
		}

		if (k & 1)
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
		if (k & 2)
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
		if (k & 4)
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

	return k;
}

/**
 ScoresCheckMedals

 Check if client receive a new medal
 */

u_int8_t ScoresCheckMedals(client_t *client)
{
	u_int8_t i, newmedal = 0;
	int16_t value;

	if(!client)
	{
		return 0;
	}

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
							newmedal += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_GRANDSTAR, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_GRANDCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_ORDERFALCON, value, client);
				}
				else
					newmedal += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_SILVERSTAR, value, client);
			}
			else
				newmedal += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_COMBATCROSS, value, client);
		}
		else
			newmedal += ScoresAddMedal(DEED_KILLSSORTIE, MEDAL_COMBATMEDAL, value, client);
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
							newmedal += ScoresAddMedal(DEED_KILLSTOD, MEDAL_GRANDSTAR, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_KILLSTOD, MEDAL_GRANDCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_KILLSTOD, MEDAL_SILVERSTAR, value, client);
				}
				else
					newmedal += ScoresAddMedal(DEED_KILLSTOD, MEDAL_ORDERFALCON, value, client);
			}
			else
				newmedal += ScoresAddMedal(DEED_KILLSTOD, MEDAL_COMBATCROSS, value, client);
		}
		else
			newmedal += ScoresAddMedal(DEED_KILLSTOD, MEDAL_COMBATMEDAL, value, client);
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
							newmedal += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_GRANDSTAR, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_GRANDCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_SILVERSTAR, value, client);
				}
				else
					newmedal += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_ORDERFALCON, value, client);
			}
			else
				newmedal += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_COMBATCROSS, value, client);
		}
		else
			newmedal += ScoresAddMedal(DEED_STRUCTSTOD, MEDAL_COMBATMEDAL, value, client);
	}

	// streak kills
	sprintf(my_query,
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
										newmedal += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										newmedal += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									newmedal += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_ORDERFALCON, value, client);
							}
							else
								newmedal += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_SILVERSTAR, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_STREAKKILLS, MEDAL_COMBATMEDAL, value, client);
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
										newmedal += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										newmedal += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									newmedal += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_ORDERFALCON, value, client);
							}
							else
								newmedal += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_SILVERSTAR, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_BSTREAKKILLS, MEDAL_COMBATMEDAL, value, client);
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
										newmedal += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_GRANDSTAR, value, client);
									}
									else
										newmedal += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_GRANDCROSS, value, client);
								}
								else
									newmedal += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_SILVERSTAR, value, client);
							}
							else
								newmedal += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_ORDERFALCON, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_COMBATCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_CAREERKILLS, MEDAL_COMBATMEDAL, value, client);
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
										newmedal += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_GRANDSTAR, value, client);
									}
									else
										newmedal += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_GRANDCROSS, value, client);
								}
								else
									newmedal += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_SILVERSTAR, value, client);
							}
							else
								newmedal += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_ORDERFALCON, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_COMBATCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_CAREERSTRUCTS, MEDAL_COMBATMEDAL, value, client);
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
										newmedal += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_GRANDSTAR, value, client);
									}
									else
										newmedal += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_GRANDCROSS, value, client);
								}
								else
									newmedal += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_SILVERSTAR, value, client);
							}
							else
								newmedal += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_ORDERFALCON, value, client);
						}
						else
							newmedal += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_COMBATCROSS, value, client);
					}
					else
						newmedal += ScoresAddMedal(DEED_CAREERFIELDS, MEDAL_COMBATMEDAL, value, client);
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

	return newmedal;
}

/**
 new = ScoresAddMedal

 Add a new medal to client if needed
 */

u_int8_t ScoresAddMedal(u_int8_t deed, u_int8_t medal, u_int16_t value, client_t *client)
{
	if(!client)
	{
		return 0;
	}

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

/**
 ScoresCheckCaptured

 Check if clients has been captured on land/ditch/bail
 */

u_int8_t ScoresCheckCaptured(client_t *client)
{
	int16_t i, closed, field;
	u_int32_t radar, distance;

	if(!client)
	{
		return 0;
	}

	field = NearestField(client->posxy[0][0], client->posxy[1][0], FALSE, TRUE, TRUE, &distance);

	if (field >= 0)
	{
		if (field < fields->value)
		{
			if(client->country == arena->fields[field].country)
				return 0;

			i = arena->fields[field].country;
			closed = arena->fields[field].closed;
		}
		else
		{
			if(client->country == arena->cities[field - (int16_t)fields->value].country)
				return 0;

			i = arena->cities[field - (int16_t)fields->value].country;
			closed = arena->cities[field - (int16_t)fields->value].closed;
		}

		radar = i == 1 ? radarrange1->value : i == 2 ? radarrange2->value : i == 3 ? radarrange3->value : i == 4 ? radarrange4->value : 0;
		radar /= 2;

		if (distance < radar && !closed)
		{
			Com_LogEvent(EVENT_CAPTURED, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
			Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);

			PPrintf(client, RADIO_YELLOW, "You have been captured by %s", GetCountry(i));

			return 1;
		}
	}

	return 0;
}

/**
 ScoresCreate

 Create Score rows
 */

void ScoresCreate(client_t *client)
{
	if(!client)
	{
		return;
	}

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

/**
 ResetScores

 Backup log_ and score_ tables;
 Truncate log_ and score_ tables
 Clear all .score files
 */
void ResetScores(void)
{
	u_int8_t i;
	time_t ltime;
	struct tm *timeptr;
	char temp[16];
	char tables[9][18] =
	{ "log_descriptions", "log_events", "score_fighter", "score_bomber", "score_ground", "score_penalty", "score_common", "score_kills", "medals" };

	time(&ltime);
	timeptr = gmtime(&ltime);

	snprintf(temp, sizeof(temp), "%d-%02d-%02d", 1900 + timeptr->tm_year, timeptr->tm_mon + 1, timeptr->tm_mday);

	for (i = 0; i < 9; i++)
	{
		sprintf(my_query, "CREATE TABLE `bkp-%s-%s` SELECT * FROM `%s`", tables[i], temp, tables[i]);

		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "ResetScores(): couldn't query CREATE(%u) error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
		}

		sprintf(my_query, "TRUNCATE %s", tables[i]);

		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "ResetScores(): couldn't query TRUNCATE(%u) error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}

#ifdef _WIN32
	system("del .\\players\\*.score");
#else
	Sys_RemoveFiles("./players/.score");
#endif
}

/**
 BackupScores

 Runs a PHP script that makes a backup of scores to allow compare with next call
 */

void BackupScores(u_int8_t collect_type)
{
	FILE *fp;
	u_int16_t s_year, s_month, s_day;
	u_int16_t e_year, e_month, e_day;
	char *pnextmap;

	if (!arena->mapnum)
	{ // first map of cycle
		s_day = initday->value;
		s_month = initmonth->value;
		s_year = inityear->value;
	}
	else
	{
		s_day = (arena->mapcycle[arena->mapnum - 1].date - ((arena->mapcycle[arena->mapnum - 1].date / 100) * 100)); /* day of the month (1 to 31) */
		s_month = ((arena->mapcycle[arena->mapnum - 1].date - ((arena->mapcycle[arena->mapnum - 1].date / 10000) * 10000)) / 100); /* months (1 to 12) */
		s_year = (arena->mapcycle[arena->mapnum - 1].date / 10000); /* years */
	}

	if ((arena->mapnum + 1) == MAX_MAPCYCLE || !arena->mapcycle[(arena->mapnum + 1)].date)
	{ // last map of cycle
		e_day = endday->value;
		e_month = endmonth->value;
		e_year = endyear->value;
		pnextmap = NULL;
	}
	else
	{
		e_day = (arena->mapcycle[arena->mapnum].date - ((arena->mapcycle[arena->mapnum].date / 100) * 100)); /* day of the month (1 to 31) */
		e_month = ((arena->mapcycle[arena->mapnum].date - ((arena->mapcycle[arena->mapnum].date / 10000) * 10000)) / 100); /* months (1 to 12) */
		e_year = (arena->mapcycle[arena->mapnum].date / 10000); /* years */
		pnextmap = arena->mapcycle[arena->mapnum + 1].mapname;
	}

	sprintf(my_query, "DELETE FROM players WHERE longnick IS NULL"); // remove all logins without nickname

	if (d_mysql_query(&my_sock, my_query))
	{
		Com_Printf(VERBOSE_WARNING, "BackupScores(): couldn't query DELETE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
	}

	fp = fopen("./cron/scores.cfg", "w");
	fprintf(fp, "actual_map=%s\n", arena->mapcycle[arena->mapnum].mapname);
	fprintf(fp, "next_map=%s\n", pnextmap);
	fprintf(fp, "vdate_tod_start=%04.0f-%02.0f-%02.0f\n", inityear->value, initmonth->value, initday->value);
	fprintf(fp, "vdate_map_start=%04u-%02u-%02u\n", s_year, s_month, s_day);
	fprintf(fp, "vdate_map_end=%04u-%02u-%02u\n", e_year, e_month, e_day);
	fprintf(fp, "vdate_tod_end=%04.0f-%02.0f-%02.0f\n", endyear->value, endmonth->value, endday->value);

	switch (collect_type)
	{
		case COLLECT_CYCLE:
			fprintf(fp, "collect_type=cycle\n");
			break;
		case COLLECT_MAP:
			fprintf(fp, "collect_type=map\n");
			break;
		case COLLECT_EVENT:
			fprintf(fp, "collect_type=special\nspecial_name=%s\n", dirname->string);
			break;
		default:
			fprintf(fp, "collect_type=default\n");
			break;
	}

	fclose(fp);

	system("php -f ./cron/cron.php &");
}

/**
 ScoreTehcnologyCost

 Return the cost of plane technology lost based in plane damage
 */

double ScoreTechnologyCost(client_t *client)
{
	return ScorePlaneLife(client) * ScorePlaneCost(client) * arena->costs.technologylost;
}

/**
 GetBuildingCost

 Return the cost of a building type
 */

double GetBuildingCost(u_int8_t type)
{
	if(type < BUILD_MAX)
	{
		return arena->costs.buildtype[type];
	}
	else
		return 0.0;
}

/**
 GetAmmoCost

 Return the cost of a munition type
 */

double GetAmmoCost(u_int8_t type)
{
	if(type < MAX_MUNTYPE)
	{
		if(arena->costs.ammotype[type] >= 0)
			return arena->costs.ammotype[type];
		else
			return 0.0;
	}
	else
		return 0.0;
}

/**
 GetFieldCost

 Return the cost of a field
 */

double GetFieldCost(u_int8_t field)
{
	u_int16_t i;
	double cost = 0.0;

	if(field < fields->value)
	{
		for(i = 0; i < MAX_BUILDINGS; i++)
		{
			if(!arena->fields[field].buildings[i].field)
				break;

			if(IsVitalBuilding(&(arena->fields[field].buildings[i]), TRUE))
			{
				cost += GetBuildingCost(arena->fields[field].buildings[i].type);
			}
		}
	}
	else
	{
		return 0.0;
	}

	return (1.5 * cost);
}

/**
 ScoreLoadCosts

 Load current costs
 */

void ScoreLoadCosts(void)
{
//	arena->costs.buildtype[BUILD_50CALACK] = 1.0; // LoadDamageModel
//	arena->costs.ammotype[MUNTYPE_BULLET];  // LoadAmmo():29
//	arena->costs.planeweight[MAX_PLANES];	// LoadDamageModel():25
//	arena->costs.planemodel[MAX_PLANES];	// LoadDamageModel():24

	arena->costs.takeoff = 1.0;
	arena->costs.newpilot = 150.0;
	arena->costs.technologylost = 0.5;
	arena->costs.informationlost = 1.0;
	arena->costs.life = 100.0;
	arena->costs.planetransport = 0.333; // plane_price / (distance (km) * weight (ton))
	arena->costs.pilottransport = 0.066; // * 0.07
	arena->costs.flighthour = 0.1;
}
