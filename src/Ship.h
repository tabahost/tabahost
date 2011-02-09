/*
 * Ship.h
 *
 *  Created on: 08/02/2011
 *      Author: franz
 */

#ifndef SHIP_H_
#define SHIP_H_

#define CLASSID_SHIP 0xCB8D0EA2

#include "Boid.h"

class Ship: private Boid
{
	private:
		u_int32_t signature;

	public:
		static u_int16_t shipCount;

		virtual bool isLegal(const char *);

		Ship();
		virtual ~Ship();

		bool retarget(int32_t x, int32_t y);
};

#endif /* SHIP_H_ */
