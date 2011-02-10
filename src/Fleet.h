/*
 * Fleet.h
 *
 *  Created on: 09/02/2011
 *      Author: franz
 */

#ifndef FLEET_H_
#define FLEET_H_

#define CLASSID_FLEET 0xEA466903

#define	MAX_WAYPOINTS		128		// max of waypoint a Fleet can handle

#include <list>
#include "Boid.h"
using namespace std;

class Fleet
{
	private:
		u_int32_t signature;

		u_int8_t id; // Fleet ID
		u_int8_t field; // field number of fleet if any
		u_int8_t country; // fleet country (1 = red, 3 = gold)
		u_int8_t threatened; // fleet is being attacked?
		u_int8_t zigzag; // next zigzag maneuver
		u_int8_t outofport; // out of port (1st waypoint)
		u_int8_t prepared; // fleet is prepared?
		//	u_int8_t	stuck;			// stuck in land
		char logfile[64]; // logfile name
		u_int8_t fleetships[12]; // fleet ships/planes/vehicles, e.g: 73;77;77;77;78
		u_int8_t fleetshipstotal;
		u_int8_t wptotal; // total of waypoints
		u_int8_t wpnum; // num of actual waypoint
		struct field_s *port;
		doublePoint_t wp[MAX_WAYPOINTS]; // waypoints
		list<Boid> boids;

	public:
		static u_int16_t fleetCount;

		virtual bool isLegal(const char *);

		Fleet();
		virtual ~Fleet();

		void resetFleet();
		void updateFieldPosition(); // if this group is a field (CV), update the field based on group leader position
		void calcWaypointsTo(int32_t x, int32_t y); // calc a path to xy destination, 1st waypoint is the origin
		void loadWaypoints(); // load waypoints from a file
		void waypointStart(); // return waypoint counter to start (origin)
		void waypointNext(); // jump to next waypoint (if reach end, reload waypoints and jump to 2nd waypoint)
		void waypointChange(int32_t x, int32_t y, u_int16_t wp); // Change waypoint wp to new xy
		void waypointChange(int32_t x, int32_t y); // Change current waypoint to new xy
		void run(u_int8_t formation); // run frame for all boids in fleet
};

#endif /* FLEET_H_ */
