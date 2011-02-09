/*
 * Ship.cpp
 *
 *  Created on: 08/02/2011
 *      Author: franz
 */

#include "Ship.h"

Ship::Ship()
{
	Com_Printf(VERBOSE_DEBUG, "Ship constructor\n");
	signature = CLASSID_SHIP;
	shipCount++;
}

Ship::~Ship()
{
	Com_Printf(VERBOSE_DEBUG, "Ship destructor\n");
	shipCount--;
	signature = 0;
}

/**
 Ship::isLegal

 Class Validator
 */

bool Ship::isLegal(const char *function)
{
	if(signature != CLASSID_SHIP)
	{
		printf("WARNING: Ilegal class passed by %s\n", function);
		return false;
	}

	return true;
}

/**
 Ship::retarget

 Leader ship retarget waypoint
 */

bool Ship::retarget(int32_t x, int32_t y)
{
	if(this->isLegal("Ship::retarget"))
	{
		return false;
	}
	else
	{

// Fleet -> update arena->fields position
//		// update field position
//		arena->fields[arena->cvs[this->group].field].posxyz[0] = this->Position.x;
//		arena->fields[arena->cvs[this->group].field].posxyz[1] = this->Position.y;
//		arena->fields[arena->cvs[this->group].field].posxyz[2] = 59; // 18m

// Fleet -> recalculate waypoints for group
//		// update target waypoint (this may be changed manually or automatically for defensive maneuver)
//		ship->Target.x = arena->cvs[this->group].wp[arena->cvs[this->group].wpnum][0];
//		ship->Target.y = arena->cvs[this->group].wp[arena->cvs[this->group].wpnum][1];

		this->Target.x = x;
		this->Target.y = y;

		// Check waypoint
		if((abs(this->Target.y - this->Position.y) > 70) || (abs(this->Target.x - this->Position.x) > 70))
			return true;
		else
		// Next waypoint
			return false;

// Fleet
//		ReadCVWaypoints(this->group); // reload waypoints from file
//
//		if(arena->cvs[this->group].threatened)
//		{
//			arena->cvs[this->group].threatened = 0;
//		}
//
//		arena->cvs[this->group].wpnum++;
//
//		if(arena->cvs[this->group].wpnum == arena->cvs[this->group].wptotal) // reset waypoint index
//		{
//			arena->cvs[this->group].wpnum = 1;
//		}
	}
}
