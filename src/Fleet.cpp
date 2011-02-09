/*
 * Fleet.cpp
 *
 *  Created on: 09/02/2011
 *      Author: franz
 */

#include "Fleet.h"

Fleet::Fleet()
{
	prepared = 0;

}

Fleet::~Fleet()
{
	boids->clear();
}

/**
 Fleet::run

 Run all boids using a gived formation
 */

void Fleet::run(u_int8_t formation)
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
	Ship *mainShip;
	list<Boid>::iterator boid;

	mainShip = &this->boids->front();
	mainShip->retarget();

	// if first steps, configure all ships
	if(!this->prepared)
	{
		mainShip->prepare();

		for(i = 0, boid = this->boids->begin(); boid != this->boids->end(); i++)
		{
			boid++; // ignore leader boid
			boid->prepare(mainShip, Form[formation][i]);
		}

		this->prepared = 1;
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
	mainShip->Vel.max = 17; // 34 feet per second

	mainShip->yaw();
	mainShip->walk();

	if(ProcessDroneShips(mainShip) < 0)
	{
		if(mainShip->drone)
			RemoveDrone(mainShip->drone);
		this->boids->pop_front(); // delete first element --> mainShip

		if(!this->boids->empty())
		{
			Com_Printf(VERBOSE_DEBUG, "RunShips() ProcessDroneShips(error/kill), going to recursive call\n");
			run(formation); // Find next mainship and continue. If no valid mainShip is found, it will return here.
		}
		else
			// no more ships in array, reset it
			ResetCV( group);
		return;
	}

	for(i = 0, ship = arena->cvs[group].ships; ship && i < 6; ship = ship->next)
	{
		if(ship == mainShip)
		{
			continue;
		}

		retarget(ship, mainShip, Form[formation][i++]);
		yaw(ship, mainShip);
		walk( ship);
		if(ProcessDroneShips(ship) < 0)
		{
			if(!RemoveShip(ship)) // no more ships in array, reset it
			{
				ResetCV( group);
				return;
			}
		}
	}
}

