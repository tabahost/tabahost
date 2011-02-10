/*
 * Boid.cpp
 *
 *  Created on: 03/02/2011
 *      Author: franz
 */

// #include <deque>
// http://www.dreamincode.net/forums/topic/35344-c-deque-tutorial/
// http://www.cplusplus.com/reference/stl/deque/

#include "Boid.h"

/// Constructor

Boid::Boid()
{
	Com_Printf(VERBOSE_DEBUG, "Boid constructor\n");
	signature = CLASSID_BOID;
	boidCount++;
}

/// Destructor

Boid::~Boid()
{
	Com_Printf(VERBOSE_DEBUG, "Boid destructor\n");
	boidCount--;
	if(drone)
		RemoveDrone(drone);

	if(this == leader) // I'm the leader, so I must nominate other leader
	{
		if(!followers.empty()) // there is someone else
		{
			list<Boid>::iterator boid;

			followers.front().leader = &followers.front(); // set new leader
			followers.front().followers.clear(); // clear follower list to make sure

			boid = followers.begin(); // get first boid
			boid++; // skip the leader;
			while(boid != followers.end()) // recreate the list on new leader
			{
				boid->leader = followers.front().leader;
				leader->followers.push_back(*boid);
			}

			followers.clear();
		}
	}
	else // I'm a follower, so I must ask leader to unregister myself
	{
		if(leader)
			leader->followers.remove(*this);         
	}
	signature = 0;
}

bool Boid::operator==(const Boid &b)
{ // example: a == b

	if(this->isLegal("Boid::operator=="))
	{
		if(this == &b)
			return true;
		else
			return false;
	}
	else
	{
		return false;
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
 Boid::walk

 Walk forward one frame
 */

void Boid::walk()
{
	if(this->isLegal("Boid::walk"))
	{
		return;
	}
	else
	{
		// IncVeloc
		Vel.curr = Vel.curr + Acel.curr;
		if(Vel.curr > Vel.max)
			Vel.curr = Vel.max;
		else if(Vel.curr < Vel.min)
			Vel.curr = Vel.min;

		Position.x = Position.x + Vel.curr * sin(Yaw.curr);
		Position.y = Position.y + Vel.curr * cos(Yaw.curr);
	}
}

/**
 Boid::angle

 Return a normalized angle
 */

double Boid::angle(double ang)
{
	if(this->isLegal("Boid::angle"))
	{
		return 0.0;
	}
	else
	{
		static float F = 2 * M_PI;

		while(ang >= F)
			ang -= F;
		while(ang < 0)
			ang += F;
		return ang;
	}
}

/**
 Boid::angleDef

 Return a normalized angle
 */

double Boid::angleDef(double ang)
{
	if(this->isLegal("Boid::angleDef"))
	{
		return 0.0;
	}
	else
	{
		ang = this->angle(ang);
		if(ang > M_PI)
			ang -= 2 * M_PI;
		return ang;
	}
}

/**
 Boid::yaw

 Adjust Yaw to waypoint
 */

void Boid::yaw(Boid *leader)
{
	double dx, dy;

	if(this->isLegal("Boid::yaw"))
	{
		return;
	}
	else
	{
		dx = Target.x - Position.x;
		dy = Target.y - Position.y;

		// ajusta angulo alvo
		if(sqrt(dx * dx + dy * dy) < 100) // ship reached WP
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
		YawVel.curr = this->angleDef(Yaw.target - Yaw.curr);
		if(YawVel.curr > YawVel.max)
			YawVel.curr = YawVel.max;
		else if(YawVel.curr < YawVel.min)
			YawVel.curr = YawVel.min;
		// incrementa yaw
		//if(MODULUS(Yaw.curr - Yaw.target) > (0.01 * M_PI))
		Yaw.curr = this->angle(Yaw.curr + YawVel.curr);
		// ajusta velocidade
		Vel.target = sqrt(dx * dx + dy * dy) / 3;
		dx = Vel.max * (1 - MODULUS(this->angleDef(Yaw.target - Yaw.curr) / (0.5 * M_PI))); // 1 - xº/90º

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
}

void Boid::yaw()
{
	if(this->isLegal("Boid::yaw()"))
	{
		return;
	}
	else
	{
		yaw((Boid *) NULL);
	}
}

/**
 Boid::retarget

 Adjust next waypoint position in formation
 */

void Boid::retarget(Boid *leader, const double *A)
{
	if(this->isLegal("Boid::retarget"))
	{
		return;
	}
	else
	{
		Target.x = leader->Position.x + A[0] * sin(leader->Yaw.curr + A[1] * M_PI);
		Target.y = leader->Position.y + A[0] * cos(leader->Yaw.curr + A[1] * M_PI);
	}
}

/**
 Boid::prepare

 Prepare boid before start walking
 */

void Boid::prepare(Boid *leader, const double *A)
{
	if(this->isLegal("Boid::prepare"))
	{
		return;
	}
	else
	{
		Yaw.target = leader->Yaw.target;
		Yaw.curr = leader->Yaw.curr;
		Target.x = leader->Position.x + A[0] * sin(leader->Yaw.curr + A[1] * M_PI);
		Target.y = leader->Position.y + A[0] * cos(leader->Yaw.curr + A[1] * M_PI);
		Position.x = Target.x;
		Position.y = Target.y;
	}
}

void Boid::prepare() // main Boid
{
	if(this->isLegal("Boid::prepare()"))
	{
		return;
	}
	else
	{
		Yaw.target = Com_Rad(AngleTo(Position.x, Position.y, Target.x, Target.y));
		Yaw.target = this->angle(Yaw.target);
		Yaw.curr = Yaw.target;
	}
}

/**
 Boid::setVelMax

 Set Vel Max
 */

void Boid::setVelMax(double max)
{
	if(this->isLegal("Boid::setVelMax"))
	{
		return;
	}
	else
	{
		Vel.max = max;
	}
}

/**
 Boid::processDroneBoid

 Copy Boid attitude to wbdrone structure
 */

int8_t Boid::processDroneBoid()
{
	client_t *drone;

	if(this->isLegal("Boid::processDroneBoid"))
	{
		return -1;
	}
	else
	{
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
		if(drone->status_damage & ((1 << PLACE_REARFUSE) | (1 << PLACE_CENTERFUSE) | (1 << PLACE_RGEAR) | (1 << PLACE_LGEAR)))
		{
			ScoresEvent(SCORE_KILLED, drone, 0);
			return -1;
		}

		DroneVisibleList(drone);

		drone->posxy[0][0] = Position.x; // X
		drone->posxy[1][0] = Position.y; // Y
		drone->posalt[0] = 0; // Z
		drone->speedxyz[0][0] = Vel.curr * sin(Yaw.curr); // X
		drone->speedxyz[1][0] = Vel.curr * cos(Yaw.curr); // Y
		drone->speedxyz[2][0] = 0; // Z
		drone->angles[0][0] = 0; // Roll
		drone->angles[1][0] = 0; // Pitch
		drone->angles[2][0] = 3600 - (int32_t) floor(Com_Deg(Yaw.curr) * 10); // Yaw
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
}
