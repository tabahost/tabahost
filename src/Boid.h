/*
 * Boid.h
 *
 *  Created on: 03/02/2011
 *      Author: franz
 */

#ifndef BOID_H_
#define BOID_H_

#define CLASSID_BOID 0x87EF9895

#define	MAX_WAYPOINTS		128		// max of waypoint a Boid can handle

#include "shared.h"
using namespace std;

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

class Boidlist;

class Boid
{
	private:
		u_int32_t signature;

	protected:
		// Group variables
		u_int8_t field; // field number of group if any
		bool outofport; // out of port (1st waypoint)
		bool prepared; // fleet is prepared?
		u_int8_t group; // group index
		u_int8_t type; // type: 0 = CV; 1 = CA; 2 = DD
		u_int8_t formation; // which formation the group is using

		// Individual variables
		u_int8_t plane; // ship plane: KAGA, ENTERPRISE, Etc.
		u_int8_t country; // ship country (1 = red, 3 = gold)
		doublePoint_t Position; // Position
		doublePoint_t Target; // Target Position / Next waypoint
		int32_t radius; // ship radius
		value2_t Vel; // Speed
		value2_t Acel; // Acceleration
		value1_t Yaw; // Heading
		value2_t YawVel; // Yaw change speed
		struct client_s *drone; // WBDrone that this Boid is associated with
		u_int8_t zigzag; // next zigzag maneuver
		bool threatened; // fleet is being attacked?

		// Leader exclusive
		u_int8_t wptotal; // total of waypoints
		u_int8_t wpnum; // num of actual waypoint
		doublePoint_t wp[MAX_WAYPOINTS]; // waypoints
		Boidlist *followers;

		// Follower exclusive
		Boid *leader;
		u_int8_t position; // boid position in formation

		// Revisar
		//	u_int8_t	stuck;			// stuck in land
		char logfile[64]; // logfile name
		u_int8_t fleetships[12]; // fleet ships/planes/vehicles, e.g: 73;77;77;77;78
		u_int8_t fleetshipstotal;
		struct field_s *port;


	public:
		static u_int16_t boidCount; // may be obsolete because off Boidlist::count
		static Boidlist boids;
		static void runBoids();

		virtual bool isLegal(const char *);

		Boid();
		Boid(struct client_s *drone, Boid* leaderboid);
		virtual ~Boid();

		// tools
		bool operator==(const Boid &b);
		double angle(double ang);
		double angleDef(double ang);

		// Variable set/access/test
		void setVelMax(double a){Vel.max = a;};
		void setPosition(u_int8_t a){position = a;};
		u_int8_t getPosition(){return position;};
		void setPrepared(bool a){prepared = a;};
		Boid *setLeader(Boid* a){leader = a; return a;};
		bool hasLeader(){return (leader != NULL);};
		struct client_s *setDrone(struct client_s *a){drone = a; return a;};

		// leader functions
		void addFollower(Boid *follower);
		void removeFollower(Boid *follower);
		virtual void loadWaypoints();

		// follower functions
		//void joinLeader(Boid *leader);

		// Game Interface
		int8_t processDroneBoid(); // boid interface with wb-drone

		// Movement functions
		virtual int8_t run();
		void walk(); // make a move forward
		void yaw(Boid *leader); // follower yaw
		void yaw(); // leader yaw
		void retarget(const double *A); // follower retarget (target leader formation)
		virtual bool retarget(doublePoint_t &wp); // leader retarget (target next waypoint)
		void prepare(const double *A); // follower prepare (point to formation)
		void prepare(); // leader prepare (point to waypoint)
};

class Boidnode
{
	public:
		Boid *value;
		Boidnode *prev;
		Boidnode *next;

		Boidnode():value(NULL), prev(NULL), next(NULL){};
		Boidnode(Boid *boid):value(boid), prev(NULL), next(NULL){};
		virtual ~Boidnode(){if(prev && next){prev->next = next;next->prev = prev;} else if(prev && !next){prev->next = NULL;} else if(!prev && next){next->prev = NULL;}};
};

class Boidlist
{
	private:
		u_int32_t count; // counter
		Boidnode *it; // iterator
		Boidnode *first;
		Boidnode *last;
	public:
		Boidlist():count(0),it(NULL),first(NULL),last(NULL){}; // create an empty list
		Boidlist(Boid *boid):count(1),it(NULL),first(new Boidnode(boid)),last(first){}; // create a list with one boid
		virtual ~Boidlist(); // delete list

		void restart(){it = first;};
		Boid *prev(){it = it->prev; return it->value;};
		Boid *next(){it = it->next; return it->value;};
		Boid *current(){return it->value;};

		void push_back(Boid *a){count++; if(last){last->next = new Boidnode(a); last->next->prev = last; last = last->next;} else {first = new Boidnode(a); last = first;}};
		void push_front(Boid *a){count++; if(first){first->prev = new Boidnode(a); first->prev->next = first; first = first->prev;} else {first = new Boidnode(a); last = first;}};
		Boid* front(){if(first) return first->value; else return NULL;};
		Boid* back(){if(last) return last->value; else return NULL;};
		void erase(Boid *a){if(!first) return; for(Boidnode *i = first; it; i = i->next){if(i->value == a){if(first == i) first = i->next; if(last == i) last = i->prev; it = i->prev; delete i; count--; break;}}};
		void erase_del(Boid *a){erase(a); delete a;}; // erase pointer from list and delete the object
		void pop_back(){count--; if(first == last){delete first; first = last = NULL;} else {it = last->prev; delete last; last = it;}};
		void pop_front(){count--; if(first == last){delete first; first = last = NULL;} else {it = first->next; delete first; first = it;}};
		bool empty(){return (count == 0);};
		void clear(){if(first){it = first->next; while(first){delete first; first = it; if(it) it = it->next;}} count = 0;};
};

#endif /* BOID_H_ */
