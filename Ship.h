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

class Ship: public Boid
{
	private:
		u_int32_t signature;

	public:
		static u_int16_t shipCount;
		static u_int16_t getShipCount(){return shipCount;};
		static Ship *getShipByNum(u_int8_t num);
		static void resetShips(u_int8_t group);


		virtual bool isLegal(const char *);

		Ship();
		virtual ~Ship();

		// Game Interface
		void processFieldBoid(); // boid interface with wb-field

		// virtuals
		void prepare(const double *A); // follower prepare (point to formation)
		void prepare(); // leader prepare (point to waypoint)
		void loadWaypoints(u_int8_t wpnum);
		bool retarget(coordinates_t &wp);
		int8_t run(); // run frame

		u_int8_t getShipNum();
		void sendCVDots();
		void cvFire(int32_t destx, int32_t desty);
		void addRemoveCVScreen(client_t *client, u_int8_t remove, u_int8_t unk1, u_int8_t cvnum);
		void attackNearestPlane();
		void attackNearestField();
//		void updateFieldPosition(); // if this group is a field (CV), update the field based on group leader position
//		void calcWaypointsTo(int32_t x, int32_t y); // calc a path to xy destination, 1st waypoint is the origin
//		void waypointStart(); // return waypoint counter to start (origin)
//		void waypointNext(); // jump to next waypoint (if reach end, reload waypoints and jump to 2nd waypoint)
//		void waypointChange(int32_t x, int32_t y, u_int16_t wp); // Change waypoint wp to new xy
//		void waypointChange(int32_t x, int32_t y); // Change current waypoint to new xy
};

#endif /* SHIP_H_ */
