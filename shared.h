/******************************************************************************
 *  Copyright (C) 2004-2009 Francisco Bischoff
 *  Copyright (C) 2006 MaxMind LLC
 *  Copyright (C) 2000-2003 MySQL AB
 *
 *  This file is part of Tabajara Host Server.
 *
 *  Tabajara Host Server is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tabajara Host Server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Tabajara Host Server.  If not, see <http://www.gnu.org/licenses/agpl.html>.
 *
 *****************************************************************************/

/**
 * @file		/include/shared.h
 * @author		Francisco Bischoff
 * @version	11.2011
 * @date		2011.11.28
 * @brief		common header file
 */

#ifndef SHARED_H_
#define SHARED_H_


// Includes
#include <ctype.h>
#include <errno.h>
#include <GeoIP.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <mysql/mysql.h>
#include <sys/resource.h>
#include <execinfo.h>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <conio.h>
#include <io.h>
#else
#include <dirent.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <unistd.h>
#endif

// Typedefs

#ifdef _WIN32
typedef char int8_t;
typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef int int32_t;
typedef unsigned int u_int32_t;
#endif

// Defines
//#ifndef NULL
//#define NULL ((void *)0)
//#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define VERSION				"b4.20"

#define V_WB2				0
#define V_WB2007			1
#define V_WB2008			2

#define MAX_ENTITIES		256		// absolute limit (must be 2 x MAX_CLIENTS?)
#define MAX_CLIENTS			128		// absolute limit
#define MAX_ARENASLIST		5		// max numer of arenas in arenaslist
#define MAX_UDPDATA			4096	// max udp packet size
#define MAX_RECVDATA		1024	// max data recv each recv()
#define MAX_SENDDATA		2048	// max data can send
#define MAX_SENDBUFFER		8192	// max data can store in send buffer
#define MAX_LOGBUFFER		16384	// max data can store in log buffer
#define MAX_QUERY			4096	// max data can be query
#define MAX_RETRY			5		// max number of retries
#define MAX_PREDICT			6		// max number os history for prediction
#define MAX_TIMEOUT			6000	// 60 seconds of timeout (60x100frames)
#define MAX_PRINTMSG		4096	// max chars per line
#define MAX_RADIOMSG		75		// max of radio msg (64 chars + info)
#define MAX_SCREEN			64		// max aircrafts you can see on screen (32 wb2, 64, wb3)
#define MAX_SCREENBUF		800		// buffer of MAX_SCREEN (19+MAX_SCREEN*21)
#define MAX_MUNTYPE			204 	// = number of munition types
#define MAX_BUILDINGS		600		// 39?
#define MAX_CITYFIELD		17		// max cities linked to a field
#define	MAX_HITBY			64		// max of players can get kill assist
#define MAX_UINT32			(u_int32_t) 0xFFFFFFFF // max value for unsigned int 32 (4294967295UL)
#define MAX_UINT16			(u_int16_t) 0xFFFF // max value for unsigned in 16 (65535)
#define MAX_INT16			(int16_t) 0x7FFF // max value for signed in 16 (32767)
#define MAX_RELATED			7		// max of drones user can mantain relationship
#define	MAX_PLANES			210		// (126 = FHL v1.66) max of plane numbers (last plane number + 1)
#define MAX_CVS				8		// max CV groups
#define MAX_MEDALS			20		// max number of medals player can be award
#define MAX_BOMBS			256		// emulated bombs array
#define MAX_BOMBRADIUS		315		//
#define MIN_BOMBRADIUS		129		//
#define	MAX_MAPCYCLE		16		// max of maps can contain mapcycle
#define MAX_PLACE			32		// max num of plane parts
#define MUNTYPE_BULLET		1		// munition type
#define MUNTYPE_TORPEDO		2		// munition type
#define MUNTYPE_BOMB		3		// munition type
#define MUNTYPE_ROCKET		4		// munition type
#define MUNTYPE_MAX			5		// max
#define MUNCAL_7MM			1		// munition caliber
#define MUNCAL_13MM			2		// munition caliber
#define MUNCAL_20MM			3		// munition caliber
#define MUNCAL_30MM			4		// munition caliber  30-37mm
#define MUNCAL_40MM			5		// munition caliber
#define MUNCAL_88MM			6		// munition caliber  75-88mm
#define VAR_ARCHIVE			1		// set to cause it to be saved to config.cfg
#define VAR_NOSET			2		// block variable from change value
#define VAR_ADMIN			4		// only admins can change this variable
#define MORTAR_BOMB			88		//
#define MORTAR				680.0	// mortar velocity (ft/s) (585, range= 10697; 680, range = 14450)
#define MORTAR_RANGE		14000	// mortar range is 14000ft
#define	GRAVITY				32.0//.8083989501312335958005249343832		// gravity acceleration (ft/s�)
#define MODULUS(a) ((a) > 0 ? (a) : (a) * -1)
#define PLANE_FAU			180	// ME262 48, Predator 180
#define COLLECT_CYCLE		0
#define COLLECT_MAP			1
#define COLLECT_EVENT		2
#define FLAG_ADMIN			1		// check if a client is ADMIN
#define FLAG_OP				2		// check if a client is OP
#define FLAG_EASYMODE		0x01
//#define FLAG_UNK			0x02
#define FLAG_ENEMYNAMES		0x04
#define FLAG_ENEMYPLANES	0x08
#define FLAG_MIDAIRS		0x10
#define FLAG_BLACKOUT		0x20
//#define FLAG_UNK			0x40
#define FLAG_AIRSHOWSMOKE	0x80
#define FLAG_ENABLEOTTOS	0x100
#define FLAG_EASYMODE2		0x200
//#define FLAG_UNK			0x400
//#define FLAG_UNK			0x800
//#define FLAG_UNK			0x1000
//#define FLAG_UNK			0x2000
//#define FLAG_UNK			0x4000
//#define FLAG_UNK			0x8000
#define FLAG_PLANEATRADAR	0x02
#define FLAG_FRIENDLYDOTSTWR	0x04
#define FLAG_ENEMYDOTSTWR	0x08
#define FLAG_FRIENDLYDOTSFLY	0x10
#define FLAG_ENEMYDOTSFLY	0x20
#define FLAG_MAPFLAGSTWR	0x40
#define FLAG_MAPFLAGSFLY	0x80
#define FLAG_MAPFLAGSOWN	0x100
#define FLAG_MAPFLAGSENEMY	0x200

#define EVENT_CREATE		10000	// PLIP, PLCTRID
#define EVENT_LOGIN			10001	// PLIP, PLCTRID
#define EVENT_LOGOUT		10002	// PLIP, PLCTRID
#define EVENT_DISCO			10003	// PLIP, PLCTRID
#define EVENT_KILL			10004	// PLPLANE, PLPLTYPE, PLCTRY, PLORD, VCPLANE, VCPLTYPE, VCCTRY, VCORD
#define EVENT_KILLSTRUCT	10005	// PLCTRY, COUNTRY, STRUCT, FIELD
#define EVENT_HITSTRUCT		10006	// PLCTRY, COUNTRY, STRUCT, FIELD, AMMO
#define EVENT_DROP			10007	// PLPLANE, AMMO
#define EVENT_CAPT			10008	// PLCTRY, FIELD
#define EVENT_RESET			10009	// TERRAIN
#define EVENT_TAKEOFF		10010	// PLPLANE, PLCTRY, PLORD, FIELD
#define EVENT_LAND			10011	// PLPLANE, PLCTRY, FIELD
#define EVENT_EJECT			10012	// PLPLANE, PLCTRY
#define EVENT_CAPTURED		10013	// PLPLANE, PLCTRY
#define EVENT_BAIL			10014	// PLPLANE, PLCTRY
#define EVENT_DITCH			10015	// PLPLANE, PLCTRY
#define EVENT_COLLIDED		10016	// PLPLANE, PLCTRY, VCPLANE, VCCTRY
#define EVENT_RESCUE		10017	// PLPLANE, PLCTRY, VCPLANE
#define EVENT_TANKS			10018	// PLCTRY, FIELD, TOFIELD
#define EVENT_FAU			10019	// PLCTRY, FIELD
#define EVENT_MEDAL			10020	// MDWHAT, MDWHY, MDHM
#define EVENT_THANKS		10021	// EVENT_THANKS - value
#define EVENT_ADM_ALTVAR	20001	// VAR
#define EVENT_ADM_COMMAND	20002	// COMMAND
#define EVENT_ADM_CAPT		20003	// COUNTRY, FIELD, TERRAIN
#define EVENT_ADM_RESET		20004	// TERRAIN
#define EVENT_ADM_TANKS		20005	// FIELD, TOFIELD
#define EVENT_DESC_PLIP		30001	// player IP
#define EVENT_DESC_PLCTRID	30002	// player IP country ID
#define EVENT_DESC_PLCTRY	30003	// player country
#define EVENT_DESC_PLPLANE	30004	// player plane
#define EVENT_DESC_PLPLTYPE	30005	// player plane type
#define EVENT_DESC_PLORD	30006	// player ord
#define EVENT_DESC_VCIP		40001	// victim IP
#define EVENT_DESC_VCCTRY	40002	// victim country
#define EVENT_DESC_VCPLANE	40003	// victim plane
#define EVENT_DESC_VCPLTYPE	40004	// player plane type
#define EVENT_DESC_VCORD	40005	// victim ord
#define EVENT_DESC_STRUCT	50001	// struct ID
#define EVENT_DESC_COUNTRY	50002	// country (struct, field, whatever)
#define EVENT_DESC_FIELD	50003	// field number
#define EVENT_DESC_TOFIELD	50004	// to field number
#define EVENT_DESC_AMMO		50005	// ammo ID
#define EVENT_DESC_VAR		50006	// variable string
#define EVENT_DESC_COMMAND	50007	// command string
#define EVENT_DESC_TERRAIN	50008	// terrain string
#define EVENT_DESC_MDWHAT	60001	// what medal
#define EVENT_DESC_MDWHY	60002	// why earned
#define EVENT_DESC_MDHM		60003	// how much

#define ENDFLIGHT_ZERO			0	// Zero?
#define ENDFLIGHT_LANDED		1	// landed
#define ENDFLIGHT_PILOTKILL		2	// killed in flight
#define ENDFLIGHT_CRASHED		4	// plane crashed
#define ENDFLIGHT_DITCHFAILED	5	// failed to ditch
#define ENDFLIGHT_BAILED		7	// sucessfully bailed
#define ENDFLIGHT_DITCHED		8	// ditched
#define ENDFLIGHT_COLLIDED		9	// collided
#define ENDFLIGHT_PANCAKE		12	// became a pancake

#define SCORE_TAKEOFF		0x0001	// 1 takeoff
#define SCORE_DROPITEM		0x0002	// 2 drop bomb/rocket
#define SCORE_HARDHIT		0x0004	// 4 hit bomb/rocket
#define SCORE_STRUCTURE		0x0008	// 8 destroyed structure
#define SCORE_STRUCTDAMAGE	0x0010	// 16 damaged structure
#define SCORE_LANDED		0x0020	// 32 landed
#define SCORE_DITCHED		0x0040	// 64 ditched
#define SCORE_BAILED		0x0080	// 128 bailed
#define SCORE_CAPTURED		0x0100	// 256 captured
#define SCORE_KILLED		0x0200	// 512 killed
#define SCORE_COLLIDED		0x0400	// 1024 collided
#define SCORE_FIELDCAPT		0x0800	// 2048 field capture
#define SCORE_DISCO			0x1000	// 4096 disconnected

#define VERBOSE_ALWAYS			0		// always prints these messages
#define VERBOSE_ATTENTION		1		// attention messages
#define VERBOSE_WARNING			2		// warning messages
#define VERBOSE_ERROR			3		// error messages
#define VERBOSE_DEBUG_SCORES	4		// debug scores messages
#define VERBOSE_DAMAGE	5		// debug damage messages
#define VERBOSE_DEBUG			6		// debug messages
#define VERBOSE_MAX				6		// max
#define VERBOSE_SMAX			"6"		// max string
#define VERBOSE_ONLINE			-7		// always prints these messages
#define VERBOSE_CHAT			-8		// always prints these messages
#define VERBOSE_KILL			-9		// always prints these messages
#define MAX_LOGFILE				10		// maximum of logfiles

#define FILE_CONSOLE		"./logs/console.log"
#define FILE_ATTENTION		"./logs/attention.log"
#define FILE_WARNING		"./logs/warning.log"
#define FILE_ERROR			"./logs/error.log"
#define FILE_DEBUG_SCORES	"./logs/debug_scores.log"
#define FILE_DAMAGE			"./logs/damage.log"
#define FILE_DEBUG			"./logs/debug.log"
#define FILE_ONLINE			"./logs/online.log"
#define FILE_CHAT			"./logs/chat.log"
#define FILE_KILL			"./logs/kill.log"

#define FILE_ARNASETTINGS	"./arenas/settings.txt"
#define FILE_DRONENICKS		"./players/drones.txt"
#define FILE_DRONENICKS_LOCK "./players/drones.txt.LOCK"
#define FILE_INGAME			"./players/ingame.txt"
#define FILE_OP				"./players/igops.txt"
#define FILE_ADMIN			"./players/igadmins.txt"
#define MISC_SIZE			0x0027C3B0
#define MISC_FILE			"misc.bin"
#define EXEC_SIZE			0x00115000
#define EXEC_FILE			"EXEC"

#define FLIGHT_TIME(xxxxx)	(arena->time - (xxxxx)->dronetimer)

#define CRC_WARBIRDS		0x669ACE70
#define CRC_WBD3D			0x5587583A
#define CRC_WBD3D_MOD		0xB7599D79
#define CRC_MISCBIN			0x9F164A62 // FHL 1.66 (0x9F164A62)
// FHL 1.65 (0x5969E370)
// FHL 1.64r2 (0x6D62CAAC)
// FHL 1.64 (0x860F11D5)
// FHL 1.63 (0x1A566AB0)
// FHL 1.62 (0xBAA80B74)
// FHL 1.61 (0x73109404)
// FHL 1.60 (0x2E2A8C25)
// FHL 1.51 (0xE6BDC24B)
#define BUILD_50CALACK		1
#define	BUILD_20MMACK		2
#define BUILD_40MMACK		3
#define BUILD_88MMFLAK		4
#define BUILD_TOWER			5
#define BUILD_HANGAR		6
#define BUILD_FUEL			7
#define BUILD_AMMO			8
#define BUILD_RADAR			9
#define	BUILD_WARE			10
#define BUILD_RADIOHUT		11
#define BUILD_ANTENNA		12
#define BUILD_CV			13
#define BUILD_DESTROYER		14
#define BUILD_CRUISER		15
#define BUILD_CARGO			16
#define BUILD_SUBMARINE		17
#define BUILD_BRIDGE		18
#define BUILD_SPECIALBUILD	19
#define BUILD_FACTORY		20
#define BUILD_BARRACKS		21
#define BUILD_STATICS		22
#define BUILD_REFINERY		23
#define BUILD_PLANEFACTORY	24
#define BUILD_BUILDING		25
#define BUILD_CRANE			26
#define BUILD_STRATEGIC		27
#define BUILD_ARTILLERY		28
#define BUILD_HUT			29
#define BUILD_TRUCK			30
#define BUILD_TREE			31
#define BUILD_SPAWNPOINT	32
#define BUILD_HOUSE			33
#define BUILD_ROCK			34
#define BUILD_FENCE			35
#define BUILD_MAX			36
#define	FIELD_LITTLE		1
#define	FIELD_MEDIUM		2
#define	FIELD_MAIN			3
#define FIELD_CV			4
#define FIELD_CARGO			5
#define FIELD_DD			6
#define FIELD_SUBMARINE		7
#define FIELD_RADAR			8
#define FIELD_BRIDGE		9
#define FIELD_CITY			10
#define FIELD_PORT			11
#define FIELD_CONVOY		12
#define FIELD_FACTORY		13
#define FIELD_REFINERY		14
#define FIELD_OPENFIELD		15
#define FIELD_WB3POST		16
#define FIELD_WB3VILLAGE	17
#define FIELD_WB3TOWN		18
#define FIELD_WB3PORT		19
#define MAX_FIELDTYPE		20
#define	PLACE_ENGINE1		0x00 // engine
#define	PLACE_ENGINE2		0x01 // drive-train
#define	PLACE_ENGINE3		0x02 // gear box
#define	PLACE_ENGINE4		0x03 // right mid suspension
#define	PLACE_ENGINE1S		0x04 // engine oil
#define	PLACE_ENGINE2S		0x05 //
#define	PLACE_ENGINE3S		0x06 //
#define	PLACE_ENGINE4S		0x07 //
#define	PLACE_ELEVATOR		0x08 // right rear suspension
#define	PLACE_HSTAB			0x09 // left rear suspension
#define	PLACE_RUDDER		0x0A //
#define	PLACE_VSTAB			0x0B // left mid suspension
#define	PLACE_LAILERON		0x0C // left track
#define	PLACE_RAILERON		0x0D // right track
#define	PLACE_LFUEL			0x0E // left fuel tank
#define	PLACE_RFUEL			0x0F // right fuel tank
#define	PLACE_CENTERFUEL	0x10 // center fuel tank
#define	PLACE_REARFUSE		0x11 // front armor
#define	PLACE_CENTERFUSE	0x12 // chassis -> explodes
#define	PLACE_PILOTARMOR	0x13 //
#define	PLACE_PILOT			0x14 // driver -> killed
#define	PLACE_TAILGUN		0x15 // turret
#define	PLACE_NOSEGUN		0x16 // gunner
#define	PLACE_LEFTGUN		0x17 //
#define PLACE_RIGHTGUN		0x18 //
#define	PLACE_TOPGUN		0x19 //
#define	PLACE_BOTTOMGUN		0x1A //
#define	PLACE_LWING			0x1B // left armor
#define	PLACE_RWING			0x1C // right armor
#define	PLACE_RGEAR			0x1D // right front suspension
#define	PLACE_LGEAR			0x1E // left front suspension
#define	PLACE_FLAPS			0x1F // explodes everything!!!!
#define	STATUS_ENGINE1		0x00000001 // engine
#define	STATUS_ENGINE2		0x00000002 // drive-train
#define	STATUS_ENGINE3		0x00000004 // gear box
#define	STATUS_ENGINE4		0x00000008 // right mid suspension
#define	STATUS_ENGINE1S		0x00000010 // engine oil
#define	STATUS_ENGINE2S		0x00000020 //
#define	STATUS_ENGINE3S		0x00000040 //
#define	STATUS_ENGINE4S		0x00000080 //
#define	STATUS_ELEVATOR		0x00000100 // right rear suspension
#define	STATUS_HSTAB		0x00000200 // left rear suspension
#define	STATUS_RUDDER		0x00000400 //
#define	STATUS_VSTAB		0x00000800 // left mid suspension
#define	STATUS_LAILERON		0x00001000 // left track
#define	STATUS_RAILERON		0x00002000 // right track
#define	STATUS_LFUEL		0x00004000 // left fuel tank
#define	STATUS_RFUEL		0x00008000 // right fuel tank
#define	STATUS_CENTERFUEL	0x00010000 // center fuel tank
#define	STATUS_REARFUSE		0x00020000 // front armor
#define	STATUS_CENTERFUSE	0x00040000 // chassis -> explodes
#define	STATUS_PILOTARMOR	0x00080000 //
#define	STATUS_PILOT		0x00100000 // driver -> killed
#define	STATUS_TAILGUN		0x00200000 // turret
#define	STATUS_NOSEGUN		0x00400000 // gunner
#define	STATUS_LEFTGUN		0x00800000 //
#define STATUS_RIGHTGUN		0x01000000 //
#define	STATUS_TOPGUN		0x02000000 //
#define	STATUS_BOTTOMGUN	0x04000000 //
#define	STATUS_LWING		0x08000000 // left armor
#define	STATUS_RWING		0x10000000 // right armor
#define	STATUS_RGEAR		0x20000000 // right front suspension
#define	STATUS_LGEAR		0x40000000 // left front suspension
#define	STATUS_FLAPS		0x80000000 // explodes everything!!!!
#define	STATUS_VITALS		(STATUS_PILOT | STATUS_LWING | STATUS_RWING | STATUS_REARFUSE | STATUS_CENTERFUSE)
//#define CV_SPEED			50	// ft/s
#define DRONE_DBID_BASE		10000000
#define DRONE_FAU_SPEED		500	// Fau Speed
#define DRONE_TANK_SPEED	50	// ft/s
#define DRONE_DIST			150	// Wingman distance
#define DRONE_KATYSALVO		16  // katy salvo
#define	DRONE_FAU			1	// FAU
#define	DRONE_WINGS1		2	// 1st wingman
#define	DRONE_WINGS2		4	// 2nd wingman
#define	DRONE_HMACK			8	// Human Ack
#define	DRONE_HTANK			16	// Human Tank
#define	DRONE_TANK1			32	// Tank
#define	DRONE_TANK2			64	// Tank FIXME, obsolete?
#define	DRONE_AAA			128	// AAA
#define	DRONE_KATY			256	// Katyusha
#define	DRONE_EJECTED		512	// Plane when client ejects
#define DRONE_COMMANDOS		1024	// Commandos
#define DRONE_SHIP			2048	// SHIP (CV, CA, DD)
#define DRONE_PLANE			4096
#define DRONE_DEBUG			8192	//
#define PLANETYPE_FIGHTER	1	//
#define PLANETYPE_BOMBER2	2	//
#define PLANETYPE_BOMBER4	3	//
#define PLANETYPE_VEHICLE	4	//
#define PLANETYPE_CV		5	//
#define PLANETYPE_CA		6	//
#define PLANETYPE_DD		7	//
#define SHIP_KAGA			73
#define SHIP_DD				74
#define SHIP_CA				77
#define SHIP_ENTERPRISE		78
#define COUNTRY_RED			1
#define COUNTRY_GREEN		2
#define COUNTRY_GOLD		3
#define COUNTRY_PURPLE		4
#define RADIO_LIGHTYELLOW	99
#define RADIO_GRAY			100
#define RADIO_RED			101
#define RADIO_GREEN			102
#define RADIO_GOLD			103
#define RADIO_PURPLE		104
#define RADIO_LIGHTGRAY		110
#define RADIO_DARKGREEN		111
#define RADIO_WHITE			200
#define RADIO_BLUE			201
#define RADIO_YELLOW		202
#define RADIO_WEATHER		1000
#define ELO_WINNER			1
#define ELO_LOOSER			2
#define ELO_BOTH			3

#define	SQUADRON_REMOVE		1
#define	SQUADRON_INVITE		2
#define MEDAL_GRANDSTAR		0
#define MEDAL_GRANDCROSS	1
#define MEDAL_ORDERFALCON	2
#define MEDAL_SILVERSTAR	3
#define MEDAL_COMBATCROSS	4
#define MEDAL_COMBATMEDAL	5
#define DEED_STREAKKILLS	1
#define DEED_KILLSTOD		2
#define DEED_CAREERKILLS	3
#define DEED_KILLSSORTIE	4
#define DEED_BSTREAKKILLS	5
#define DEED_STRUCTSTOD		6
#define DEED_CAREERSTRUCTS	7
#define DEED_CAREERFIELDS	8
#define	FORMATION_VWING		1
#define	FORMATION_LABREAST	2
#define	FORMATION_LASTERN	3
#define	FORMATION_ECHELON	4

// Classes

class Boid;
class Ship;

// Structures & Unions

#pragma pack(1)

//typedef struct ship_s
//{
//	u_int8_t	plane;		// ship plane: KAGA, ENTERPRISE, Etc.
//	u_int8_t	type;		// ship type: 0 = CV; 1 = CA; 2 = DD
//	u_int8_t	country;	// ship country (1 = red, 3 = gold)
//	u_int8_t	group;		// group index
//	doublePoint_t Position;	// Position
//	doublePoint_t Target;	// Target Position / Next waypoint
//	int32_t	 radius;		// ship radius
//	value2_t	Vel;			// Speed
//	value2_t	Acel;		// Acceleration
//	value1_t	Yaw;			// Heading
//	value2_t	YawVel;			// Yaw change speed
//	struct client_s	*drone;			// Drone that this boid is associated
//	struct ship_s *next;		// pointer to next ship in chain list
//} ship_t;

//typedef struct cvs_s
//{
//	u_int8_t	id;				// CV ID
//	u_int8_t	field;			// field number of CV
//	u_int8_t	country;	// ship country (1 = red, 3 = gold)
//	u_int8_t	threatened;		// cv is being attacked?
//	u_int8_t	zigzag;			// next zigzag maneuver
//	u_int8_t	outofport;		// out of port (1st waypoint)
//	u_int8_t	prepared;		// cv is prepared?
////	u_int8_t	stuck;			// stuck in land
//	char		logfile[64];	// logfile name
//	u_int8_t	fleetships[12];	// fleet ships, e.g: 73;77;77;77;78
//	u_int8_t	fleetshipstotal;
//	u_int8_t	wptotal;		// total of waypoints
//	u_int8_t	wpnum;			// num of actual waypoint
//	struct field_s *port;
//	int32_t		wp[MAX_WAYPOINTS][2]; // waypoints
//	ship_t		*ships;
//} cvs_t;

//typedef struct cv_s
//{
//	u_int8_t	id;				// CV ID
//	u_int8_t	field;			// field number of CV
//	double		speed;			// speed in ft/s
//	double		xyspeed[2];		// x and y speed for pos update
//	u_int8_t	threatened;		// cv is being attacked?
//	u_int8_t	zigzag;			// next zigzag maneuver
//	u_int32_t	timebase;		// system time when next waypoint will be send
//	u_int8_t	wptotal;		// total of waypoints
//	u_int8_t	outofport;		// out of port (1st waypoint)
////	u_int8_t	stuck;			// stuck in land
//	char		logfile[64];	// logfile name
//	u_int8_t	wpnum;			// num of actual waypoint
//	int32_t		wp[MAX_WAYPOINTS][2]; // waypoints
//} cv_t;

typedef struct bool_s
{
    u_int b : 1;
} bool_t;

typedef struct munition_s
{
    char name[32]; // gun name
    char abbrev[12]; // abbreviated gun name
    int32_t he; // explosive coeficient // -1 = unused weapon
    u_int16_t ap; // penetration coeficient (linear coef)
    int8_t decay; // penetration decay (angular coef)
    u_int8_t type; // type: bullet, rocket, bomb, torp
    u_int8_t caliber; // [7mm, 13mm, 20mm, 30-37mm, 40-57mm, 75-88mm]
} munition_t;

typedef struct building_s
{
    u_int8_t field;
    u_int8_t fieldtype;
    u_int8_t country;
    u_int16_t id;
    u_int8_t type;
    u_int8_t status; // 0-up, 1-destroyed, 2-smoking
    int32_t timer;
    u_int32_t armor;
    u_int8_t infield;
    int32_t posx;
    int32_t posy;
    int32_t posz;
} building_t;

typedef struct rps_s
{
    u_int8_t used; // if this plane has been used in rps some time (rpslag)
    u_int8_t country; // 1 = red, 2 = green, 4 = gold, 8 = purp (can use "OR" |)
    u_int32_t in; // date plane enter in rps (yyyymmdd)
    u_int32_t out; // date plane exit rps (yyyymmdd)
    int8_t pool[19]; // -1 = infinite (S,M,B,CV,Cargo, DD, SUB, Radar, Bridge, City, Port, Convoy, Factory, Refinery, Openfield, Post ,Village, Town)
} rps_t;

typedef struct hitby_s
{
    u_int32_t dbid;
    u_int8_t country;
    char longnick[10];
    u_int32_t squadron;
    u_int16_t plane;
    double damage;
} hitby_t;

typedef struct field_s
{
    u_int8_t number;
    u_int8_t type;
    u_int8_t country;
    int32_t posxyz[3];
    u_int32_t radius;
    u_int8_t abletocapture;
    u_int8_t closed;
    double tonnage;
    u_int8_t vitals;
    u_int8_t paras;
    u_int32_t alert;
    u_int32_t underattack;
    u_int32_t warehouse;
    u_int8_t ctf;
    u_int8_t ctfwho;
    u_int32_t ctfcount;
    struct hitby_s hitby[MAX_HITBY]; // players who hit field
    u_int8_t fleetships[12]; // fleet ships/planes/vehicles, e.g: 73;77;77;77;78
    u_int8_t fleetshipstotal;
    Ship *cv; // linked CV legacy
    struct city_s * city[MAX_CITYFIELD]; // linked city
    double rps[MAX_PLANES];
    u_int8_t rps_custom_rate[MAX_PLANES];
    building_t buildings[MAX_BUILDINGS]; // 1st building is radar
} field_t;

typedef struct city_s
{
    u_int8_t number;
    u_int8_t type;
    char name[32];
    u_int8_t country;
    int32_t posxyz[3];
    u_int8_t closed;
    u_int8_t needtoclose;
    u_int16_t assembling;
    u_int32_t alert;
    field_t *field; // linked field
    building_t buildings[MAX_BUILDINGS]; // 1st building is radar
} city_t;

typedef struct damage_s
{
    char name[33];
    char abbrev[11];
    u_int8_t type;
    int32_t points[32]; // -1 unused part
    int32_t apstop[32];
    int32_t imunity[32];
    int32_t parent[32];
    double positiveG;
    double negativeG;
} damage_t;

typedef struct mapcycle_s
{
    char mapname[MAX_MAPCYCLE]; // name of map
    u_int32_t date; // when map expires (YYYYMMDD)
} mapcycle_t;

typedef struct bomb_s
{
    u_int16_t id;
    u_int8_t type;
    int32_t destx;
    int32_t desty;
    int32_t destz;
    int16_t speed;
    u_int32_t timer;
    struct client_s *from;
} bomb_t;

typedef struct arena_s
{
    u_int32_t time; // current server time
    u_int8_t overload; // overload flag
    u_int32_t sent; // bytes sent
    u_int32_t recv; // bytes recv
    u_int32_t frame; //
    u_int8_t bufferit; // buffer the next sent packet if send() returns EWOULDBLOCK
    u_int16_t year; // arena year
    u_int8_t month; // arena month
    u_int8_t day; // arena day
    u_int8_t hour; // arena hour
    u_int8_t minute; // arena minute
    u_int8_t multiplier; // timer multiplier
    u_int32_t scenario; // scenario start frame
    bool_t thaisent[256]; // array of 256 bits. used in loops to check if already sent data to some THAI group;
    munition_t munition[MAX_MUNTYPE]; // ammo characteristics
    //	cvs_t		cvs[MAX_CVS];	// New CV structures
    rps_t rps[MAX_PLANES]; // planes to auto field update
    mapcycle_t mapcycle[16]; // list of maps to cycle
    int8_t mapnum; // num of current map
    u_int32_t countdown; // numbers of frames to countdown before change map
    char *mapname; // name of current map
    char *name; // Arena name (e.g.: New WBmed Arena)
    char *address; // Arena address (e.g.: wb.chph.ras.ru)
    damage_t planedamage[MAX_PLANES]; // Set plane armor

    struct
    {
        double takeoff;
        double ammotype[MAX_MUNTYPE]; // LoadAmmo():29
        double buildtype[BUILD_MAX]; //
        double planemodel[MAX_PLANES]; // LoadDamageModel():24
        double planeweight[MAX_PLANES]; // LoadDamageModel():25
        double newpilot; //
        double technologylost; //
        double informationlost; //
        double life; //
        double planetransport; //
        double pilottransport; //
        double flighthour; //
    } costs;

    struct
    {
        int32_t points;
        int32_t apstop;
        int32_t imunity;
    } buildarmor[BUILD_MAX];
    double goldindex; // dificulty index for gold
    double redindex; // dificulty index for red
    u_int8_t lastreset; // Last reset 1 = Red, 3 = Gold
    int16_t numplayers; // number of current playing players
    int16_t numdrones; // number of current playing drones
    bomb_t bombs[MAX_BOMBS]; //
    field_t *fields;
    city_t *cities;
} arena_t;

// this structure is just to check if #pragma pack(1) is working

typedef struct checkpack_s
{
    u_int32_t a;
    u_int8_t b;
    u_int32_t c;
    u_int32_t d;
    u_int32_t e;
    u_int8_t f;
    u_int16_t g;
    u_int8_t h;
    u_int8_t i;
    u_int8_t j;
    u_int8_t k;
    u_int8_t l;
    u_int32_t m;
    bool_t n[256];
    int8_t o;
    u_int32_t p;
    char *q;
    char *r;
    char *s;

    struct
    {
        double takeoff;
        double newpilot; //
        double technologylost; //
        double informationlost; //
        double life; //
        double planetransport; //
        double pilottransport; //
        double flighthour; //
    } costs;
    double t;
    double u;
    u_int8_t v;
    int16_t x;
    int16_t z;
} checkpack_t;

typedef struct var_s
{
    char *name; // variable name
    char *string; // string stored in variable
    char *description; // description
    int flags; // variable flags
    u_int8_t modified; // it was recently changed?
    double min; // variable min
    double max; // variable max
    double value; // variable value
    struct var_s *next; // pointer to next variable in chain list
} var_t;

typedef union nick_s
{

    struct
    {
        u_int f : 5;
        u_int e : 5;
        u_int d : 5;
        u_int c : 5;
        u_int b : 5;
        u_int a : 5;
        u_int attr : 2;
    } longnick; // player's nick (6bytes string)

    struct
    {
        u_int32_t value;
    } shortnick; // player's wbnick format (4bytes integer)
} nick_t;

typedef struct score_s
{
    double airscore;
    double groundscore;
    double rescuescore;
    double captscore;
    double costscore;
    double penaltyscore;
} score_t;

typedef struct medals_s
{
    u_int8_t what;
    u_int8_t why;
    u_int32_t when;
    u_int16_t howmuch;
} medals_t;

typedef struct visible_s
{
    struct client_s *client;
    u_int32_t timer;
} visible_t;

typedef struct client_s
{
    u_int8_t inuse; // client exist in world?

    u_int32_t id; // player DB id

    //	THAI vars BEGIN
    u_int8_t thai; // set THAI group
    //	THAI vars END

    //	drone vars BEGIN
    u_int16_t drone; // client is drone? if yes, define its classes
    u_int8_t threatened; // drone are in threat?
    u_int8_t droneformation; // formation wings drones will assume
    int16_t droneformchanged; // wingmen formation changed (angle diff between last pos to new pos)
    double dronedistance; // wingmen formation changed (angle diff between last pos to new pos)
    u_int8_t dronefield; // where drone was dropped
    u_int32_t dronetimer; // how many frames drones will live & time client started to fly (game.c:1643)
    u_int16_t dronelasttarget; // Last target drone (commandos) had aimed
    u_int8_t commandos; // how many commandos client is allowed to drop
    int32_t aim[3]; // where drone is aiming
    //	drone vars END

    int socket; // player's socket
    u_int16_t buf_offset; //
    u_int8_t buffer[MAX_SENDBUFFER]; // Com_Send() buffer
    u_int16_t logbuf_start; //
    u_int16_t logbuf_end; //
    u_int8_t logbuffer[MAX_LOGBUFFER]; // Com_Send() buffer
    u_int32_t sendcount[6][2]; // byte count: 1 second / 5 seconds / 10 seconds / 30 seconds / 60 seconds
    u_int32_t recvcount[6][2]; // byte count: 1 second / 5 seconds / 10 seconds / 30 seconds / 60 seconds
    u_int8_t wouldblock; // wouldblock flag
    char ip[16]; // player's IP

    u_int16_t ctrid; // player's country ID
    u_int32_t hdserial; // player's HD serial
    u_int32_t wbversion; // player's WB Version

    u_int8_t ready; // if client is ready to play
    u_int8_t login; // if != 0, in login process
    u_int32_t frame;
    u_int8_t arenafieldsok; // server already sent all country for all fields to client
    u_int16_t arenabuildsok; // server already sent all builds status to client

    //	u_int8_t	slot;			// player's slot
    int loginkey; // used temporarily to store user/password, so, flag arenalist or enter the game
    char loginuser[33]; //
    u_int16_t packetlen; // lastpacket lenght
    u_int32_t key; // crypt key
    u_int8_t attr; // player's attributes: Admin/OP

    u_int32_t shortnick; // player's wbnick format (4bytes integer)
    char longnick[11]; // player's nick (6bytes string + 4 for (TK))

    u_int8_t chute; // player ejected

    u_int16_t warp; // DEBUG: simulate warps
    u_int16_t warptimer; // DEBUG: simulate warps
    u_int8_t predict; //
    int16_t offset; // offset
    int32_t posxy[2][MAX_PREDICT]; // plane's position (x, y)
    int32_t posalt[MAX_PREDICT]; // plane's altitude
    int16_t angles[3][MAX_PREDICT]; // plane's angles (pitch, roll, yaw)
    int16_t accelxyz[3][MAX_PREDICT]; // plane's acceleration (x, y, z)
    int16_t aspeeds[3][MAX_PREDICT]; // plane's angular speeds (pitch, roll, yaw)
    int16_t speedxyz[3][MAX_PREDICT]; // plane's speed (x, y, z)

    u_int8_t ackstarcount; // count to avoid recalc every time
    u_int8_t ackstar; // flag to check ackstar rules (below 1000ft, in friendlyfield range, with wingmen, buffer, have otto disabled).
    u_int8_t contrail; // plane is above contrail->value
    u_int8_t damaged; // is plane was damaged by attacker
    u_int32_t status_damage; // plane's status (gun, fuselage, etc)
    u_int32_t status_status; // plane's status (engine, baydoor, smoke, etc)
    int32_t reldist; // relative distance between 2 players
    u_int8_t mapdots;
    visible_t visible[MAX_SCREEN]; // array with visible entities

    u_int8_t field; // which field player is
    u_int8_t hq; // player moved to HQ
    u_int16_t inflight; // if client is in flight (flight ID)
    u_int16_t plane; // selected plane
    u_int32_t fuel; // %fuel
    u_int32_t conv; // convergency
    u_int8_t country; // player's country
    u_int32_t countrytime; // time untill can change country
    u_int8_t gunstat; // enable gunstats printing
    u_int8_t ord; // ordinance
    u_int8_t rain; // enable or disable rain
    u_int8_t easymode; // easy mode
    int32_t obradar; // plane have onboard radar (value == range)
    u_int8_t mortars; // how many mortars user have to fire from plane

    struct client_s *invite; // actual invited squadron
    u_int32_t squadron; // client's squadron owner's ID
    u_int16_t squad_flag; // client's squadron flags
    u_int8_t hls; // already requested hls in this session

    u_int8_t wings; // wings mode enabled

    score_t score; // score values
    u_int16_t flypenalty; // time penalty if killed/crashed in flypenalty->value time
    u_int8_t flypenaltyfield; // which field client was penalized
    u_int8_t hmackgear; //
    u_int16_t hmackpenalty; // time penalty if killed in flypenalty->value time
    u_int8_t hmackpenaltyfield; // which field client was penalized

    u_int16_t hits[6]; // hits scored [7mm, 13mm, 20mm, 30-37mm, 40-57mm, 75-88mm]
    u_int16_t hitstaken[6]; // hits taken
    u_int8_t hitsstat[6]; // hits scored each 3 seconds
    u_int8_t hitstakenstat[6]; // hits taken each 3 seconds
    u_int8_t killssortie; // kills in this sortie
    u_int16_t killstod; // kills in this TOD
    u_int16_t structstod; // structures in this TOD
    double lastscore; // score in last flight
    double streakscore; // score in last streak
    u_int8_t nummedals; // num of medals received // TODO: Misc: unused????
    int16_t rank; // Elo rating
    u_int8_t ranking; // pilot ranking

    u_int8_t tklimit; // TODO: FIXME: add limit for friendly buildings/planes kills till be kicked
    u_int8_t tkstatus; //

    struct hitby_s hitby[MAX_HITBY]; // players who hit client

    u_int8_t lograwdata; // flag to log raw data
    char logfile[64]; // logfile name
    char file[128]; // filenames (motd, score, etc)
    char skin[64]; // client skin
    u_int8_t fileframe; // if filesize bigger than a value, clip it in fileframe packets

    int8_t lives; // how many lives player have (-1 = infinite)
    damage_t armor; // resistance of each structure in a plane
    u_int32_t fueltimer; // fuel timer until explode
    u_int32_t oiltimer; // oil lines timer untill explode // in disuse
    u_int32_t oildamaged; // oil lines ID to explode // in disuse

    int8_t cancollide; // set if player can collide with other player

    u_int8_t radio[4]; // frequency of radio 1-4
    u_int32_t lastmsgtime; // when last msg
    char lastmsg[MAX_RADIOMSG]; // last msg sent, to block flood

    struct client_s *view; // view what other player is doing
    struct client_s *shanghai; // what client is shanghai
    struct client_s *attached; // who client is attached
    Ship *deck; // client is at CV Deck

    struct client_s *gunnerview; // which client player is viewing
    struct client_s * gunners[14];
    struct client_s * related[MAX_RELATED]; // 0 reserved for fau or hmack

    u_int32_t mapper; // mapper mode activated
    u_int16_t mapperx; // X index
    u_int16_t mappery; // Y index

    u_int16_t msgtimer; // msgtimer (friendly hit)
    u_int32_t lastsql; // last time client made a SQL request;
    u_int32_t timer; // player timer
    u_int32_t clienttimer; // player clienttimer
    u_int32_t basetimer; // player basetimer
    u_int32_t basediff; // player basediff
    u_int32_t pingtest; // pingtest timer
    u_int8_t connection; // Connection status. 0 = stable, 1 = fair, 2 = unstable, 3 = poor
    u_int32_t conn_sum; // Connection sum
    u_int16_t conn_count; // Connection counter
    double conn_lastavg; // Connection last average
    double conn_curravg; // Connection current average
    u_int32_t postimer; // last position update arena's time
    u_int32_t awaytimer; // away timer
    u_int32_t timeout; // actual timeout
    u_int8_t bugged; // client had made a fatal bug and will be kicked
    u_int8_t disconnect; // client request disconnect

} client_t;

typedef struct playerslist_s
{
    u_int32_t nick;
    u_int8_t country;
} playerslist_t;

typedef struct arenaslist_s
{
    char mapname[20];
    char hostdomain[40];
    char hostname[50];
    u_int32_t numplayers;
    u_int32_t maxclients;
    u_int8_t array[1024];
    u_int32_t logintime;
    u_int32_t time;
} arenaslist_t;

typedef struct initflight_s // 02 01
{
    u_int16_t packetid;
    u_int32_t fuel;
    u_int32_t conv; // x3
    u_int32_t alt;
    u_int32_t runway;
    u_int32_t country;
    u_int16_t field; // -1
    u_int16_t unknown1;

    u_int16_t bulletradius1;
    u_int16_t bulletradius2;
    u_int16_t gunrad;
    u_int16_t bulletradius3;
    u_int16_t unknown6;
    u_int16_t unknown7;
    u_int16_t unknown8;

    u_int8_t unknown9;
    u_int8_t unknown10;
    u_int32_t unknown11;
    u_int32_t rules;
    u_int32_t attachedpos;
    u_int32_t attached;
    u_int16_t plane;
    u_int16_t unknown12;

    u_int32_t unknown13;
    u_int8_t numofarrays; // how to use this?????
} initflight_t;

typedef struct wb3initflight_s // 02 01
{
    u_int16_t packetid;
    u_int16_t error; // init flight error
    u_int32_t country;
    u_int16_t field; // -1
    u_int16_t unknown1;

    u_int16_t bulletradius1;
    u_int16_t bulletradius2;
    u_int16_t gunrad;
    u_int16_t bulletradius3;
    u_int16_t unknown2;
    u_int16_t unknown3;
    u_int16_t unknown4;

    u_int8_t unknown5;
    u_int8_t unknown6;
    u_int32_t unknown7;
    u_int32_t rules;
    u_int32_t unknown8;
    u_int32_t attachedpos;
    u_int32_t unknown10; // same value from wb3requestfly_t unk4
    u_int32_t unknown11; // same value from wb3requestfly_t unk5
    u_int32_t alt;
    u_int32_t unknown09; // not confirmed
    u_int16_t attached; // not confirmed
    u_int16_t plane;

    u_int32_t unknown12;
    u_int8_t numofarrays;

    //	u_int32_t	attachedpos;
    //	u_int32_t	attached;

} wb3initflight_t;

typedef struct initflight2_s // 02 01
{
    u_int8_t unknown14;
    u_int32_t unknown15;
    u_int8_t ord;
    u_int32_t unknown16;
    u_int16_t unknown17;
} initflight2_t;

typedef struct endflight_s // 02 02
{
    u_int16_t packetid;
    u_int16_t type;
    u_int16_t field;
    u_int16_t gunused;
    u_int16_t gunhits;
} endflight_t;

typedef struct move_s // 02 03
{
    u_int16_t packetid;
    u_int32_t country;
    u_int16_t unknown1;
    u_int8_t options;
    u_int8_t field;
} move_t;

typedef struct lastconfig_s // 02 04
{
    u_int16_t packetid; //
    u_int32_t plane;
    u_int32_t country;
    u_int32_t nick;
    u_int32_t unknown4;
    u_int32_t fuel;
    u_int32_t unknown6;
    u_int32_t conv;
    u_int32_t ord;
} lastconfig_t;

typedef struct wb3lastconfig_s // 02 04
{
    u_int16_t packetid; //
    u_int32_t unk1;
    u_int32_t plane; // sure is plane?
    u_int32_t unk2;
    u_int32_t unk3;
    u_int32_t unk4;
    u_int32_t country;
    u_int32_t nick;
    u_int32_t unk5;
} wb3lastconfig_t;

typedef struct reqbomberlist_s // 02 04
{
    u_int16_t packetid;
    u_int8_t unknown1;
    u_int8_t numofnicks;
    u_int32_t nick;
} reqbomberlist_t;

typedef struct squadmembers1_s // 02 09
{
    u_int16_t packetid;
    u_int8_t unknown1;
    u_int16_t nicks;
} squadmembers1_t;

typedef struct squadmembers2_s // 02 09
{
    u_int32_t nick;
    u_int32_t attr;
} squadmembers2_t;

typedef struct squadlookup_s // 02 0C
{
    u_int16_t packetid;
    u_int16_t unknown1;
    u_int32_t nick;
} squadlookup_t;

typedef struct requestmedals_s // 02 0D
{
    u_int16_t packetid;
    u_int32_t nick;
} requestmedals_t;

typedef struct clientmedals_s // 02 0E
{
    u_int16_t packetid;
    u_int8_t amount;
} clientmedals_t;

typedef struct clientmedals2_s // 02 0E
{
    u_int8_t medal;
    u_int8_t deed;
    u_int32_t time;
    u_int16_t num;
} clientmedals2_t;

typedef struct clientmedals3_s // 02 0E
{
    u_int16_t unknown1;
    u_int8_t msgsize;
    u_int8_t ranking; // message
} clientmedals3_t;

typedef struct gunnerlist_s // 02 11 and 02 13
{
    u_int16_t packetid;
    u_int32_t gunners[14]; // first 7 are requested players, and 7 last are accepted
    u_int16_t unknown1; // 02 13
} gunnerlist_t;

typedef struct reqgunnerlist_s // 02 12
{
    u_int16_t packetid;
    u_int16_t unknown1;
    u_int32_t nick;
} reqgunnerlist_t;

typedef struct gunnerstatus_s // 02 14
{
    u_int16_t packetid;
    u_int32_t shortnick;
    u_int16_t status; // ??
} gunnerstatus_t;

typedef struct plane_s // 02 15
{
    u_int16_t packetid;
    u_int16_t plane;
    u_int16_t conv;
    u_int16_t fuel;
    u_int16_t ord;
} plane_t;

typedef struct hls_s // 02 19
{
    u_int16_t packetid;
    u_int8_t num;
    u_int32_t nick;
} hls_t;

typedef struct currentview_s // 02 1A
{
    u_int16_t packetid;
    u_int8_t unknown1;
    u_int8_t unknown2;
    u_int8_t view;
    u_int8_t unknown3;
} currentview_t;

typedef struct wb3requestfly_s // 02 1C
{
    u_int16_t packetid;
    u_int32_t plane;
    u_int32_t country;
    int32_t field;
    u_int32_t unk1;
    int32_t unk2;
    u_int32_t ord;
    u_int32_t spawnpoint;
    u_int32_t unk4;
    u_int32_t unk5;
} wb3requestfly_t;

typedef struct wb3startack_s // 02 1F
{
    u_int16_t packetid;

    u_int16_t unk1;
    u_int32_t country;
    u_int16_t field; // field - 1
    u_int16_t unk2;

    u_int16_t bulletradius1;
    u_int16_t bulletradius2;
    u_int16_t gunrad;
    u_int16_t bulletradius3;
    u_int16_t unk3;
    u_int16_t unk4;
    u_int16_t unk5;

    u_int32_t unk6;
    u_int32_t rules;

    u_int32_t unk7;
    u_int32_t unk8;
    u_int32_t unk9;
    u_int32_t unk10;
    u_int32_t unk11;
    u_int16_t unk12;
    u_int16_t plane;

    u_int32_t unk13;
    u_int8_t numofarrays;

    u_int8_t unk14;
    u_int32_t unk15;
    u_int8_t ord;
    u_int32_t unk16;
    u_int16_t unk17;

    u_int32_t unk18;
    u_int32_t unk19;

    int32_t posx;
    int32_t posy;
    u_int32_t posz;

    u_int32_t unk23;
    u_int32_t unk24;
    u_int32_t unk25;

    u_int32_t unk26;
    u_int32_t unk27;
    u_int32_t unk28;
} wb3startack_t;

typedef struct wb3requestmannedack_s // 02 20
{
    u_int16_t packetid;
    u_int32_t plane;
    u_int32_t country;
    u_int32_t field;
    u_int32_t unk1;
    u_int32_t unk2; // timer?
    u_int32_t unk3;
    u_int32_t unk4;
    u_int32_t ackid;
} wb3requestmannedack_t;

typedef struct arenarules_s // 03 00
{
    u_int16_t packetid;
    u_int32_t radaralt;
    u_int32_t mapflags;
    u_int32_t planerangelimit;
    u_int32_t enemyidlim;
    u_int32_t friendlyidlim;
    u_int32_t ammomult;
    u_int32_t xwindvelocity;
    u_int32_t ywindvelocity;
    u_int32_t zwindvelocity;
    u_int16_t maxpilotg;
    u_int8_t structlim;
    u_int8_t unknown1;
    u_int16_t unknown2;
    u_int8_t altv;
    u_int32_t fueldiv;
    u_int32_t flakmax;
    u_int32_t radarrange0;
    u_int32_t radarrange1;
    u_int32_t radarrange2;
    u_int32_t radarrange3;
    u_int32_t radarrange4;
    u_int8_t unknown3; // 0x55
    u_int8_t unknown4; // 0x30
    u_int8_t arenaflags2; // 0x00
    u_int8_t arenaflags1; // 0x03
    u_int8_t unknown7; // 0x10
    u_int8_t unknown8; // 0x01
    u_int8_t unknown9; // 0x07
} arenarules_t;

typedef struct wb3arenarules_s // 03 00
{
    u_int16_t packetid;
    u_int32_t radaralt;
    u_int32_t mapflags;
    u_int32_t planerangelimit;
    u_int32_t enemyidlim;
    u_int32_t friendlyidlim;
    u_int32_t ammomult;
    u_int16_t maxpilotg;
    u_int32_t xwindvelocity;
    u_int32_t ywindvelocity;
    u_int32_t zwindvelocity;
    u_int8_t structlim;
    u_int8_t unknown1;
    u_int8_t unknown2;
    u_int8_t ackmaxtrav; // ?
    u_int8_t altv;
    u_int32_t fueldiv;
    u_int32_t flakmax;
    u_int32_t radarrange0;
    u_int32_t radarrange1;
    u_int32_t radarrange2;
    u_int32_t radarrange3;
    u_int32_t radarrange4;
    u_int8_t ackshrinkco;
    u_int8_t ackgrowco;
    u_int8_t arenaflags2;
    u_int8_t arenaflags1;
    u_int8_t unknown5; // 0x10
    u_int32_t clickrangelim;
    u_int8_t unknown6; // 0x05
} wb3arenarules_t;

typedef struct arenatime_s // 03 01
{
    u_int16_t packetid;
    u_int8_t hour;
    u_int8_t minute;
    u_int8_t mult;
} arenatime_t;

typedef struct cvpos_s // 03 02
{
    u_int16_t packetid;
    u_int16_t number;
    u_int32_t posx;
    u_int32_t posy;
    u_int32_t unknown1;
} cvpos_t;

typedef struct cvroute_s // 03 03
{
    u_int16_t packetid;
    u_int16_t number;
    u_int32_t basetime;
    u_int32_t triptime;
    u_int32_t destx;
    u_int32_t desty;
    u_int32_t unknown3;
    u_int32_t posx;
    u_int32_t posy;
    u_int32_t unknown4;
} cvroute_t;

typedef struct sinkboat_s // 03 04
{
    u_int16_t packetid;
    u_int16_t task;
    u_int16_t boat;
} sinkboat_t;

typedef struct captfield_s // 03 06
{
    u_int16_t packetid;
    u_int16_t field;
    u_int32_t country;
} captfield_t;

typedef struct buildstatus_s // 03 07
{
    u_int16_t packetid;
    u_int16_t building;
    u_int8_t status;
    u_int8_t country;
} buildstatus_t;

typedef struct pingtest_s // 03 09
{
    u_int16_t packetid;
    u_int16_t unk1;
    u_int16_t frame;
} pingtest_t;

typedef struct wb3gruntconfig_s // 03 0D
{
    u_int16_t packetid;
    u_int32_t gruntsmaxd;
    u_int8_t shootgrunts;
    u_int8_t capturegrunts;
} wb3gruntconfig_t;

typedef struct wb3randomseed_s // 03 0E
{
    u_int16_t packetid;
    u_int32_t seed;
} wb3randomseed_t;

typedef struct wb3arenaflags3_s // 03 0F
{
    u_int16_t packetid;
    u_int32_t flags;
} wb3arenaflags3_t;

typedef struct wb3configfm_s // 03 10
{
    u_int16_t packetid;
    u_int32_t unk1;
    u_int32_t unk2;
    u_int32_t unk3;
    u_int32_t unk4;
    u_int32_t unk5;
    u_int32_t unk6;
    u_int32_t unk7;
    u_int32_t unk8;
    u_int32_t unk9;
    u_int32_t unk10;
    u_int32_t unk11;
    u_int32_t unk12;
} wb3configfm_t;

typedef struct wb3arenaconfig2_s // 03 12
{
    u_int16_t packetid;
    u_int32_t arnaflags3;
    u_int32_t wingstrikerng;
} wb3arenaconfig2_t;

typedef struct idle_s // 09 04
{
    u_int16_t packetid;
    u_int32_t timer;
} idle_t;

typedef struct thaiwatchdog_s // 09 04
{
    u_int16_t packetid;
    u_int8_t group;
    u_int8_t id2;
    u_int8_t id3;
    u_int8_t id4;
} thaiwatchdog_t;

typedef struct requestgunner_s // 09 05
{
    u_int16_t packetid;
    u_int32_t nick;
    u_int16_t pos;
} requestgunner_t;

typedef struct acceptgunner_s // 09 06
{
    u_int16_t packetid;
    u_int32_t nick;
    u_int16_t unknown1;
} acceptgunner_t;

typedef struct switchottopos_s // 09 07
{
    u_int16_t packetid;
    int16_t pos;
    u_int16_t unknown1; // convergency??
} switchottopos_t;

typedef struct reloadweapon_s // 09 09
{
    u_int16_t packetid;
    u_int16_t unknown1;
    u_int16_t unknown2;
    u_int16_t amount;
} reloadweapon_t;

typedef struct sendrps_s // 09 0A
{
    u_int16_t packetid;
    u_int8_t amount;
    u_int8_t plane; // first of array
} sendrps_t;

typedef struct shanghai_s // 09 0C
{
    u_int16_t packetid;
    u_int32_t nick;
    u_int16_t unknown1;
} shanghai_t;

typedef struct hdserial_s // 09 0E
{
    u_int16_t packetid;
    u_int32_t serial;
} hdserial_t;

typedef struct copyrighta_s // 0C 00
{
    u_int16_t packetid; // 0c 00
    u_int32_t gameversion; // 00 04 3B 55
    u_int8_t nicksize;
    u_int8_t nick; // &(variable->nick)
} copyrighta_t;

typedef struct copyrightb_s // 0C 00
{
    u_int32_t cryptkey;
    u_int8_t mapnamesize;
    u_int8_t mapname;
} copyrightb_t;

typedef struct planeposition_s // 0E 00
{
    u_int16_t packetid;
    int32_t posx;
    int32_t posy;
    u_int32_t alt;
    int16_t xspeed;
    int16_t yspeed;
    int16_t climbspeed;
    int16_t sideaccel;
    int16_t forwardaccel;
    int16_t climbaccel;
    int16_t pitchangle;
    int16_t rollangle;
    int16_t yawangle;
    int16_t pitchangspeed;
    int16_t rollangspeed;
    int16_t yawangspeed;
    u_int32_t timer;
    u_int16_t radar;
    u_int16_t plane;
} planeposition_t;

typedef struct wb3planeposition_s // 0E 00
{
    u_int16_t packetid;
    u_int32_t timer;
    u_int16_t radar;
    u_int16_t plane;
    int32_t posx;
    int32_t posy;
    u_int32_t alt;
    int16_t xspeed;
    int16_t yspeed;
    int16_t climbspeed;
    int16_t sideaccel;
    int16_t forwardaccel;
    int16_t climbaccel;
    int16_t pitchangle;
    int16_t rollangle;
    int16_t yawangle;
    int16_t pitchangspeed;
    int16_t rollangspeed;
    int16_t yawangspeed;
} wb3planeposition_t;

typedef struct planestatus1_s // 0E 01 // 0E 02
{
    u_int16_t packetid;
    u_int32_t status_damage;
    u_int32_t status_status;
} planestatus1_t;

typedef struct planeposition2_s // 0E 04
{
    u_int16_t packetid;
    u_int32_t timer;
    u_int16_t radar;
    u_int16_t plane;
} planeposition2_t;

typedef struct chutepos_s // 0E 05
{
    u_int16_t packetid;
    u_int32_t timer;
    int32_t posx;
    int32_t posy;
    int32_t alt;
    int32_t xspeed;
    int32_t yspeed;
    int32_t zspeed;
} chutepos_t;

typedef struct wb3gunnerupdate_s // 0E 07
{
    u_int16_t packetid;
    u_int32_t timer;
    u_int16_t gun;
    u_int8_t pitch;
    u_int8_t yaw;
} wb3gunnerupdate_t;

typedef struct wb3nwattachslot_s // 0E 0B
{
    u_int16_t packetid;
    u_int32_t slot; // ???
} wb3nwattachslot_t;

typedef struct wb3firesupression_s // 0E 0E
{
    u_int16_t packetid;
    u_int32_t supress;
} wb3firesupression_t;

typedef struct wb3externalammocnt_s // 0E 0F
{
    u_int16_t packetid;
    u_int32_t unk1;
    u_int32_t unk2;
} wb3externalammocnt_t;

typedef struct ottoparams_s // 0F 00
{
    u_int16_t packetid; //
    u_int8_t accuracy; //OTTO_ACCURACY
    u_int16_t unknown1; // ???
    u_int16_t range; // OTTO_RANGE
    u_int16_t burston; // OTTO_BURSTON_BASE x 100
    u_int16_t burstonmax; // OTTO_BURSTON_MAX x 100
    u_int16_t burstoff; // OTTO_BURSTOFF_BASE x 100
    u_int16_t retarget; // OTTO_RETARGET_TIME x 100
    u_int16_t adjust; // OTTO_ADJUST
    u_int32_t override; // OTTO_OVERRIDES
} ottoparams_t;

typedef struct ottoparams2_s // 0F 03
{
    u_int16_t packetid; //
    u_int16_t unk1; //
    u_int16_t ottoacquirerange; // OTTO_ACQUIRE_RANGE?
} ottoparams2_t;

typedef struct ottofiring_s // 0F 01
{
    u_int16_t packetid; //

    u_int8_t item;
    u_int16_t id;
    int32_t posx;
    int32_t posy;
    int32_t alt;

    int16_t xspeed;
    int16_t yspeed;
    int16_t zspeed;

    u_int32_t unknown6;
    u_int32_t shortnick;
} ottofiring_t;

typedef struct radiomessage_s // 12 00
{
    u_int16_t packetid; //
    u_int32_t msgto; // radio channel
    u_int32_t msgfrom; // who is talking
    u_int8_t msgsize;
    char message; // contain the first char of msg, must be used as &(variable->message)
} radiomessage_t;

typedef struct setradiochannel_s // 12 01
{
    u_int16_t packetid;
    u_int16_t radionum;
    u_int16_t channel;
} setradiochannel_t;

typedef struct wb3dotcmd_s // 12 05
{
    u_int16_t packetid; //
    u_int32_t unk1; //
    u_int8_t msgsize;
    char message; // contain the first char of msg, must be used as &(variable->message)
} wb3dotcmd_t;

typedef struct hostvar_s // 0x1412
{
    u_int16_t packetid; //
    u_int32_t unk1; //
    u_int8_t size;
    char var; // contain the first char of msg, must be used as &(variable->var)
} hostvar_t;

typedef struct checkexecutables_s // 18 00
{
    u_int16_t packetid;
    u_int32_t exemisc;
    u_int32_t unknown1;
    u_int32_t filesize;
    u_int8_t msgsize;
    u_int8_t msg;
} checkexecutables_t;

typedef struct checkclientfiles_s // 18 01
{
    u_int16_t packetid;
    u_int32_t exemiscview;
    u_int32_t unknown1;
    u_int32_t filesize;
    u_int32_t unknown2;
    u_int32_t crc;
    u_int8_t msgsize;
    char msg;
} checkclientfiles_t;

typedef struct insertnick_s // 1A 01
{
    u_int16_t packetid;
    u_int8_t allow;
    u_int8_t msgsize;
    char msg;
} insertnick_t;

typedef struct rocketbomb_s // 19 00
{
    u_int16_t packetid;
    u_int8_t item;
    u_int16_t id;
    int32_t posx;
    int32_t posy;
    int32_t alt;
    int16_t xspeed;
    int16_t yspeed;
    int16_t zspeed;
    u_int32_t unknown1;
    u_int32_t shortnick;
    u_int32_t fuse; // only in 0x1918 packet
} rocketbomb_t;

typedef struct flakhit_s // 19 02
{
    u_int16_t packetid;
    u_int32_t victim;
    u_int8_t hits;
    u_int8_t type;
    u_int8_t unknown1;
    u_int16_t speed;
} flakhit_t;

typedef struct hitstructure_s // 1B 03
{
    u_int16_t packetid;
    u_int16_t build;
    u_int8_t hits;
    u_int8_t munition;
    u_int8_t unknown1;
    u_int16_t bulletspeed;
} hitstructure_t;

typedef struct hardhitplane_s // 1B 04
{
    u_int16_t packetid;
    u_int8_t munition;
    u_int32_t victim;
    u_int16_t unknown1;
    int8_t place;
    u_int16_t unknown2;
    u_int16_t unknown3;
    u_int16_t bulletspeed;
} hardhitplane_t;

typedef struct hardhitstructure_s // 1B 05
{
    u_int16_t packetid;
    u_int8_t munition;
    u_int16_t build;
    u_int16_t unknown1;
    u_int16_t unknown2;
    u_int16_t munid;
    u_int16_t speed;
} hardhitstructure_t;

typedef struct weapondestroy_s // 1B 06
{
    u_int16_t packetid;
    u_int16_t itemid;
} weapondestroy_t;

typedef struct pings_s // 1B 07
{
    u_int16_t packetid;
    u_int8_t type;
    u_int8_t hits;
} pings_t;

typedef struct firemg_s // 1B 09
{
    u_int16_t packetid;
    u_int32_t sumguntype;
    u_int8_t slot;
} firemg_t;

typedef struct hitplane_s // 1B 0B
{
    u_int16_t packetid;
    u_int32_t victim;
    u_int8_t hits;
    u_int8_t type;
    int8_t place[5];
    u_int16_t bulletspeed;
} hitplane_t;

typedef struct wb3tonnage_s // 1B 13
{
    u_int16_t packetid;
    u_int8_t ammo;
    u_int16_t field;
    u_int16_t distance;
} wb3tonnage_t;

typedef struct wb3firemg_s // 1B 15
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int32_t sumguntype;
} wb3firemg_t;

typedef struct wb3fuelconsumed_s // 1B 16
{
    u_int16_t packetid;
    u_int32_t amount;
} wb3fuelconsumed_t;

typedef struct wb3delayedfuse_s //1B 17
{
    u_int16_t packetid;
    u_int8_t item;
    u_int16_t id;
    int32_t posx;
    int32_t posy;
    int32_t alt;
    int16_t xspeed;
    int16_t yspeed;
    int16_t zspeed;
    u_int32_t unknown1;
    u_int32_t shortnick;
    u_int32_t fusealt;
} wb3delayedfuse_t;

typedef struct sendfile3_s // 1F 01
{
    u_int16_t packetid; //
    u_int8_t transaction;
    u_int8_t unknown1;
    u_int32_t time;
    u_int32_t filesize;
} sendfile3_t;

typedef struct sendfile1_s // 1F 03
{
    u_int16_t packetid; //
    u_int8_t transaction;
    u_int8_t msgsize;
    u_int8_t msg;
    u_int32_t unknown2;
    u_int32_t unknown3;
} sendfile1_t;

typedef struct sendfile5_s // 1F 05
{
    u_int16_t packetid; //
    u_int8_t transaction;
    u_int8_t unknown1;
    u_int32_t frame;
    u_int16_t msgsize;
    u_int8_t msg;
} sendfile5_t;

typedef struct sendfile6_s // 1F 06
{
    u_int16_t packetid; //
    u_int8_t transaction;
    u_int32_t frame;
} sendfile6_t;

typedef struct sendfile7_s // 1F 07
{
    u_int16_t packetid; //
    u_int16_t unknown1;
    u_int8_t msgsize;
    u_int8_t msg;
} sendfile7_t;

typedef struct addplane_s // 21 00
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int32_t shortnick;
    u_int32_t country;
    u_int16_t unknown1; //?? 00, 01 (in shanghai?), 02, 03
    u_int16_t plane;
    u_int16_t remove; // set 0D to remove player from screen
    u_int16_t unknown2;
} addplane_t;

typedef struct updateplane_s // 21 01
{
    u_int16_t packetid;
    u_int32_t timer; // ???
    int32_t posx;
    int32_t posy;
    int32_t alt;
    u_int8_t num;
} updateplane_t;

typedef struct updateplane2_s // 21 01
{
    int16_t timeoffset; // plane offset (difference in miliseconds from last position packet)
    u_int8_t slot; // plane slot
    int16_t relposx; // relative pos to updateplane_t.posx
    int16_t relposy; // relative pos to updateplane_t.posy
    int16_t relalt; // relative pos to updateplane_t.alt
    int8_t pitch; // pitch angle
    int8_t xaccel; // acceleration in X axis
    int16_t prxspeed; // pitch angular speed (7bits) AND X axis speed (9bits)
    //int8_t		xspeed;
    int8_t roll; // roll angle
    int8_t yaccel; // acceleration in Y axis
    int16_t bryspeed; // roll angular speed (7bits) AND Y axis speed (9bits)
    //int8_t		yspeed;
    int8_t yaw; // yaw angle
    int8_t zaccel; // acceleration in Z axis
    int16_t yrzspeed; // yaw angular speed (7bits) AND Z axis speed (9bits)
    //int8_t		zspeed;
} updateplane2_t;

typedef struct wb3updateplane2_s // 21 1E
{
    int16_t timeoffset; // plane offset (difference in miliseconds from last position packet)
    u_int8_t slot; // plane slot
    u_int8_t unk1; // not known, value = 0x10
    int16_t relposx; // relative pos to updateplane_t.posx
    int16_t relposy; // relative pos to updateplane_t.posy
    int16_t relalt; // relative pos to updateplane_t.alt
    int8_t pitch; // pitch angle
    int8_t xaccel; // acceleration in X axis
    int16_t prxspeed; // pitch angular speed (7bits) AND X axis speed (9bits)
    int8_t roll; // roll angle
    int8_t yaccel; // acceleration in Y axis
    int16_t bryspeed; // roll angular speed (7bits) AND Y axis speed (9bits)
    int8_t yaw; // yaw angle
    int8_t zaccel; // acceleration in Z axis
    int16_t yrzspeed; // yaw angular speed (7bits) AND Z axis speed (9bits)
} wb3updateplane2_t;

typedef struct planestatus2_s // 21 02
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int32_t status_damage;
    u_int32_t status_status;
} planestatus2_t;

typedef struct radardot1_s // 21 05
{
    u_int16_t packetid;
    u_int8_t numdots;
} radardot1_t;

typedef struct radardot2_s // 21 05
{
    u_int16_t slot;
    int16_t posx;
    int16_t posy;
    u_int8_t country;
} radardot2_t;

typedef struct wb3updategunner_s // 21 07
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int8_t gun;
    u_int8_t pitch;
    u_int8_t yaw;
} wb3updategunner_t;

typedef struct addaiplane_s // 21 08
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int32_t shortnick;
    u_int32_t country;
    u_int16_t plane;
    u_int16_t unknown1; // log show 0x05
    u_int8_t unknown2;
} addaiplane_t;

typedef struct wb3supressfire_s // 21 22
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int32_t supress;
} wb3supressfire_t;

typedef struct wb3aifillslot_s // 00 08
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int32_t shortnick;
    u_int32_t country;
    u_int16_t plane;
    u_int16_t unk1;
    u_int8_t cvnum;
} wb3aifillslot_t;

typedef struct wb3allaiplanesupdate_s // 00 15
{
    u_int16_t packetid;
    u_int16_t number;
    u_int32_t posx;
    u_int32_t posy;
    u_int32_t unk1;
    u_int32_t unk2;
    u_int32_t country;
    u_int32_t plane;
    u_int16_t slot;
} wb3allaiplanesupdate_t;

typedef struct wb3aimount_s // 00 18
{
    u_int16_t packetid;
    u_int16_t unk1;
    u_int8_t cvnum;
    u_int8_t inout; // 1 in, 2 out
} wb3aimount_t;

typedef struct wb3planeskin_s // 00 2C
{
    u_int16_t packetid;
    u_int8_t msgsize;
    u_int8_t msg;
} wb3planeskin_t;

typedef struct wb3overrideskin_s // 00 2D
{
    u_int16_t packetid;
    u_int8_t slot;
    u_int8_t msgsize;
    u_int8_t msg;
} wb3overrideskin_t;

typedef struct datagram_s //
{
    u_int16_t size;
    u_int8_t data; // use as &(variable->data)
} datagram_t;

typedef struct debug_s
{
    unsigned j_ebp; /*****************/
    unsigned j_ebx; /*               */
    unsigned j_edi; /*    Calling    */
    unsigned j_esi; /*  environment  */
    unsigned j_esp; /*               */
    unsigned j_ret; /*****************/
    unsigned j_excep; /* Saved the signal mask?  */
    unsigned j_context; /* Saved signal mask.  */
} debug_t;

#pragma pack()

extern arena_t *arena; /// arena settings
extern arenaslist_t arenaslist[MAX_ARENASLIST]; /// list of online arenas
extern client_t *clients; /// list of clients
extern u_int32_t factorybuildings[4];
extern u_int32_t factorybuildingsup[4];
extern u_int32_t maxmuntype; /// get this value from number of entries in DB
extern u_int32_t maxplanes; /// get this value from number of entries in DB
extern jmp_buf debug_buffer; /// debug buffer
extern u_int8_t mainbuffer[MAX_RECVDATA];
extern var_t *var_vars; /// list of variables
extern GeoIP *gi; /// GeoIP struct
extern MYSQL my_sock; /// mysql socket
extern MYSQL_RES *my_result; /// mysql results' main pointer
extern MYSQL_ROW my_row; /// mysql main row
extern u_int32_t my_id; /// mysql main id from AUTO_INCREMENT
extern char my_query[MAX_QUERY]; /// main query buffer
extern u_int16_t packets_tab[210][3];
extern var_t *ackstardisable; /// disable ackstar
extern var_t *ackgrowco; /// Fixed ack grow coefficient
extern var_t *ackshrinkco; /// Fixed ack shrink coefficient
extern var_t *ackmaxtrav; /// Fixed ack max traverse rate
extern var_t *airshowsmoke; /// enable air show smoke
extern var_t *allowtakeoff; /// allow players to takeoff
extern var_t *altv; /// SHOW: 0-15 ??? other altv configures?
extern var_t *ammomult; ///
extern var_t *arcade; /// enable arena arcade mode
extern var_t *arenaflags1; /// arena flags
extern var_t *arenaflags2; /// arena flags
extern var_t *arenaflags3; /// arena flags
extern var_t *arenalist; /// domain of central arena that receive UDP packets
extern var_t *batchfile; /// exec batch file on login
extern var_t *blackout; /// enable blackout
extern var_t *boidgroups; /// num of boid groups by side, including CV's
extern var_t *broadcast; /// enable broadcast channel
extern var_t *bulletradius; /// bulletradius value
extern var_t *canreload; /// enable reload airplanes
extern var_t *canreset; /// enable reset
extern var_t *clickrangelim; /// Maximum range for left/right click detect
extern var_t *cities; /// num of cities (radars, ports, cities, refineries...)
extern var_t *changemaponreset; /// enable change map on reset mode
extern var_t *consoleinput; /// enable/disable console input (windows stuffs, arg!)
extern var_t *contrail; /// contrail altitude limit
extern var_t *countries; /// num of countries playing the game
extern var_t *countrytime; /// limit change country for X seconds
extern var_t *crcview; /// temporary var
extern var_t *ctf; /// ctf mode
extern var_t *currday; /// set current day
extern var_t *currmonth; /// set current month
extern var_t *curryear; /// set current year
extern var_t *cvcapture; /// enable cv/cargo capture
extern var_t *cvdelay; /// time between salvos
extern var_t *cvradarrange0; /// radar range of neutral CV
extern var_t *cvradarrange1; /// radar range of red CV
extern var_t *cvradarrange2; /// radar range of green CV
extern var_t *cvradarrange3; /// radar range of gold CV
extern var_t *cvradarrange4; /// radar range of purple CV
extern var_t *cvrange; /// max range of CV's artillery
extern var_t *cvs; /// num of cv's in map
extern var_t *cvsalvo; /// amount of bullets fired in a time from artillery
extern var_t *cvspeed; /// CV speed in ft/s
extern var_t *database; /// DB name
extern var_t *dayhours; /// how many hours have a day
extern var_t *dbpasswd; /// DB password
extern var_t *dbuser; /// DB username
extern var_t *debug; /// debugvar
extern var_t *destroytokill; /// Need to destroy plane part to get a kill
extern var_t *printqueries; /// debugvar
extern var_t *dirname; /// directory of current map
extern var_t *dpitch; /// debugvar
extern var_t *droll; /// debugvar
extern var_t *dyaw; /// debugvar
extern var_t *easymode; /// allow easymode [1] and force easy mode [2]
extern var_t *economy;
extern var_t *emulatecollision; /// emulate collision with the other side (e.g. if one client collided, other will be forced to collide
extern var_t *enableottos; /// allow to use ottos
extern var_t *endday; /// set end day
extern var_t *endmonth; /// set end month
extern var_t *endyear; /// set end year
extern var_t *enemydotsfly; ///
extern var_t *enemydotstwr; ///
extern var_t *enemyidlim; ///
extern var_t *enemyidlimbomber; ///
extern var_t *enemynames; /// allow to see enemy names
extern var_t *enemyplanes; /// allow to see enemy planes icon
extern var_t *fields; /// num of fields
extern var_t *flakmax; /// Maximum altitude for fixed ack 88 guns
extern var_t *flypenalty; /// how much seconds client will be penalized
extern var_t *friendlydotsfly; ///
extern var_t *friendlydotstwr; ///
extern var_t *friendlyfire; /// allow to shot down friendly planes
extern var_t *friendlyidlim; ///
extern var_t *friendlyidlimbomber; ///
extern var_t *gruntsmaxd; /// WB3 distance where grund are heard
extern var_t *gruntshoot; /// WB3 enable shoot grunt
extern var_t *gruntcapture; /// WB3 capture grunt
extern var_t *fueldiv; ///
extern var_t *gunrad; /// gunradius value
extern var_t *gunstats; /// enable print gunstats
extern var_t *gwarning; /// warning limit of G overload
extern var_t *hideadmin; /// turns Admins invisible for .ros and entered exits game messages
extern var_t *hostdomain; /// domain of arena
extern var_t *hostname; /// name of arena
extern var_t *iff; /// enable Identification Friendly or Foe
extern var_t *initday; /// set initial day
extern var_t *initmonth; /// set initial month
extern var_t *inityear; /// set initial year
extern var_t *iconbombersoverride; /// allow to override bomber icons
extern var_t *katyrange; /// max range of KATY fire
extern var_t *killcvtoreset; /// CVs are in reset accounting
extern var_t *landingcapture; /// enable landing capture
extern var_t *lethality; /// damage multiplier
extern var_t *logfile_active;
extern var_t *lograwposition; /// log raw position in files
extern var_t *mapcycle; /// enable mapcycle
extern var_t *mapflags; /// enable mapflags
extern var_t *mapflagsenemy; /// show circles of enemy countries
extern var_t *mapflagsfly; /// show circles while flying
extern var_t *mapflagsown; /// show circles of own country
extern var_t *mapflagstwr; /// show circles while in twr
extern var_t *mapname; /// name of current map
extern var_t *mapscale; /// map square scale
extern var_t *mapsize; /// map size in miles
extern var_t *maxclients; /// maximum of incomming connections
extern var_t *maxentities; /// maximum of players + drones in world
extern var_t *maxpilotg; ///
extern var_t *metar; /// allow use metar information
extern var_t *midairs; /// midairs collisions
extern var_t *mortars; /// set how much mortars JU52 and Li-2 can fire
extern var_t *mview; /// set who can use .view (OPs, Admins or both)
extern var_t *notanks; /// dont allow players and arena to spawn tanks/hmack/katy
extern var_t *nowings; /// dont allow players to use wingmen
extern var_t *obradar; /// adjust obradar for historical planes
extern var_t *oldcapt; /// enable old way to capture fields (destroy all structures and drop paras)
extern var_t *ottoaccuracy; /// set otto accuracy
extern var_t *ottoacquirerange; /// set otto acquire range
extern var_t *ottoadjust; /// set otto adjust
extern var_t *ottoburstoff; /// set otto burst pause time
extern var_t *ottoburston; /// set otto burst time by dist
extern var_t *ottoburstonmax; /// set otto burst time by hit
extern var_t *ottorange; /// set otto max range
extern var_t *ottoretarget; /// set otto retarget rime
extern var_t *ottooverrides; /// set parameters client can override
extern var_t *overload; /// set value for overload msg
extern var_t *parassmall; /// paratroopers needed to capture a small field
extern var_t *parasmedium; /// paratroopers needed to capture a medium field
extern var_t *paraslarge; /// paratroopers needed to capture a large field
extern var_t *paraspost; /// paratroopers needed to capture a Post
extern var_t *parasvillage; /// paratroopers needed to capture a Village
extern var_t *parasport; /// paratroopers needed to capture a Port
extern var_t *parastown; /// paratroopers needed to capture a Town
extern var_t *planeatradar; /// show planes models at radar range
extern var_t *planerangelimit; ///
extern var_t *planerangelimitbomber; ///
extern var_t *port; /// server port
extern var_t *predictpos; /// enable position prediction with x degree
extern var_t *printeject; /// print eject messages to everyone
extern var_t *printkills; /// print kills messages
extern var_t *radaralt; ///
extern var_t *radarheight; ///
extern var_t *radarrange0; ///
extern var_t *radarrange1; ///
extern var_t *radarrange2; ///
extern var_t *radarrange3; ///
extern var_t *radarrange4; ///
extern var_t *rebuildtime; /// rebuilding time base seconds
extern var_t *resetsred; // How many resets by red
extern var_t *resetsgold; // How many resets by gold
extern var_t *respawncvs; /// after killed, cv will respawn
extern var_t *rps; /// enable rps (value = minutes)
extern var_t *server_speeds; /// show server speed
extern var_t *skins; /// enable skins
extern var_t *spawnred; ///
extern var_t *spawngold; ///
extern var_t *sqlserver; /// sql server address
extern var_t *startalt; ///
extern var_t *structlim; ///
extern var_t *tanksrange; /// max range of TANKS fire
extern var_t *teamkiller; /// Allow server set TK flag for teamkillers
extern var_t *teamkillstructs; /// Allow to damage friendly structures
extern var_t *testarena; /// Enable Test Arena features
extern var_t *thskins; /// enable force TH Skins
extern var_t *timemult; /// arena time multiplier
extern var_t *timeout; /// away timer
extern var_t *ttc; /// tonnage to close
extern var_t *verbose; /// printf messages priority
extern var_t *wb3; /// enable WB3 protocol
extern var_t *wbversion; /// WB Version
extern var_t *weather; /// configure weather
extern var_t *whitelist; /// white list
extern var_t *wingstrikerng; ///
extern var_t *xwindvelocity; ///
extern var_t *ywindvelocity; ///
extern var_t *zwindvelocity; ///

extern FILE *logfile[MAX_LOGFILE]; /// general logfile

#endif /* SHARED_H_ */
