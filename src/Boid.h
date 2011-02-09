/*
 * Boid.h
 *
 *  Created on: 03/02/2011
 *      Author: franz
 */

#ifndef BOID_H_
#define BOID_H_

#define CLASSID_BOID 0x87EF9895

#include "shared.h"

typedef struct doublePoint_s // TDoublePoint
{
		int32_t x;
		int32_t y;
} doublePoint_t;

typedef struct value1_s // TValue1
{
		double curr;
		double target;
} value1_t;

typedef struct value2_s // TValue2
{
		double curr;
		double target;
		double max;
		double min;
} value2_t;

class Boid
{
	private:
		u_int32_t signature;

	protected:
		u_int8_t plane; // ship plane: KAGA, ENTERPRISE, Etc.
		u_int8_t type; // ship type: 0 = CV; 1 = CA; 2 = DD
		u_int8_t country; // ship country (1 = red, 3 = gold)
		u_int8_t group; // group index
		doublePoint_t Position; // Position
		doublePoint_t Target; // Target Position / Next waypoint
		int32_t radius; // ship radius
		value2_t Vel; // Speed
		value2_t Acel; // Acceleration
		value1_t Yaw; // Heading
		value2_t YawVel; // Yaw change speed
		struct client_s *drone; // Drone that this Boid is associated with
		//Boid *next; // pointer to next ship in chain list

	public:
		static u_int16_t boidCount;

		virtual bool isLegal(const char *);

		Boid();
		virtual ~Boid();

		double angle(double ang);
		double angleDef(double ang);

		int8_t processDroneBoid(); // boid interface with wb-drone

		void walk(); // make a move forward
		void yaw(Boid *leader); // follower yaw
		void yaw(); // leader yaw
		void retarget(Boid *leader, const double *A); // follower retarget (target leader formation)
		virtual bool retarget(int32_t x, int32_t y); // leader retarget (target next waypoint)
		void prepare(Boid *leader, const double *A); // follower prepare (point to formation)
		void prepare(); // leader prepare (point to waypoint)
};

#endif /* BOID_H_ */
