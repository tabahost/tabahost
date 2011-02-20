/*
 * Plane.h
 *
 *  Created on: 20/02/2011
 *      Author: franz
 */

#ifndef PLANE_H_
#define PLANE_H_

#define CLASSID_PLANE 0xE3D3E0E

#include "Boid.h"

class Plane: public Boid
{
	private:
		u_int32_t signature;

	public:
		static u_int16_t planeCount;
		static u_int16_t getPlaneCount(){return planeCount;};
		static void test(u_int8_t group = 1);

		virtual bool isLegal(const char *);

		Plane();
		virtual ~Plane();

		// virtuals
		void prepare(const double *A); // follower prepare (point to formation)
		void prepare(); // leader prepare (point to waypoint)
		void loadWaypoints(u_int8_t wpnum);
		bool retarget(doublePoint_t &wp);
		int8_t run(); // run frame

		void attackNearestPlane();
};

#endif /* PLANE_H_ */
