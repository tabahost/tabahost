/*
 * Ship.cpp
 *
 *  Created on: 08/02/2011
 *      Author: franz
 */

// Ao criar o boid, executar:
// 1) loadWaypoints()
// após isto, está apto para entrar no loop run();

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
 Ship::loadWaypoints

 Load waypoints (create or read from file)
 */

void Ship::loadWaypoints()
{
	char file[32];
	u_int8_t i;
	FILE *fp;
	char buffer[128];

	if(!this->isLegal("Ship::loadWaypoints"))
		return;

	Com_Printf(VERBOSE_DEBUG, "Ship::loadWaypoints() %u\n", group);

	snprintf(file, sizeof(file), "./arenas/%s/cv%d.rte", dirname->string, group);

	wptotal = 0;

	if(!(fp = fopen(file, "r")))
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() Cannot open file \"%s\"", file);
		return;
	}

	for(i = 0; fgets(buffer, sizeof(buffer), fp); i++)
	{
		wp[i].x = Com_Atoi((char *) strtok(buffer, ";"));
		wp[i].y = Com_Atoi((char *) strtok(NULL, ";"));

		wptotal++;

		if(wptotal >= MAX_WAYPOINTS)
			break;

		memset(buffer, 0, sizeof(buffer));
	}

	if(!wptotal)
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() error reading \"%s\"", file);
	}

	fclose(fp);
}

/**
 Ship::retarget

 Leader ship retarget waypoint
 */

bool Ship::retarget(doublePoint_t &wp)
{
	if(!this->isLegal("Ship::retarget"))
		return false;

	// Fleet -> update arena->fields position
	//		// update field position
	//		arena->fields[arena->cvs[this->group].field].posxyz[0] = this->Position.x;
	//		arena->fields[arena->cvs[this->group].field].posxyz[1] = this->Position.y;
	//		arena->fields[arena->cvs[this->group].field].posxyz[2] = 59; // 18m

	// Fleet -> recalculate waypoints for group
	//		// update target waypoint (this may be changed manually or automatically for defensive maneuver)
	//		ship->Target.x = arena->cvs[this->group].wp[arena->cvs[this->group].wpnum][0];
	//		ship->Target.y = arena->cvs[this->group].wp[arena->cvs[this->group].wpnum][1];

	Target.x = wp.x;
	Target.y = wp.y;

	// Check waypoint
	if((abs(Target.y - Position.y) > 70) || (abs(Target.x - Position.x) > 70))
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

/**
 Ship::run

 Run frame
 */

int8_t Ship::run()
{
	const double Form[4][6][2] = { { { 2500, 0.25 }, { 2500, -0.25 }, { 4243, 0.75 }, { 4243, -0.75 }, { 3000, 1 }, { 4000, 0 } }, { { 600, 0.25 }, { 600,
			-0.25 }, { 848.5281, 0.5 }, { 848.5281, -0.5 }, { 600, 1 }, { 700, 0 } }, { { 500, 0.5 }, { 500, -0.5 }, { 5, 1 }, { 707.1068, 0.75 }, { 707.1068,
			-0.75 }, { 500, 0 } }, { { 600, 0.1667 }, { 600, -0.1667 }, { 600, 0.5 }, { 6, -0.5 }, { 600, 0.8333 }, { 600, -0.8333 } } };

	if(!this->isLegal("Ship::run"))
		return -1;

	// Ship parts:
	// PLACE_ENGINE1
	// PLACE_REARFUSE | PLACE_CENTERFUSE
	// PLACE_TAILGUN
	// PLACE_NOSEGUN
	// PLACE_LEFTGUN
	// PLACE_RIGHTGUN
	// PLACE_TOPGUN
	// PLACE_BOTTOMGUN
	// PLACE_RGEAR
	// PLACE_LGEAR
	// PLACE_FLAPS

	if(!leader) // I'm the leader!
	{
		this->retarget(wp[wpnum]);

		// if first step, configure all boids
		if(!prepared) // the "prepare" step is to spawn a boid in proper xy position/formation, aiming to next waypoint
		{
			this->prepare();

			u_int8_t i;
			for(i = 0, followers->restart(); followers->next(); i++)
			{
				followers->current()->prepare(Form[formation][i]);
				followers->current()->setPosition(i);
				followers->current()->setPrepared(true); // this is necessary when leader is killed, so avoid newleader to think it is spawning
			}

			prepared = true;
		}

		// Set main ship always as the CV speed, so other ships can sustain the formation
		// TODO: convoy speed verification (speed == more damaged)
		this->setVelMax(17); // 34 feet per second
		this->yaw();
		this->walk();

		// if error sync with wb-drone (e.g.: drone bugged and must be removed)
		if(this->processDroneBoid() < 0)
		{
			return -1;
		}

	}
	else // I'm a follower
	{
		Boid::retarget(Form[formation][position]);
		this->yaw(leader);
		this->walk();

		if(this->processDroneBoid() < 0)
		{
			return -1;
		}
	}

	return 0;
}
