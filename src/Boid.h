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

class Boid;

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

		void push_back(Boid *a){count++; if(last){last->next = new Boidnode(a); last->next->prev = last; last = last->next;} else {first = new Boidnode(a); last = first;}};
		void push_front(Boid *a){count++; if(first){first->prev = new Boidnode(a); first->prev->next = first; first = first->prev;} else {first = new Boidnode(a); last = first;}};
		Boid* front(){if(first) return first->value; else return NULL;};
		Boid* back(){if(last) return last->value; else return NULL;};
		void erase(Boid *a){if(!first) return; for(it = first; it; it = it->next){if(it->value == a){if(first == it) first = it->next; if(last == it) last = it->prev; delete it; count--; break;}}};
		void pop_back(){count--; if(first == last){delete first; first = last = NULL;} else {it = last->prev; delete last; last = it;}};
		void pop_front(){count--; if(first == last){delete first; first = last = NULL;} else {it = first->next; delete first; first = it;}};
		bool empty(){return (count == 0);};
		void clear(){if(first){it = first->next; while(first){delete first; first = it; if(it) it = it->next;}} count = 0;};
};

class Boid
{
	private:
		u_int32_t signature;

	protected:
		// Fica
		u_int8_t plane; // ship plane: KAGA, ENTERPRISE, Etc.
		u_int8_t country; // ship country (1 = red, 3 = gold)
		doublePoint_t Position; // Position
		doublePoint_t Target; // Target Position / Next waypoint
		int32_t radius; // ship radius
		value2_t Vel; // Speed
		value2_t Acel; // Acceleration
		value1_t Yaw; // Heading
		value2_t YawVel; // Yaw change speed
		struct client_s *drone; // Drone that this Boid is associated with
		Boid *leader;
		Boidlist *followers;

		// Revisar
		u_int8_t group; // group index
		u_int8_t type; // ship type: 0 = CV; 1 = CA; 2 = DD


	public:
		static u_int16_t boidCount;

		virtual bool isLegal(const char *);

		Boid();
		virtual ~Boid();

		bool operator==(const Boid &b);
		void removeBoid(Boid *boid);

		double angle(double ang);
		double angleDef(double ang);

		int8_t processDroneBoid(); // boid interface with wb-drone
		void setVelMax(double max);

		void walk(); // make a move forward
		void yaw(Boid *leader); // follower yaw
		void yaw(); // leader yaw
		void retarget(Boid *leader, const double *A); // follower retarget (target leader formation)
		virtual bool retarget(doublePoint_t &wp); // leader retarget (target next waypoint)
		void prepare(Boid *leader, const double *A); // follower prepare (point to formation)
		void prepare(); // leader prepare (point to waypoint)
};

#endif /* BOID_H_ */
