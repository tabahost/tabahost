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
 Fleet::readCVWaypoints

 Read CV waypoints from file
 */

void Fleet::readCVWaypoints(u_int8_t group)
{
	char file[32];
	u_int8_t i;
	FILE *fp;
	char buffer[128];

	Com_Printf(VERBOSE_DEBUG, "ReadCVWaypoints() group %u\n", group);

	snprintf(file, sizeof(file), "./arenas/%s/cv%d.rte", dirname->string, group);

	arena->cvs[group].wptotal = 0;

	if (!(fp = fopen(file, "r")))
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() Cannot open file \"%s\"", file);
		return;
	}

	for (i = 0; fgets(buffer, sizeof(buffer), fp); i++)
	{
		arena->cvs[group].wp[i][0] = Com_Atoi((char *)strtok(buffer, ";"));
		arena->cvs[group].wp[i][1] = Com_Atoi((char *)strtok(NULL, ";"));

		arena->cvs[group].wptotal++;

		if (arena->cvs[group].wptotal >= MAX_WAYPOINTS)
			break;

		memset(buffer, 0, sizeof(buffer));
	}

	if (!arena->cvs[group].wptotal)
	{
		PPrintf(NULL, RADIO_YELLOW, "WARNING: ReadCVWaypoints() error reading \"%s\"", file);
	}

	fclose(fp);
}

/**
 Fleet::resetFleet

 Reset fleet or prepare it for the first time
 */

void Fleet::resetFleet()
{
	if(this->isLegal("Fleet::resetFleet"))
	{
		return;
	}
	else
	{
		// CREATE ALL SHIPS AGAIN
		Com_Printf(VERBOSE_DEBUG, "ResetCV() group %u\n", group);

		ReadCVWaypoints(group);
		RemoveAllShips(group);
		CreateAllShips(group);

		threatened = 0;
		outofport = 0;
		prepared = 0;
		wpnum = 1;
		snprintf(logfile, sizeof(logfile), "%s,cv%u,%s,%u", mapname->string, id, GetCountry(country), (u_int32_t) time(NULL));
	}
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

