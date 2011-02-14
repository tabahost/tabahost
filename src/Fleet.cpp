/*
 * Fleet.cpp
 *
 *  Created on: 09/02/2011
 *      Author: franz
 */

#include "Fleet.h"

Fleet::Fleet()
{
	Com_Printf(VERBOSE_DEBUG, "Fleet constructor\n");
	signature = CLASSID_FLEET;
	fleetCount++;
	prepared = 0;
}

Fleet::~Fleet()
{
	Com_Printf(VERBOSE_DEBUG, "Fleet destructor\n");
	fleetCount--;
	boids.clear();
	signature = 0;
}

/**
 Fleet::isLegal

 Class Validator
 */

bool Fleet::isLegal(const char *function)
{
	if(signature != CLASSID_FLEET)
	{
		printf("WARNING: Ilegal class passed by %s\n", function);
		return false;
	}

	return true;
}

/**
 Fleet::run

 Run all boids using a gived formation
 */

void Fleet::run(u_int8_t formation)
{
	if(this->isLegal("Fleet::run"))
	{
		return;
	}
	else
	{
		const double Form[4][6][2] =
		{
		{
		{ 2500, 0.25 },
		{ 2500, -0.25 },
		{ 4243, 0.75 },
		{ 4243, -0.75 },
		{ 3000, 1 },
		{ 4000, 0 } },
		{
		{ 600, 0.25 },
		{ 600, -0.25 },
		{ 848.5281, 0.5 },
		{ 848.5281, -0.5 },
		{ 600, 1 },
		{ 700, 0 } },
		{
		{ 500, 0.5 },
		{ 500, -0.5 },
		{ 5, 1 },
		{ 707.1068, 0.75 },
		{ 707.1068, -0.75 },
		{ 500, 0 } },
		{
		{ 600, 0.1667 },
		{ 600, -0.1667 },
		{ 600, 0.5 },
		{ 6, -0.5 },
		{ 600, 0.8333 },
		{ 600, -0.8333 } } };

		u_int8_t i;
		Boid *mainBoid;
		list<Boid>::iterator boid;

		mainBoid = &boids.front();
		mainBoid->retarget(wp[wpnum]);

		// if first steps, configure all ships
		if(!prepared)
		{
			mainBoid->prepare();

			for(i = 0, boid = boids.begin(); boid != boids.end(); i++)
			{
				boid++; // ignore leader boid
				boid->prepare(mainBoid, Form[formation][i]);
			}

			prepared = 1;
		}

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

		// Set main ship always as the CV speed, so other ships can sustain the formation
		// TODO: convoy speed verification (speed == more damaged)
		mainBoid->setVelMax(17); // 34 feet per second
		mainBoid->yaw();
		mainBoid->walk();

		// if error sync with wb-drone (e.g.: drone bugged and must be removed)
		if(mainBoid->processDroneBoid() < 0)
		{
			boids.pop_front(); // delete first element --> mainShip

			if(boids.empty())
			{
				// no more ships in array, reset it
				this->resetFleet();
			}
			else
			{
				Com_Printf(VERBOSE_DEBUG, "RunShips() ProcessDroneShips(error/kill), going to recursive call\n");
				this->run(formation); // Find next mainship and continue. If no valid mainShip is found, it will/must return here.
			}

			return;
		}

		for(i = 0, boid = boids.begin(); boid != boids.end(); i++)
		{
			boid++; // ignore leader boid
			boid->retarget(mainBoid, Form[formation][i]);
			boid->yaw(mainBoid);
			boid->walk();

			if(boid->processDroneBoid() < 0)
			{
				boids.erase(boid); // remove boid

				if(boids.empty())
				{
					// no more ships in array, reset it
					this->resetFleet();
				}
			}
		}
	}
}

