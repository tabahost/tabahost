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
	signature = 0;
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
		this->Vel.curr = this->Vel.curr + this->Acel.curr;
		if(this->Vel.curr > this->Vel.max)
			this->Vel.curr = this->Vel.max;
		else if(this->Vel.curr < this->Vel.min)
			this->Vel.curr = this->Vel.min;

		this->Position.x = this->Position.x + this->Vel.curr * sin(this->Yaw.curr);
		this->Position.y = this->Position.y + this->Vel.curr * cos(this->Yaw.curr);
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
		ang = angle(ang);
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
		dx = this->Target.x - this->Position.x;
		dy = this->Target.y - this->Position.y;

		// ajusta angulo alvo
		if(sqrt(dx * dx + dy * dy) < 100) // ship reached WP
		{
			if(leader)
				this->Yaw.target = angle(leader->Yaw.curr);
			else
				this->Yaw.target = this->Yaw.curr;
		}
		else
		{
			this->Yaw.target = Com_Rad(AngleTo(this->Position.x, this->Position.y, this->Target.x, this->Target.y));
		}

		this->Yaw.target = angle(this->Yaw.target);
		// ajusta velocidade angular
		// this->YawVel = this->Yawtarget-this->Yaw;
		this->YawVel.curr = angleDef(this->Yaw.target - this->Yaw.curr);
		if(this->YawVel.curr > this->YawVel.max)
			this->YawVel.curr = this->YawVel.max;
		else if(this->YawVel.curr < this->YawVel.min)
			this->YawVel.curr = this->YawVel.min;
		// incrementa yaw
		//if(MODULUS(this->Yaw.curr - this->Yaw.target) > (0.01 * M_PI))
		this->Yaw.curr = angle(this->Yaw.curr + this->YawVel.curr);
		// ajusta velocidade
		this->Vel.target = sqrt(dx * dx + dy * dy) / 3;
		dx = this->Vel.max * (1 - MODULUS(angleDef(this->Yaw.target - this->Yaw.curr) / (0.5 * M_PI))); // 1 - xº/90º

		if(dx < this->Vel.min)
			dx = this->Vel.min;

		if(this->Vel.target > dx)
			this->Vel.target = dx;

		this->Acel.curr = this->Vel.target - this->Vel.curr; // controle P
		if(this->Acel.curr > this->Acel.max)
			this->Acel.curr = this->Acel.max;
		else if(this->Acel.curr < this->Acel.min)
			this->Acel.curr = this->Acel.min;
		// this->Vel.curr = this->Vel.target;
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
		this->Target.x = leader->Position.x + A[0] * sin(leader->Yaw.curr + A[1] * M_PI);
		this->Target.y = leader->Position.y + A[0] * cos(leader->Yaw.curr + A[1] * M_PI);
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
		this->Yaw.target = leader->Yaw.target;
		this->Yaw.curr = leader->Yaw.curr;
		this->Target.x = leader->Position.x + A[0] * sin(leader->Yaw.curr + A[1] * M_PI);
		this->Target.y = leader->Position.y + A[0] * cos(leader->Yaw.curr + A[1] * M_PI);
		this->Position.x = this->Target.x;
		this->Position.y = this->Target.y;
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
		this->Yaw.target = Com_Rad(AngleTo(this->Position.x, this->Position.y, this->Target.x, this->Target.y));
		this->Yaw.target = angle(this->Yaw.target);
		this->Yaw.curr = this->Yaw.target;
	}
}

/**
 Boid::processDroneBoid

 Copy Boid attitude to wbdrone structure
 */

int8_t Boid::processDroneBoid()
{
	client_t *drone;

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

	drone->posxy[0][0] = this->Position.x; // X
	drone->posxy[1][0] = this->Position.y; // Y
	drone->posalt[0] = 0; // Z
	drone->speedxyz[0][0] = this->Vel.curr * sin(this->Yaw.curr); // X
	drone->speedxyz[1][0] = this->Vel.curr * cos(this->Yaw.curr); // Y
	drone->speedxyz[2][0] = 0; // Z
	drone->angles[0][0] = 0; // Roll
	drone->angles[1][0] = 0; // Pitch
	drone->angles[2][0] = 3600 - (int32_t) floor(Com_Deg(this->Yaw.curr) * 10); // Yaw
	drone->accelxyz[0][0] = this->Acel.curr * sin(this->Yaw.curr); // X
	drone->accelxyz[1][0] = this->Acel.curr * cos(this->Yaw.curr); // Y
	drone->accelxyz[2][0] = 0; // Z
	drone->aspeeds[0][0] = 0; // Roll
	drone->aspeeds[1][0] = 0; // Pitch
	drone->aspeeds[2][0] = 0;//(int32_t)floor(Com_Deg(this->YawVel.curr)); // debug * 2); // Yaw - degrees per second?

	drone->offset = -500;
	drone->timer += 500;

	return 0;
}
