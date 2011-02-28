/*
 * Ship.cpp
 *
 *  Created on: 08/02/2011
 *      Author: franz
 */

#include "Ship.h"

u_int16_t Ship::shipCount;

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
		PPrintf(NULL, RADIO_YELLOW, "WARNING: Ship::loadWaypoints() Cannot open file \"%s\"", file);
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

	if(wpnum)
		this->wpnum = wpnum;

	if(!wptotal)
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: Ship::loadWaypoints() error reading \"%s\"", file);
	}

	fclose(fp);
}

/**
 Ship::prepare

 Prepare follower boid before start walking
 */

void Ship::prepare(const double *A)
{
	if(!this->isLegal("Ship::prepare"))
		return;

	Com_Printf(VERBOSE_DEBUG, "Preparing follower group %u\n", group);

	Attitude.x = 0;
	Attitude.y = 0;
	Attitude.z = 0;

	Vel.curr = 0;
	Vel.target = Vel.curr;
	Acel.curr = 0;
	Acel.target = Acel.curr;
	Acel.min = -2;
	Acel.max = 3;
	YawVel.curr = 0;
	YawVel.target = 0;

	Yaw.target = leader->getYawTarget();
	Yaw.curr = leader->getYawCurr();
	Target.x = leader->getPositionX() + A[0] * sin(Yaw.curr + A[1] * M_PI);
	Target.y = leader->getPositionY() + A[0] * cos(Yaw.curr + A[1] * M_PI);
	Position.x = Target.x;
	Position.y = Target.y;
	Position.z = 0;

	switch(plane)
	{
		case SHIP_CA: // CA 77
			radius = 330; // 330 feet
			Vel.max = 20; // 40 feet per second
			Vel.min = 0.2;
			YawVel.max = 1.3334 * M_PI / 180; // 2.6666º per second (in radians)
			YawVel.min = -YawVel.max;
			break;
		case SHIP_DD: // DD 74
			radius = 330; // 330 feet
			Vel.max = 20; // 40 feet per second
			Vel.min = 0.2;
			YawVel.max = 1.5 * M_PI / 180; // 3º per second (in radians)
			YawVel.min = -YawVel.max;
			break;
		default:
			Com_Printf(VERBOSE_WARNING, "prepare(): unknown ship type\n");
			break;
	}
}

/**
 Ship::prepare

 Prepare leader boid before start walking
 */

void Ship::prepare() // main Boid
{
	if(!this->isLegal("Ship::prepare()"))
		return;

	Com_Printf(VERBOSE_DEBUG, "Preparing Leader group %u\n", group);

	Attitude.x = 0;
	Attitude.y = 0;
	Attitude.z = 0;

	Position.x = wp[0].x;
	Position.y = wp[0].y;
	Position.z = 0;
	Target.x = wp[1].x;
	Target.y = wp[1].y;

	Vel.curr = 0;
	Vel.target = Vel.curr;
	Acel.curr = 0;
	Acel.target = Acel.curr;
	Acel.min = -2;
	Acel.max = 3;
	Yaw.curr = Com_Rad(AngleTo(Position.x, Position.y, Target.x, Target.y));
	Yaw.curr = this->angle(Yaw.curr);
	Yaw.target = Yaw.curr;
	YawVel.curr = 0;
	YawVel.target = 0;


	// KAGA-ENTERPRISE
	radius = 800; // 800 feet
	Vel.max = 17; // 34 feet per second
	Vel.min = 0.2;
	YawVel.max = 1 * M_PI / 180; // 2º per second (in radians)
	YawVel.min = -YawVel.max;
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

bool Ship::retarget(coordinates_t &wp)
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
 Ship::resetShips

 Reset ship formation
 */

void Ship::resetShips(u_int8_t group)
{
	Ship *leader, *ship;
	client_t *drone;
	// CREATE ALL SHIPS AGAIN
	Com_Printf(VERBOSE_DEBUG, "ResetCV() group %u\n", group);

	removeGroup(group);

	// Create leader
	leader = new Ship();
	leader->createFollowers(new Boidlist());
	leader->setGroup(group);
	leader->loadWaypoints();
	leader->setFormation(0);
	leader->setPort(&(arena->fields[group - 1]));
	leader->setField(group - 1);
	leader->setCountry(arena->fields[group - 1].country);
	arena->fields[group - 1].cv = leader;
	//	leader->setPosition(arena->fields[group - 1].posxyz[0], arena->fields[group - 1].posxyz[1], arena->fields[group - 1].posxyz[2]);
	leader->setPlane(arena->fields[group - 1].fleetships[0]);

	drone = AddDrone(DRONE_SHIP, leader->Position.x, leader->Position.y, 0, leader->country, leader->plane, NULL);

	if(!drone)
	{
		delete leader;
		Com_Printf(VERBOSE_WARNING, "ResetCV() group %u - error creating leader drone\n", group);
		return;
	}
	else
	{
		leader->setDrone(drone);
	}

	for(u_int8_t i = 1; i < arena->fields[group - 1].fleetshipstotal; i++)
	{
		ship = new Ship();
		leader->addFollower(ship); // calls ship->setCountry(), ship->setPosition(), ship->setFormation(), ship->setLeader() and ship->setGroup()
		ship->setPlane(arena->fields[group - 1].fleetships[i]);
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
 Ship::cvFire

 Fires artillary fire to nearest enemy field
 */

void Ship::cvFire(int32_t destx, int32_t desty)
{
	u_int8_t i, j;

	switch(this->getPlaneType())
	{
		case PLANETYPE_CV:
			i = 5;
			break;
		case PLANETYPE_CA:
			i = 3;
			break;
		case PLANETYPE_DD:
			i = 2;
			break;
		default:
			i = 2;
			break;
	}

	for(j = 0; j < i; j++)
	{
		ThrowBomb(FALSE, Position.x, Position.y, 59, destx, desty, 0, NULL);
	}
	ThrowBomb(TRUE, Position.x, Position.y, 0, destx, desty, 0, NULL);
}

/**
 Ship::sendCVDots

 Send CV dots at radar
 */

void Ship::sendCVDots()
{
	u_int8_t i;
	wb3allaiplanesupdate_t *cvdot;
	u_int8_t buffer[30];

	if((arena->frame % 500)) // 5 seconds
		return;

	cvdot = (wb3allaiplanesupdate_t *) buffer;

	memset(buffer, 0, sizeof(buffer));

	//	for (country = 1; country <= 4; country++)
	//	{

	cvdot->packetid = htons(Com_WBhton(0x0015));
	cvdot->number = htons(this->getShipNum());
	cvdot->posx = htonl(Position.x);
	cvdot->posy = htonl(Position.y);
	cvdot->unk1 = 0;
	cvdot->unk2 = htonl(dpitch->value);
	cvdot->country = htonl(country);
	cvdot->plane = htonl(plane);
	cvdot->slot = htons(this->getShipNum());

	memset(arena->thaisent, 0, sizeof(arena->thaisent));

	for(i = 0; i < maxentities->value; i++)
	{
		if((clients[i].country == 3 || clients[i].country == 1) && (clients[i].country == country) && clients[i].inuse && !clients[i].drone && clients[i].ready) // && !clients[i].inflight)
		{
			if(clients[i].thai) // SendCVDots
			{ // this case assume that all AI have access to all cvdots, including enemies. This may cause dot packets to be repeated by num of coutries in game
				if(arena->thaisent[clients[i].thai].b)
					continue;
				else
					arena->thaisent[clients[i].thai].b = 1;
			}

			//	if (clients[i].mapdots)
			//		ClearMapDots(&clients[i]);

			//	clients[i].mapdots = 1;
			SendPacket(buffer, sizeof(buffer), &clients[i]);
		}
	}

	//				memset(buffer, 0, sizeof(buffer));
	//	}
}

/**
 Ship::getShipByNum

 Return Ship number
 */

u_int8_t Ship::getShipNum()
{
	u_int8_t i, j;
	Ship *leader;

	for(i = 0, j = 0; i < fields->value; i++)
	{
		if(arena->fields[i].type >= FIELD_CV && arena->fields[i].type <= FIELD_SUBMARINE)
		{
			leader = arena->fields[i].cv;

			if(this == leader)
			{
				return j;
			}
			else
			{
				for(j++, arena->fields[i].cv->followers->restart(); arena->fields[i].cv->followers->current(); arena->fields[i].cv->followers->next(), j++)
				{
					if(this == arena->fields[i].cv->followers->current())
						return j;
				}
			}
		}
	}

	return 0;
}

/**
 Ship::getShipByNum

 Return Ship client_t from cvnum
 */

Ship *Ship::getShipByNum(u_int8_t num)
{
	u_int8_t i;

	for(i = 0, boids.restart(); boids.current(); boids.next())
	{
		if(IsShip(boids.current()->getDrone()))
		{
			if(i == num)
			{
				return (Ship *) boids.current();
			}
			i++;
		}
	}

	return NULL;
}

/**
 Ship::attackNearestPlane

 Attack nearest plane with ack/flacks and set leader as threatened
 */

void Ship::attackNearestPlane()
{
	if(!this->isLegal("Ship::attackNearestPlane"))
		return;

	// check if there are enemies around
	client_t *nearplane = NearPlane(drone, country, 15000);

	if(nearplane)
	{
		int32_t distplane = DistBetween(Position.x, Position.y, 0, nearplane->posxy[0][0], nearplane->posxy[1][0], nearplane->posalt[0], -1);

		if(!leader /*I'm leader*/&& !threatened && !(arena->frame % 600))
		{
			changeRoute();
		}

		// Ship x Airplane
		if(distplane > 0)
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

			if(distplane <= 3000) // D10
			{
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
			else if(!(arena->frame % 300)) // 3 sec
			{
				// % of hit
				j = (int16_t) (-0.001 * (float) distplane + 15.0);
				if(j < 0)
					j = 0;

				if((rand() % 100) < j) // hit
					FireFlak(drone, nearplane, distplane, 0);
				else
					// fail
					FireFlak(drone, nearplane, distplane, 1);
			}
		}
	}
}

/**
 Ship::attackNearestField

 Attack nearest field/CV with artillery
 */

void Ship::attackNearestField()
{
	if(!((arena->frame - drone->frame) % ((u_int32_t) cvdelay->value * 100)) && !(field >= fields->value))
	{
		u_int32_t distshipfield = 0;
		u_int8_t targetfield;

		// check nearest CV
		for(u_int8_t i = 0; i < cvs->value; i++)
		{
			if(country != arena->fields[(u_int16_t) (fields->value - cvs->value + i)].country)
			{
				int32_t dist = DistBetween(Position.x, Position.y, 0, arena->fields[(u_int16_t) (fields->value - cvs->value + i)].posxyz[0],
						arena->fields[(u_int16_t) (fields->value - cvs->value + i)].posxyz[1], 0, (int32_t) cvrange->value);

				if(dist > 0)
				{
					if(!distshipfield || ((u_int32_t) dist < distshipfield))
					{
						distshipfield = (u_int32_t) dist;
						targetfield = (u_int16_t) fields->value - cvs->value + i;
					}
				}
			}
		}

		// check nearest field
		if(!targetfield)
		{
			targetfield = NearestField(Position.x, Position.y, country, TRUE, FALSE, &distshipfield);
		}

		if(targetfield >= 0 && distshipfield < (u_int32_t) cvrange->value && targetfield != field)
		{
			if(targetfield < fields->value)
			{
				if(!arena->fields[targetfield].closed)
				{
					cvFire(arena->fields[targetfield].posxyz[0], arena->fields[targetfield].posxyz[1]);
				}
			}
			else
			{
				if(!arena->cities[targetfield - (int16_t) fields->value].closed)
				{
					cvFire(arena->cities[(u_int32_t) (targetfield - fields->value)].posxyz[0],
							arena->cities[(u_int32_t) (targetfield - fields->value)].posxyz[1]);
				}
			}
		}
	}
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
			for(i = 0, followers->restart(); followers->current(); followers->next(), i++)
			{
				followers->current()->prepare(Form[formation][i]);
				followers->current()->setPrepared(true); // this is necessary when leader is killed, so avoid newleader to think it is spawning
			}

			prepared = true;
		}

		// Set main ship always as the CV speed, so other ships can sustain the formation
		// TODO: convoy speed verification (speed == more damaged)
		this->setVelMax(17); // 34 feet per second
		this->yaw();
		this->walk();
		this->attackNearestPlane();
		this->attackNearestField();
		this->sendCVDots();

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
		this->attackNearestPlane();
		this->attackNearestField();
		this->sendCVDots();

		if(this->processDroneBoid() < 0)
		{
			return -1;
		}
	}

	return 0;
}

/**
 Ship::addRemoveCVScreen

 Adds or remove a plane to client screen
 */

void Ship::addRemoveCVScreen(client_t *client, u_int8_t remove, u_int8_t unk1, u_int8_t cvnum)
{
	u_int8_t buffer[16];
	wb3aifillslot_t *aifillslot;

	memset(buffer, 0, sizeof(buffer));
	aifillslot = (wb3aifillslot_t *) buffer;

	aifillslot->packetid = htons(Com_WBhton(0x0008)); // nick, country, plane == 0 -> remove
	aifillslot->slot = 0;

	if(!remove)
	{
		if(drone)
		{
			aifillslot->shortnick = htonl(drone->shortnick);
			aifillslot->country = htonl(drone->country);
			aifillslot->plane = htons(drone->plane);
			client->deck = this;
		}
		else
			return;
	}
	else
		client->deck = NULL;

	aifillslot->unk1 = htons(unk1);
	aifillslot->cvnum = cvnum;

	SendPacket(buffer, sizeof(buffer), client);
	//SendPlaneStatus(plane, client);
}
