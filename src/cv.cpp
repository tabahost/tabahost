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
 ChangeCVRoute

 Change Route of CV, in threathness or by command
 */

void ChangeCVRoute(cvs_t *cv, double angle /*0*/, u_int16_t distance /*10000*/, client_t *client)
{
	int8_t angleoffset = 0;

	Com_Printf(VERBOSE_DEBUG, "WP %d\n", cv->wpnum);

	if(!cv->threatened) // step wpnum back, because its an automatic route change or first manual change
	{
		cv->wpnum--;
		cv->threatened = 1;
	}
	// else // mantain wpnum, because is a manual route change

	if(!client)
	{
		// grab the cvpos pathway angle
		angle = AngleTo(cv->ships->Position.x, cv->ships->Position.y, cv->wp[cv->wpnum + 1][0], cv->wp[cv->wpnum + 1][1]);

		// change the route based in the pathway angle
		if(rand() % 100 < 60) // zigzag
		{
			angleoffset = 45 * Com_Pow(-1, cv->zigzag);

			if(cv->zigzag == 1)
			{
				cv->zigzag = 2;
			}
			else
			{
				cv->zigzag = 1;
			}
		}
		else
		{
			angleoffset = 45 * Com_Pow(-1, rand() % 2);
		}

		// check if new waypoint is over land
		if(GetHeightAt(cv->ships->Position.x + (10000 * sin(Com_Rad(angle + angleoffset))), cv->ships->Position.y + (10000 * cos(Com_Rad(angle + angleoffset))))) // WP is over land
		{
			angleoffset *= -1;
		}

		angle += angleoffset;
		distance = 2000;
	}

	cv->wp[cv->wpnum][0] = arena->fields[cv->field].posxyz[0] + (distance * sin(Com_Rad(angle)));
	cv->wp[cv->wpnum][1] = arena->fields[cv->field].posxyz[1] + (distance * cos(Com_Rad(angle)));

	if(client)
	{
		PPrintf(client, RADIO_YELLOW, "Waypoint changed to %s", Com_Padloc(cv->wp[cv->wpnum][0], cv->wp[cv->wpnum][1]));
		PPrintf(client, RADIO_YELLOW, "ETA: %s\"", Com_TimeSeconds(distance / cv->ships->Vel.curr));
	}

	// configure to next wpnum be that nearest to cv->wp[lastwp][0]
	// coded at threatened = 0;
}
