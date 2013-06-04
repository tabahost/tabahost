/*
 * Vehicle.h
 *
 *  Created on: 04/03/2011
 *      Author: franz
 */

#ifndef VEHICLE_H_
#define VEHICLE_H_

#define CLASSID_VEHICLE 0x23037428

#include "Boid.h"

class Vehicle: public Boid
{
	private:
		u_int32_t signature;
		u_int16_t originField;
		u_int16_t targetField;

	public:
		static u_int16_t vehicleCount;
		static u_int16_t getPlaneCount(){return vehicleCount;};
		static void createMission(u_int8_t country);
		static void createMission(u_int16_t origin, u_int16_t destiny, u_int8_t planemodel);

		virtual bool isLegal(const char *);

		Vehicle();
		virtual ~Vehicle();

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

#endif /* VEHICLE_H_ */
