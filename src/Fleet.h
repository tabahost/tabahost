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

#include "Boid.h"
#include <list>

class Fleet
{
	private:
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
		int32_t wp[MAX_WAYPOINTS][2]; // waypoints
		list<Boid> boids;

	public:
		Fleet();
		virtual ~Fleet();

		void calcWaypointsTo(int32_t x, int32_t y); // calc a path to xy destination
		void loadWaypoints(); // load waypoints from a file
		void run(u_int8_t formation);
};

#endif /* FLEET_H_ */
