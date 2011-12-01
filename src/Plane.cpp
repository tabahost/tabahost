/*
 * Plane.cpp
 *
 *  Created on: 20/02/2011
 *      Author: franz
 */

#include "arena.h"
#include "client.h"
#include "common.h"
#include "drone.h"
#include "game.h"
#include "Plane.h"

u_int16_t Plane::planeCount;

Plane::Plane()
{
	Com_Printf(VERBOSE_DEBUG, "Plane constructor\n");
	signature = CLASSID_PLANE;
	wpreach = 10;
	planeCount++;
}

Plane::~Plane()
{
	Com_Printf(VERBOSE_DEBUG, "Plane destructor\n");
	planeCount--;
	signature = 0;
}

/**
 Plane::isLegal

 Class Validator
 */

bool Plane::isLegal(const char *function)
{
	if(signature != CLASSID_PLANE)
	{
		printf("WARNING: Ilegal class passed by %s\n", function);
		return false;
	}

	return true;
}

/**
 Plane::attackNearestPlane

 Attack nearest plane with ack/flacks and set leader as threatened
 */

void Plane::attackNearestPlane()
{
	if(!this->isLegal("Plane::attackNearestPlane"))
		return;

	// check if there are enemies around
	client_t *nearplane = NearPlane(drone, country, 15000);

	if(nearplane)
	{
		int32_t distplane = DistBetween(Position.x, Position.y, Position.z, nearplane->posxy[0][0], nearplane->posxy[1][0], nearplane->posalt[0], -1);

		if(!leader /*I'm leader*/&& !threatened && !(arena->frame % 600))
		{
			changeRoute(10000, 0, false);
		}

		// Otto x Airplane
		if(distplane > 0)
		{
			if(distplane <= 3000) // D10
			{

				double temp;
				double speedaz = AngleTo(Position.x, Position.y, nearplane->posxy[0][0], nearplane->posxy[1][0]);
				double speedel = Com_Deg(atan2((double) distplane, (double) (Position.z - nearplane->posalt[0])));

				temp = speedaz;
				speedaz = Eazimuth - speedaz;
				Eazimuth = temp;
				temp = speedel;
				speedel = Eelev - speedel;
				Eelev = temp;

				double speed = sqrt(speedaz * speedaz + speedel * speedel);

				double j;

				// % of hit
				j = -0.003 * (double) distplane + 11.0;
				if(j < 0)
					j = 0;

				j *= (-0.036 * speed + 1.18); // (5,1)(30,0.1) linear
				if(j < 0)
					j = 0;

				if((rand() % 100) < j) // hit
				{
					FireAck(drone, nearplane, distplane, 0);
				}
				else
				{
					// fail
					FireAck(drone, nearplane, distplane, 1);
				}
			}
		}
	}
}

/**
 Plane::loadWaypoints

 Load waypoints (create or read from file)
 */

void Plane::loadWaypoints(u_int8_t wpnum = 1)
{
	if(!this->isLegal("Plane::loadWaypoints"))
		return;

	Com_Printf(VERBOSE_DEBUG, "Plane::loadWaypoints() %u - origin %u - destiny %u\n", group, originField, targetField);

	wptotal = 3;

	wp[0].x = arena->fields[originField - 1].posxyz[0];
	wp[0].y = arena->fields[originField - 1].posxyz[1];
	wp[0].type = 0;
	wp[1].x = arena->fields[targetField - 1].posxyz[0];
	wp[1].y = arena->fields[targetField - 1].posxyz[1];
	wp[1].type = 1; // 0 = wp, 1 = bomb wp
	wp[2].x = arena->fields[originField - 1].posxyz[0];
	wp[2].y = arena->fields[originField - 1].posxyz[1];
	wp[2].type = 0;

	if(wpnum)
		this->wpnum = wpnum;
}

//void Plane::loadWaypoints(u_int8_t wpnum = 1)
//{
//	char file[32];
//	u_int8_t i;
//	FILE *fp;
//	char buffer[128];
//
//	if(!this->isLegal("Plane::loadWaypoints"))
//		return;
//
//	Com_Printf(VERBOSE_DEBUG, "Plane::loadWaypoints() %u\n", group);
//
//	snprintf(file, sizeof(file), "./arenas/%s/plane%d.rte", dirname->string, group);
//
//	wptotal = 0;
//
//	if(!(fp = fopen(file, "r")))
//	{
//		PPrintf(NULL, RADIO_YELLOW, "WARNING: Plane::loadWaypoints() Cannot open file \"%s\"", file);
//		return;
//	}
//
//	for(i = 0; fgets(buffer, sizeof(buffer), fp); i++)
//	{
//		wp[i].x = Com_Atoi((char *) strtok(buffer, ";"));
//		wp[i].y = Com_Atoi((char *) strtok(NULL, ";"));
//
//		wptotal++;
//
//		if(wptotal >= MAX_WAYPOINTS)
//			break;
//
//		memset(buffer, 0, sizeof(buffer));
//	}
//
//	if(wpnum)
//		this->wpnum = wpnum;
//
//	if(!wptotal)
//	{
//		PPrintf(NULL, RADIO_YELLOW, "WARNING: Plane::loadWaypoints() error reading \"%s\"", file);
//	}
//
//	fclose(fp);
//}

/**
 Plane::retarget

 Leader plane retarget waypoint
 */

bool Plane::retarget(coordinates_t &wp)
{
	if(!this->isLegal("Plane::retarget"))
		return false;

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

		if(wp.type == 1) // TEST BOMB
		{
			Com_Printf(VERBOSE_DEBUG, "Drop Bomb at X %u Y %u\n", Target.x, Target.y);

			DropBomb(Target.x, Target.y, 84, drone);
			DropBomb(Target.x, Target.y, 84, drone);
			DropBomb(Target.x, Target.y, 84, drone);
			DropBomb(Target.x, Target.y, 84, drone);
			DropBomb(Target.x, Target.y, 84, drone);

			for(followers->restart(); followers->current(); followers->next())
			{
				DropBomb(Target.x, Target.y, 84, followers->current()->getDrone());
				DropBomb(Target.x, Target.y, 84, followers->current()->getDrone());
				DropBomb(Target.x, Target.y, 84, followers->current()->getDrone());
				DropBomb(Target.x, Target.y, 84, followers->current()->getDrone());
				DropBomb(Target.x, Target.y, 84, followers->current()->getDrone());
			}
		}

		// don't bomb this wp again.
		wp.type = 0;
		// this->loadWaypoints(wpnum); // reload waypoints
		wpnum++;

		//		if(wpnum == wptotal) // reset waypoint index
		//			wpnum = 1;

		return false;
	}
}

/**
 Plane::prepare

 Prepare follower boid before start walking
 */

void Plane::prepare(const double *A)
{
	if(!this->isLegal("Plane::prepare"))
		return;

	Com_Printf(VERBOSE_DEBUG, "Preparing follower group %u\n", group);

	Attitude.x = 0;
	Attitude.y = 0;
	Attitude.z = 0;

	Vel.curr = 0;
	Vel.target = Vel.curr;
	Acel.curr = 0;
	Acel.target = Acel.curr;
	Acel.min = -10;
	Acel.max = 20;
	YawVel.curr = 0;
	YawVel.target = 0;

	Yaw.target = leader->getYawTarget();
	Yaw.curr = leader->getYawCurr();
	Target.x = leader->getPositionX() + A[0] * sin(Yaw.curr + A[1] * M_PI);
	Target.y = leader->getPositionY() + A[0] * cos(Yaw.curr + A[1] * M_PI);
	Position.x = Target.x;
	Position.y = Target.y;
	Position.z = 10000;

	//	switch(plane)
	//	{
	//		case SHIP_CA: // CA 77
	//			radius = 330; // 330 feet
	//			Vel.max = 20; // 40 feet per second
	//			Vel.min = 0.2;
	//			YawVel.max = 1.3334 * M_PI / 180; // 2.6666º per second (in radians)
	//			YawVel.min = -YawVel.max;
	//			break;
	//		default:
	radius = 103; // 103 feet
	Vel.max = 160; // 351 km/h
	Vel.min = 82; // 180 km/h
	YawVel.max = 3 * M_PI / 180; // 6º per second (in radians)
	YawVel.min = -YawVel.max;
	//			Com_Printf(VERBOSE_WARNING, "prepare(): unknown ship type\n");
	//			break;
	//	}
}

/**
 Plane::prepare

 Prepare leader boid before start walking
 */

void Plane::prepare() // main Boid
{
	if(!this->isLegal("Plane::prepare()"))
		return;

	Com_Printf(VERBOSE_DEBUG, "Preparing Leader group %u\n", group);

	Attitude.x = 0;
	Attitude.y = 0;
	Attitude.z = 0;

	Position.x = wp[0].x;
	Position.y = wp[0].y;
	Position.z = 10000;
	Target.x = wp[1].x;
	Target.y = wp[1].y;

	Vel.curr = 0;
	Vel.target = Vel.curr;
	Acel.curr = 0;
	Acel.target = Acel.curr;
	Acel.min = -5;
	Acel.max = 10;
	Yaw.curr = Com_Rad(AngleTo(Position.x, Position.y, Target.x, Target.y));
	Yaw.curr = this->angle(Yaw.curr);
	Yaw.target = Yaw.curr;
	YawVel.curr = 0;
	YawVel.target = 0;

	radius = 103; // 103 feet
	Vel.max = 150; // 330 km/h
	Vel.min = 82; // 180 km/h
	YawVel.max = 1.5 * M_PI / 180; // 3º per second (in radians)
	YawVel.min = -YawVel.max;
}

/**
 Plane::run

 Run frame
 */

int8_t Plane::run()
{
	const double Form[4][6][2] = { { { 200, 0.75 }, { 200, -0.75 }, { 282, 1 }, { 400, -0.75 }, { 400, 0.75 }, { 565, 1 } }, { { 600, 0.25 }, { 600, -0.25 }, {
			848.5281, 0.5 }, { 848.5281, -0.5 }, { 600, 1 }, { 700, 0 } } };

	if(!this->isLegal("Plane::run"))
		return -1;

	if(!leader) // I'm the leader!
	{
		this->retarget(wp[wpnum]);

		if(wpnum == wptotal)
		{
			return -1; // end of mission
		}

		// if first step, configure all boids
		if(!prepared) // the "prepare" step is to spawn a boid in proper xy position/formation, aiming to next waypoint
		{
			CPrintf(country, RADIO_YELLOW, "Starting mission from F%u to F%u with %s", originField, targetField, GetSmallPlaneName(plane));

			this->prepare();

			u_int8_t i;
			for(i = 0, followers->restart(); followers->current(); followers->next(), i++)
			{
				followers->current()->prepare(Form[formation][i]);
				followers->current()->setPrepared(true); // this is necessary when leader is killed, so avoid newleader to think it is spawning
			}

			prepared = true;
		}

		// Set main ship always as the CV speed, so other ships can sustain the formation
		// TODO: convoy speed verification (speed == more damaged)
		this->setVelMax(150); // 330 km/h
		this->yaw();
		Attitude.y = (int16_t) (-200.0 * YawVel.curr / YawVel.max); // Roll
		this->walk();
		this->attackNearestPlane();

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
		Attitude.y = (int16_t) (-200.0 * YawVel.curr / YawVel.max); // Roll
		this->walk();
		this->attackNearestPlane();

		if(this->processDroneBoid() < 0)
		{
			return -1;
		}
	}

	return 0;
}
/**
 Plane::createMission

 Send drones from field x to field y
 */

void Plane::createMission(u_int8_t country)
{
	u_int16_t origin;
	u_int16_t destiny;
	u_int8_t planemodel;
	u_int16_t field, i, j;

	field = (u_int8_t) (fields->value - cvs->value);

	for(j = 0, i = rand() % field; j < field; j++, i = rand() % field)
	{
		if((arena->fields[i].country == country) && (arena->fields[i].type >= FIELD_LITTLE) && (arena->fields[i].type <= FIELD_MAIN)
				&& !arena->fields[i].underattack && !arena->fields[i].closed)
			break;
	}

	if(j == field) // not found
		return;

	origin = i;

	for(j = 0, i = rand() % maxplanes; j < maxplanes; j++, i = rand() % maxplanes)
	{
		if(arena->rps[i].used && IsBomber(NULL, i) && !IsCargo(NULL, i) && (arena->fields[origin].rps[i] >= 1))
			break;
	}

	if(j == maxplanes) // not found
		return;

	planemodel = i;

	for(j = 0, i = rand() % field; j < field; j++, i = rand() % field)
	{
		if((arena->fields[i].country != country) && !arena->fields[i].closed)
			break;
	}

	if(j == field) // not found
		return;

	destiny = i;

	origin++;
	destiny++;

	Com_Printf(VERBOSE_DEBUG, "Mission %s from F%d to F%d with %s\n", GetCountry(country), origin, destiny, GetSmallPlaneName(planemodel));
	Plane::createMission(origin, destiny, planemodel);
}

/**
 Plane::createMission

 Send drones from field x to field y
 */

void Plane::createMission(u_int16_t origin, u_int16_t destiny, u_int8_t planemodel)
{
	Plane *leader, *plane;
	u_int8_t group, i;
	client_t *drone;

	//removeGroup(group);

	// Create leader
	leader = new Plane();
	leader->createFollowers(new Boidlist());

	for(i = 0, group = rand() % 255 + 1; i < 256; i++, group = rand() % 255 + 1)
	{
		if(!Boid::hasGroup(group))
			break;
	}

	leader->setGroup(group);
	leader->setFormation(0);
	leader->setCountry(arena->fields[origin - 1].country);
	leader->setPlane(planemodel); // B17F
	leader->setOriginField(origin);
	leader->setTargetField(destiny);
	leader->loadWaypoints();

	drone = AddDrone(DRONE_PLANE, leader->Position.x, leader->Position.y, leader->Position.z, leader->country, leader->plane, NULL);

	if(!drone)
	{
		delete leader;
		return;
	}
	else
	{
		leader->setDrone(drone);
	}

	for(u_int8_t i = 1; i < 3; i++)
	{
		plane = new Plane();
		leader->addFollower(plane); // calls plane->setCountry(), plane->setPosition(), plane->setFormation(), plane->setLeader() and plane->setGroup()
		plane->setPlane(planemodel);
		plane->setOriginField(origin);
		plane->setTargetField(destiny);
		drone = AddDrone(DRONE_PLANE, plane->Position.x, plane->Position.y, plane->Position.z, plane->country, plane->plane, NULL);
		if(!drone)
		{
			delete plane;
			return;
		}
		else
			plane->setDrone(drone);
	}
	return;
}
