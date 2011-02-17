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

	boids.restart();
	i = 0;

	while(boids.next())
	{
		if(boids.current()->getPlaneType() > PLANETYPE_CV)
		{
			if(i == num)
				return (Ship *)boids.current();
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

/**
 Ship::changeCVRoute

 Change Route of CV, in threathness or by command
 */

void Ship::changeCVRoute(cvs_t *cv, double angle /*0*/, u_int16_t distance /*10000*/, client_t *client)
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
