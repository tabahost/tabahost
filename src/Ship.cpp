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

	if(wpnum)
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
/*
 void Ship::sendCVDots()
 {
 u_int8_t i, j, k;
 Ship *ship;
 wb3allaiplanesupdate_t *cvdot;
 u_int8_t buffer[30];

 cvdot = (wb3allaiplanesupdate_t *) buffer;

 memset(buffer, 0, sizeof(buffer));

 //	for (country = 1; country <= 4; country++)
 //	{
 for(i = 0, j = 0; i < cvs->value; i++)
 {
 for(ship = arena->cvs[i].ships; ship; ship = ship->next)
 {
 cvdot->packetid = htons(Com_WBhton(0x0015));
 cvdot->number = htons(j);
 cvdot->posx = htonl(ship->Position.x);
 cvdot->posy = htonl(ship->Position.y);
 cvdot->unk1 = 0;
 cvdot->unk2 = htonl(dpitch->value);
 cvdot->country = htonl(ship->country);
 cvdot->plane = htonl(ship->plane);
 cvdot->slot = htons(j++);

 memset(arena->thaisent, 0, sizeof(arena->thaisent));

 for(k = 0; k < maxentities->value; k++)
 {
 if((clients[k].country == 3 || clients[k].country == 1) && (clients[k].country == ship->country) && clients[k].inuse && !clients[k].drone
 && clients[k].ready) // && !clients[k].inflight)
 {
 if(clients[k].thai) // SendCVDots
 { // this case assume that all AI have access to all cvdots, including enemies. This may cause dot packets to be repeated by num of coutries in game
 if(arena->thaisent[clients[k].thai].b)
 continue;
 else
 arena->thaisent[clients[k].thai].b = 1;
 }

 //						if (clients[k].mapdots)
 //							ClearMapDots(&clients[k]);

 //						clients[k].mapdots = 1;
 SendPacket(buffer, sizeof(buffer), &clients[k]);
 }
 }

 //				memset(buffer, 0, sizeof(buffer));
 }
 }
 //	}
 }
 */
/**
 Ship::getShipByNum

 Return Ship client_t from cvnum
 */

Ship *Ship::getShipByNum(u_int8_t num)
{
	u_int8_t i;

	i = 0;

	for(boids.restart(); boids.current(); boids.next())
	{
		if(boids.current()->getPlaneType() > PLANETYPE_CV)
		{
			if(i == num)
				return (Ship *) boids.current();
			i++;
		}
	}

	return NULL;
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

		// if error sync with wb-drone (e.g.: drone bugged and must be removed)
		if(this->processDroneBoid() < 0)
		{
			return -1;
		}

		// check if there are enemies around
		client_t *nearplane = NearPlane(drone, country, 15000);

		if(nearplane)
		{
			int32_t distplane = DistBetween(Position.x, Position.y, 0, nearplane->posxy[0][0], nearplane->posxy[1][0], nearplane->posalt[0], -1);

			if(!threatened && !(arena->frame % 600))
			{
				changeRoute();
			}

			// Ship x Airplane
			if(distplane > 0)
			{
				double speed = sqrt(nearplane->speedxyz[0][0] * nearplane->speedxyz[0][0] + nearplane->speedxyz[1][0] * nearplane->speedxyz[1][0]
						+ nearplane->speedxyz[2][0] * nearplane->speedxyz[2][0]);
				int16_t j;

				if(distplane <= 4000)
				{
					// % of hit
					j = (int16_t) (-0.003 * (float) distplane + 11.0);
					if(j < 0)
						j = 0;
					j = (int16_t) ((float) j * (-0.001 * speed + 1.3));
					if(j < 0)
						j = 0;

					if((rand() % 100) < j) // hit
						FireAck(drone, nearplane, distplane, 0);
					else
						// fail
						FireAck(drone, nearplane, distplane, 1);
				}
				else if(!(arena->frame % 300)) // 3 sec
				{
					// % of hit
					j = (int16_t) (-0.001 * (float) distplane + 15.0);
					if(j < 0)
						j = 0;
					j = (int16_t) ((float) j * (-0.001 * speed + 1.3));
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

		// Ship x CV|Field
		if(!((arena->frame - drone->frame) % ((u_int32_t) cvdelay->value * 100)) && !(field >= fields->value))
		{
			u_int32_t distshipfield = 0;
			u_int8_t targetfield;

			// check nearest CV
			for(u_int8_t i = 0; i < cvs->value; i++)
			{
				if(country != arena->fields[(u_int16_t)(fields->value - cvs->value + i)].country)
				{
					int32_t dist = DistBetween(Position.x, Position.y, 0, arena->fields[(u_int16_t)(fields->value - cvs->value + i)].posxyz[0],
							arena->fields[(u_int16_t)(fields->value - cvs->value + i)].posxyz[1], 0, (int32_t) cvrange->value);

					if(dist > 0)
					{
						if(!distshipfield || ((u_int32_t)dist < distshipfield))
						{
							distshipfield = (u_int32_t)dist;
							targetfield = (u_int16_t)fields->value - cvs->value + i;
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
						cvFire(arena->cities[(u_int32_t)(targetfield - fields->value)].posxyz[0], arena->cities[(u_int32_t)(targetfield - fields->value)].posxyz[1]);
					}
				}
			}
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
