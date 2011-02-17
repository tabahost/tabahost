/*
 * Ship.cpp
 *
 *  Created on: 08/02/2011
 *      Author: franz
 */

// Ao criar o boid:
// 1) definir group
// 2) executar loadWaypoints()
// 3) definir field
// 4) definir formation
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

void Ship::loadWaypoints(u_int8_t wpnum = 1)
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

	this->wpnum = wpnum;

	if(!wptotal)
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() error reading \"%s\"", file);
	}

	fclose(fp);
}

/**
 Ship::processFieldBoid

 Boid interface with wb-field
 */

void Ship::processFieldBoid()
{
	// update field position
	arena->fields[field].posxyz[0] = Position.x;
	arena->fields[field].posxyz[1] = Position.y;
	arena->fields[field].posxyz[2] = 59; // 18m
}

/**
 Ship::retarget

 Leader ship retarget waypoint
 */

bool Ship::retarget(doublePoint_t &wp)
{
	if(!this->isLegal("Ship::retarget"))
		return false;

	this->processFieldBoid();

	Target.x = wp.x;
	Target.y = wp.y;

	// Check waypoint
	if((abs(Target.y - Position.y) > 70) || (abs(Target.x - Position.x) > 70))
	{
		return true;
	}
	else // Next waypoint
	{
		threatened = 0;
		wpnum++;

		if(wpnum == wptotal) // reset waypoint index
			wpnum = 1;

		this->loadWaypoints(wpnum); // reload waypoints from file

		return false;
	}
}

/**
 Ship::ResetShips

 Reset ship formation
 */

void Ship::ResetShips(u_int8_t group)
{
	Ship *leader, *ship;
	client_t *drone;
	// CREATE ALL SHIPS AGAIN
	Com_Printf(VERBOSE_DEBUG, "ResetCV() group %u\n", group);

	removeGroup(group);

	// Create leader
	leader = new Ship();
	leader->setGroup(group);
	leader->loadWaypoints();
	leader->setFormation(0);
	leader->setPort(&(arena->fields[group-1]));
	leader->setField(group-1);
	leader->setCountry(arena->fields[group-1].country);
	arena->fields[group-1].cv = leader;

	leader->setPlane(arena->fields[group-1].fleetships[0]);

	drone = AddDrone(DRONE_SHIP, leader->Position.x, leader->Position.y, 0, leader->country, leader->plane, NULL);
	if(!drone)
	{
		delete leader;
		Com_Printf(VERBOSE_WARNING, "ResetCV() group %u - error creating leader drone\n", group);
		return;
	}
	else
		ship->setDrone(drone);

	for(u_int8_t i = 1; i < arena->fields[group-1].fleetshipstotal; i++)
	{
		ship = new Ship();
		leader->addFollower(ship); // calls ship->setCountry(), ship->setPosition(), ship->setFormation(), ship->setLeader() and ship->setGroup()
		ship->setPlane(arena->fields[group-1].fleetships[i]);
		drone = AddDrone(DRONE_SHIP, ship->Position.x, ship->Position.y, 0, ship->country, ship->plane, NULL);
		if(!drone)
		{
			delete ship;
			Com_Printf(VERBOSE_WARNING, "ResetCV() group %u - error creating drone %u\n", group, i);
			return;
		}
		else
			ship->setDrone(drone);
	}

	return;
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
		Boid::retarget(Form[formation][pos]);
		this->yaw(leader);
		this->walk();

		if(this->processDroneBoid() < 0)
		{
			return -1;
		}
	}

	return 0;
}
