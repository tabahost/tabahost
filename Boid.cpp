/**
 * Boid.cpp
 *
 *  @date 03/02/2011
 *  @author franz
 */

#include "Boid.h"

u_int16_t Boid::boidCount;
Boidlist Boid::boids;

/// Constructor

Boid::Boid()
{
	Com_Printf(VERBOSE_DEBUG, "Boid constructor\n");
	signature = CLASSID_BOID;
	leader = NULL;
	drone = NULL;
	port = NULL;
	wpreach = 100;
	followers = NULL;
	Eelev = 0.0;
	Eazimuth = 0.0;
	group = 0;
	threatened = false;
	outofport = false;
	prepared = false;
	field = 0;
	formation = 0;
	pos = 0;
	zigzag = 1;
	wpnum = 1;
	snprintf(logfile, sizeof(logfile), "%s,boid%u,%s,%u", mapname->string, group, GetCountry(country), (u_int32_t) time(NULL));
	boids.push_back(this);
	boidCount++;
}

/// Destructor

Boid::~Boid()
{
	Com_Printf(VERBOSE_DEBUG, "Boid destructor\n");
	boids.erase(this);
	boidCount--;

	if(drone)
		RemoveDrone(drone);

	if(this->hasLeader()) // I'm a follower, so I must ask leader to unregister myself
	{
		leader->removeFollowers(this);
	}
	else // I'm the leader, so I must nominate other leader
	{
		if(!followers->empty()) // there is someone else
		{
			Boid *newleader;

			newleader = followers->front();
			newleader->followers = followers; // give the member list to newleader
			newleader->leader = NULL;
			newleader->loadWaypoints(wpnum);
			followers->pop_front(); // remove newleader from list
			u_int8_t i;
			for(i = 0, followers->restart(); followers->current(); followers->next(), i++)
			{
				Com_Printf(VERBOSE_DEBUG, "Transferring leadership %u\n", i);
				followers->current()->leader = newleader;
				followers->current()->pos = i; // reset the formation position
			}
			followers = NULL; // clean followers point just to assert that this is no longer anybody's leader
		}
	}

	signature = 0;
}

bool Boid::operator==(const Boid &b)
{ // example: a == b

	if(!this->isLegal("Boid::operator=="))
		return false;

	//if(this == &b)
	if(this->drone == b.drone)
		return true;
	else
		return false;
}

/**
 Boid::runBoids

 Run all boids
 */

void Boid::runBoids()
{
	for(boids.restart(); boids.current(); boids.next())
	{
		if(boids.current()->run() < 0)
		{
			boids.erase_del(boids.current());
		}
	}
}

/**
 Boid::hasGroup

 Return true if found a boid of given group
 */

bool Boid::hasGroup(u_int8_t a)
{
	for(boids.restart(); boids.current(); boids.next())
	{
		if(boids.current()->getGroup() == a)
		{
			return true;
		}
	}

	return false;
}

/**
 Boid::countGroups

 Return the number of groups for a given country
 */

u_int8_t Boid::countGroups(u_int8_t country)
{
	u_int8_t count = 0;

	for(boids.restart(); boids.current(); boids.next())
	{
		if(!boids.current()->hasLeader() && boids.current()->getCountry() == country)
		{
			count++;
		}
	}

	return count;
}

/**
 Boid::removeGroup

 Remove all boids form a group
 */

void Boid::removeGroup(u_int8_t group)
{
	for(boids.restart(); boids.current(); boids.next())
	{
		if(boids.current()->group == group)
		{
			boids.erase_del(boids.current());
		}
	}
}

/**
 Boid::logPosition

 Log all leader position
 */

void Boid::logPosition()
{
	FILE *fp;
	char filename[128];

	for(boids.restart(); boids.current(); boids.next())
	{
		if(boids.current()->leader == NULL)
		{
			snprintf(filename, sizeof(filename), "./logs/players/%s.ail", boids.current()->logfile);

			if(!(fp = fopen(filename, "a")))
			{
				Com_Printf(VERBOSE_WARNING, "Couldn't append file \"%s\"\n", filename);
			}
			else
			{
				fprintf(fp, "%d;%d;%f;%f;%u;%u;%u\n", boids.current()->Position.x, boids.current()->Position.y, boids.current()->Vel.curr, Com_Deg(boids.current()->Yaw.curr), boids.current()->threatened, boids.current()->country, (u_int32_t) time(NULL));
				fclose(fp);
			}
		}
	}
}

/**
 Boid::isLegal

 Class Validator
 */

bool Boid::isLegal(const char *function)
{
	if(signature != CLASSID_BOID)
	{
		printf("WARNING: Ilegal class passed by %s\n", function);
		return false;
	}

	return true;
}

/**
 Boid::addFollower

 Remove a follower from list
 */

void Boid::addFollower(Boid *follower)
{
	if(!this->isLegal("Boid::addFollower"))
		return;

	Com_Printf(VERBOSE_DEBUG, "addFollower\n");

	if(follower->hasLeader())
	{
		Com_Printf(VERBOSE_WARNING, "Boid::addFollower() %s already has a leader %s\n", follower->drone->longnick, follower->leader->drone->longnick);
		return;
	}

	if(followers)
	{
		if(followers->back())
		{
			follower->setPosition(followers->back()->getPosition() + 1);
		}
		else
		{
			follower->setPosition(0);
		}
		follower->setCountry(country);
		follower->setFormation(formation);
		follower->setGroup(group);
		follower->setLeader(this);
		followers->push_back(follower);
	}
}

/**
 Boid::removeFollowers

 Remove a follower from list
 */

void Boid::removeFollowers(Boid *follower)
{
	if(!this->isLegal("Boid::removeFollowers"))
		return;

	if(followers)
		followers->erase(follower);
}

/**
 Boid::walk

 Walk forward one frame
 */

void Boid::walk()
{
	if(!this->isLegal("Boid::walk"))
		return;

	// IncVeloc
	Vel.curr = Vel.curr + Acel.curr;
	if(Vel.curr > Vel.max)
		Vel.curr = Vel.max;
	else if(Vel.curr < Vel.min)
		Vel.curr = Vel.min;

	Position.x = Position.x + Vel.curr * sin(Yaw.curr);
	Position.y = Position.y + Vel.curr * cos(Yaw.curr);

}

/**
 Boid::angle

 Return a normalized angle
 */

double Boid::angle(double ang)
{
	static float F = 2 * M_PI;

	if(!this->isLegal("Boid::angle"))
		return 0.0;

	while(ang >= F)
		ang -= F;
	while(ang < 0)
		ang += F;
	return ang;

}

/**
 Boid::angleDef

 Return a normalized angle
 */

double Boid::angleDef(double ang)
{
	if(!this->isLegal("Boid::angleDef"))
		return 0.0;

	ang = this->angle(ang);
	if(ang > M_PI)
		ang -= 2 * M_PI;
	return ang;
}

/**
 Boid::yaw

 Adjust Yaw to waypoint
 */

void Boid::yaw(Boid *leader)
{
	double dx, dy;

	if(!this->isLegal("Boid::yaw"))
		return;

	dx = Target.x - Position.x;
	dy = Target.y - Position.y;

	// ajusta angulo alvo
	if(sqrt(dx * dx + dy * dy) < wpreach) // ship reached WP
	{
		if(leader)
			Yaw.target = this->angle(leader->Yaw.curr);
		else
			Yaw.target = Yaw.curr;
	}
	else
	{
		Yaw.target = Com_Rad(AngleTo(Position.x, Position.y, Target.x, Target.y));
	}

	Yaw.target = this->angle(Yaw.target);
	// ajusta velocidade angular
	// YawVel = Yawtarget-Yaw;
	YawVel.curr = this->angleDef(Yaw.target - Yaw.curr); // TODO: Change this to YawVel.accel
	if(YawVel.curr > YawVel.max)
		YawVel.curr = YawVel.max;
	else if(YawVel.curr < YawVel.min)
		YawVel.curr = YawVel.min;
	// incrementa yaw
	//if(MODULUS(Yaw.curr - Yaw.target) > (0.01 * M_PI))
	Yaw.curr = this->angle(Yaw.curr + YawVel.curr);
	Attitude.z = 3600 - (int32_t) floor(Com_Deg(Yaw.curr) * 10);
	// ajusta velocidade
	Vel.target = sqrt(dx * dx + dy * dy) / 3;
	dx = Vel.max * (1 - MODULUS(this->angleDef(Yaw.target - Yaw.curr) / (1 * M_PI))); // 1 - xº/180º

	if(dx < Vel.min)
		dx = Vel.min;

	if(Vel.target > dx)
		Vel.target = dx;

	Acel.curr = Vel.target - Vel.curr; // controle P
	if(Acel.curr > Acel.max)
		Acel.curr = Acel.max;
	else if(Acel.curr < Acel.min)
		Acel.curr = Acel.min;
	// Vel.curr = Vel.target;
}

void Boid::yaw()
{
	if(!this->isLegal("Boid::yaw()"))
		return;

	yaw((Boid *) NULL);
}

/**
 Boid::retarget

 Adjust follower next waypoint position in formation
 */

void Boid::retarget(const double *A)
{
	if(!this->isLegal("Boid::retarget"))
		return;

	Target.x = leader->Position.x + A[0] * sin(leader->Yaw.curr + A[1] * M_PI);
	Target.y = leader->Position.y + A[0] * cos(leader->Yaw.curr + A[1] * M_PI);
}

/**
 Boid::changeRoute

 Change route from client command or just by threat
 */

void Boid::changeRoute(u_int16_t distance, double angle, bool water, client_t *client)
{
	int8_t angleoffset = 0;

	if(!threatened)
	{
		if(!client)
		{
			int32_t dx = Position.x - wp[wpnum].x;
			int32_t dy = Position.y - wp[wpnum].y;

			// don't change route if close to next waypoint
			if(sqrt(dx * dx + dy * dy) < (distance * sqrt(2)))
				return;
		}

		// step wpnum back, because its an automatic route change or first manual change
		wpnum--;
		threatened = 1;
	}
	// else // mantain wpnum, because is a manual route change

	if(!client)
	{
		// grab the cvpos pathway angle
		angle = AngleTo(Position.x, Position.y, wp[wpnum + 1].x, wp[wpnum + 1].y);

		// change the route based in the pathway angle
		if(rand() % 100 < 60) // zigzag
		{
			angleoffset = 30 * Com_Pow(-1, zigzag);

			if(zigzag == 1)
				zigzag = 2;
			else
				zigzag = 1;
		}
		else
		{
			angleoffset = 30 * Com_Pow(-1, rand() % 2);
		}

		// check if new waypoint is over land
		if(water && GetHeightAt(Position.x + (10000 * sin(Com_Rad(angle + angleoffset))), Position.y + (10000 * cos(Com_Rad(angle + angleoffset))))) // WP is over land
		{
			angleoffset *= -1;
		}

		angle += angleoffset;
	}

	wp[wpnum].x = Position.x + (distance * sin(Com_Rad(angle)));
	wp[wpnum].y = Position.y + (distance * cos(Com_Rad(angle)));

	if(client)
	{
		PPrintf(client, RADIO_YELLOW, "Waypoint changed to %s", Com_Padloc(wp[wpnum].x, wp[wpnum].y));
		PPrintf(client, RADIO_YELLOW, "ETA: %s\"", Com_TimeSeconds(distance / Vel.curr));
	}

	// configure to next wpnum be that nearest to wp[lastwp][0]
	// coded at threatened = 0;
}

/**
 Boid::processDroneBoid

 Copy Boid attitude to wbdrone structure
 */

int8_t Boid::processDroneBoid()
{
	client_t *drone;

	if(!this->isLegal("Boid::processDroneBoid"))
		return -1;

	if(!(drone = this->drone)) // boid not linked with a drone
	{
		return -1;
	}

	if(drone->bugged) // drone bugged, so boid must be deleted
	{
		BPrintf(RADIO_YELLOW, "DroneBoid %s bugged, and will be removed", drone->longnick);
		return -1;
	}

	// drone killed, so boid must be deleted


	switch(this->getPlaneType())
	{
		case PLANETYPE_FIGHTER:
		case PLANETYPE_BOMBER2:
		case PLANETYPE_BOMBER4:
			if(drone->status_damage & ((1 << PLACE_REARFUSE) | (1 << PLACE_CENTERFUSE) | (1 << PLACE_LWING) | (1 << PLACE_RWING) | (1 << PLACE_PILOT)))
			{
				ScoresEvent(SCORE_KILLED, drone, 0);
				return -1;
			}
			break;
		case PLANETYPE_VEHICLE:
			if(drone->status_damage & ((1 << PLACE_ENGINE1) | (1 << PLACE_ENGINE3) | (1 << PLACE_CENTERFUSE) | (1 << PLACE_PILOT)))
			{
				ScoresEvent(SCORE_KILLED, drone, 0);
				return -1;
			}
			break;
		case PLANETYPE_CV:
		case PLANETYPE_CA:
		case PLANETYPE_DD:
			if(drone->status_damage & ((1 << PLACE_REARFUSE) | (1 << PLACE_CENTERFUSE) | (1 << PLACE_RGEAR) | (1 << PLACE_LGEAR)))
			{
				ScoresEvent(SCORE_KILLED, drone, 0);
				return -1;
			}
			break;
		default:
			if(drone->status_damage & ((1 << PLACE_REARFUSE) | (1 << PLACE_CENTERFUSE) | (1 << PLACE_LWING) | (1 << PLACE_RWING) | (1 << PLACE_PILOT)))
			{
				ScoresEvent(SCORE_KILLED, drone, 0);
				return -1;
			}
			break;
	}

	DroneVisibleList(drone);

	drone->posxy[0][0] = Position.x; // X
	drone->posxy[1][0] = Position.y; // Y
	drone->posalt[0] = Position.z; // Z
	drone->speedxyz[0][0] = Vel.curr * sin(Yaw.curr); // X
	drone->speedxyz[1][0] = Vel.curr * cos(Yaw.curr); // Y
	drone->speedxyz[2][0] = 0; // Z
	drone->angles[0][0] = Attitude.x; // Pitch
	drone->angles[1][0] = Attitude.y; // Roll
	drone->angles[2][0] = Attitude.z; // Yaw
	drone->accelxyz[0][0] = Acel.curr * sin(Yaw.curr); // X
	drone->accelxyz[1][0] = Acel.curr * cos(Yaw.curr); // Y
	drone->accelxyz[2][0] = 0; // Z
	drone->aspeeds[0][0] = 0; // Roll
	drone->aspeeds[1][0] = 0; // Pitch
	drone->aspeeds[2][0] = 0;//(int32_t)floor(Com_Deg(YawVel.curr)); // debug * 2); // Yaw - degrees per second?

	drone->offset = -500;
	drone->timer += 500;

	return 0;
}
