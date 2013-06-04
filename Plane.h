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
		u_int16_t originField;
		u_int16_t targetField;

	public:
		static u_int16_t planeCount;
		static u_int16_t getPlaneCount(){return planeCount;};
		static void createMission(u_int8_t country);
		static void createMission(u_int16_t origin, u_int16_t destiny, u_int8_t planemodel);

		virtual bool isLegal(const char *);

		Plane();
		virtual ~Plane();

		// virtuals
		void prepare(const double *A); // follower prepare (point to formation)
		void prepare(); // leader prepare (point to waypoint)
		void loadWaypoints(u_int8_t wpnum);
		bool retarget(coordinates_t &wp);
		int8_t run(); // run frame

		void setOriginField(u_int16_t a){originField = a;};
		void setTargetField(u_int16_t a){targetField = a;};

		void attackNearestPlane();
};

#endif /* PLANE_H_ */
