/***
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
 ***/

#include "shared.h"

var_t *ackgrowco; // extern
var_t *ackshrinkco; // extern
var_t *ackmaxtrav; // extern
var_t *ackstardisable; // extern
var_t *airshowsmoke; // extern
var_t *allowtakeoff; // extern
var_t *altv; // extern
var_t *ammomult; // extern
var_t *arcade; // extern
var_t *arenaflags1; // extern
var_t *arenaflags2; // extern
var_t *arenaflags3; // extern
var_t *arenalist; // extern
var_t *batchfile; // extern
var_t *blackout; // extern
var_t *broadcast; // extern
var_t *bulletradius; // extern
var_t *canreload; // extern
var_t *canreset; // extern
var_t *clickrangelim; // extern
var_t *cities; // extern
var_t *changemaponreset; // extern
var_t *consoleinput; // extern
var_t *contrail; // extern
var_t *countries; // extern
var_t *countrytime; // extern
var_t *crcview; // extern
var_t *ctf; // extern
var_t *currday; // extern
var_t *currmonth; // extern
var_t *curryear; // extern
var_t *cvcapture; // extern
var_t *cvdelay; // extern
var_t *cvradarrange0; // extern
var_t *cvradarrange1; // extern
var_t *cvradarrange2; // extern
var_t *cvradarrange3; // extern
var_t *cvradarrange4; // extern
var_t *cvrange; // extern
var_t *cvs; // extern
var_t *cvsalvo; // extern
var_t *cvspeed; // extern
var_t *database; // extern
var_t *dayhours; // extern
var_t *dbpasswd; // extern
var_t *dbuser; // extern
var_t *debug; // extern
var_t *dirname; // extern
var_t *dpitch; // extern
var_t *droll; // extern
var_t *dyaw; // extern
var_t *easymode; // extern
var_t *emulatecollision; // extern
var_t *enableottos; // extern
var_t *endday; // extern
var_t *endmonth; // extern
var_t *endyear; // extern
var_t *enemydotsfly; // extern
var_t *enemydotstwr; // extern
var_t *enemyidlim; // extern
var_t *enemyidlimbomber; // extern
var_t *enemynames; // extern
var_t *enemyplanes; // extern
var_t *fields; // extern
var_t *flakmax; // extern
var_t *flypenalty; // extern
var_t *friendlydotsfly; // extern
var_t *friendlydotstwr; // extern
var_t *friendlyfire; // extern
var_t *friendlyidlim; // extern
var_t *friendlyidlimbomber; // extern
var_t *fueldiv; // extern
var_t *gruntsmaxd; // extern
var_t *gruntshoot; // extern
var_t *gruntcapture; // extern
var_t *gunrad; // extern
var_t *gunstats; // extern
var_t *gwarning; // extern
var_t *hideadmin; // extern
var_t *hostdomain; // extern
var_t *hostname; // extern
var_t *iconbombersoverride; //extern
var_t *iff; // extern
var_t *initday; // extern
var_t *initmonth; // extern
var_t *inityear; // extern
var_t *katyrange; // extern
var_t *killcvtoreset; // extern
var_t *landingcapture; // extern
var_t *lethality; // extern
var_t *logfile_active; // extern
var_t *lograwposition; // extern
var_t *mapcycle; // extern
var_t *mapflags; // extern
var_t *mapflagsenemy; // extern
var_t *mapflagsfly; // extern
var_t *mapflagsown; // extern
var_t *mapflagstwr; // extern
var_t *mapname; // extern
var_t *mapscale; // extern
var_t *mapsize; // extern
var_t *maxclients; // extern
var_t *maxentities; // extern
var_t *maxpilotg; // extern
var_t *metar; // extern
var_t *midairs; // extern
var_t *mortars; // extern
var_t *mview; // extern
var_t *notanks; // extern
var_t *nowings; // extern
var_t *obradar;	// extern
var_t *oldcapt; // extern
var_t *ottoaccuracy; // set otto accuracy
var_t *ottoacquirerange; // extern
var_t *ottoadjust; // set otto adjust
var_t *ottoburstoff; // set otto burst pause time
var_t *ottoburston; // set otto burst time by dist
var_t *ottoburstonmax; // set otto burst time by hit
var_t *ottorange; // set otto max range
var_t *ottoretarget; // set otto retarget rime
var_t *ottooverrides; // set parameters client can override
var_t *overload; // extern
var_t *parassmall; // extern
var_t *parasmedium; // extern
var_t *paraslarge; // extern
var_t *paraspost; // extern
var_t *parasvillage; // extern
var_t *parasport; // extern
var_t *parastown; // extern
var_t *planeatradar; // extern
var_t *planerangelimit; // extern
var_t *planerangelimitbomber; // extern
var_t *port; // extern
var_t *predictpos; // extern
var_t *printeject; // extern
var_t *printkills; // extern
var_t *printqueries; // extern
var_t *radaralt; // extern
var_t *radarheight; // extern
var_t *radarrange0; // extern
var_t *radarrange1; // extern
var_t *radarrange2; // extern
var_t *radarrange3; // extern
var_t *radarrange4; // extern
var_t *rebuildtime; // extern
var_t *respawncvs; // extern
var_t *rps; // extern
var_t *server_speeds; // extern
var_t *skins; // extern
var_t *spawnred; // extern
var_t *spawngold; // extern
var_t *sqlserver; // extern
var_t *startalt; // extern
var_t *structlim; // extern
var_t *tanksrange; // extern
var_t *teamkiller; // extern
var_t *teamkillstructs; // extern
var_t *testarena; // extern
var_t *thskins; // extern
var_t *timemult; // extern
var_t *timeout; // extern
var_t *ttc; // tonnage to close
var_t *verbose; // extern
var_t *wb3; // extern
var_t *weather; // extern
var_t *whitelist; // extern
var_t *wingstrikerng; // extern
var_t *xwindvelocity; // extern
var_t *ywindvelocity; // extern
var_t *zwindvelocity; // extern

var_t *var_vars; // extern


/**
 Init_Vars

 Initialize variable table
 */

void InitVars(void)
{
	port = Var_Get("port", "19243", "19243", "19250", "Set the listen port", 0);
#ifdef RETAIL	// RETAIL is a limited version for development or lan party
	maxclients = Var_Get("maxclients", "6", "6", "100", "Set the max clients allowed to play", VAR_NOSET);
	maxentities = Var_Get("maxentities", "12", "12", "200", "Set the max entities allowed to spawn in game", VAR_NOSET);
#else
	maxclients = Var_Get("maxclients", "100", "6", "100", "Set the max clients allowed to play", VAR_ADMIN);
	maxentities = Var_Get("maxentities", "200", "12", "200", "Set the max entities allowed to spawn in game", VAR_ADMIN);
#endif
	ackgrowco = Var_Get("ackgrowco", "8.5", "0.0", "20.0", "Fixed ack grow coefficient", VAR_ARCHIVE);
	ackshrinkco = Var_Get("ackshrinkco", "85", "0", "100", "Fixed ack shrink coefficient", VAR_ARCHIVE);
	ackmaxtrav = Var_Get("ackmaxtrav", "40", "0", "180", "Fixed ack max traverse rate", VAR_ARCHIVE);
	ackstardisable = Var_Get("ackstardisable", "0", "0", "1", "Disable ackstar", VAR_ARCHIVE);
	airshowsmoke = Var_Get("airshowsmoke", "0", "0", "1", "Allow air show smoke (.smoke)", VAR_ARCHIVE);
	allowtakeoff = Var_Get("allowtakeoff", "1", "0", "1", "Allow takeoffs", VAR_ARCHIVE);
	altv = Var_Get("altv", "0", "0", "15", "Enable external view", VAR_ARCHIVE);
	ammomult = Var_Get("ammomult", "100", "0.1", "32", "Relative amount of ammo each plane carries", VAR_ARCHIVE);
	arcade = Var_Get("arcade", "0", "0", "1", "Enable arcade mode", VAR_ARCHIVE);
	arenaflags1 = Var_Get("arenaflags1", "3", "0", "255", "Bitfield configuration for .arnaflags", VAR_ARCHIVE);
	arenaflags2 = Var_Get("arenaflags2", "0", "0", "255", "Bitfield configuration for .arnaflags", VAR_ARCHIVE);
	arenaflags3 = Var_Get("arenaflags3", "27", "0", "255", "Bitfield configuration for .arnaflags", VAR_ARCHIVE);
	arenalist = Var_Get("arenalist", "0", "0", "0", "URL to send arenalist data (0 = disabled)", VAR_ADMIN);
	batchfile = Var_Get("batchfile", "0", "0", "1", "Exec server-side player's batch file on login", VAR_ARCHIVE);
	blackout = Var_Get("blackout", "1", "0", "1", "Enable blackout effect", VAR_ARCHIVE);
	broadcast = Var_Get("broadcast", "1", "0", "1", "Enable talking in channel 100", VAR_ARCHIVE);
	bulletradius = Var_Get("bulletradius", "0.1", "0.01", "8.00", "Collision bubble for projectiles", VAR_ARCHIVE);
	canreload = Var_Get("canreload", "0", "0", "1", "Enable reaload airplanes at landing", VAR_ARCHIVE);
	canreset = Var_Get("canreset", "1", "0", "1", "Enable automatically reset arena", VAR_ARCHIVE | VAR_ADMIN);
	clickrangelim = Var_Get("clickrangelim", "1500", "0", "40500", "Maximum range for left/right click detect", VAR_ARCHIVE);
	cities = Var_Get("cities", "5", "0", "100", "Number of cities in this map", VAR_ARCHIVE | VAR_ADMIN);
	changemaponreset = Var_Get("changemaponreset", "0", "0", "1", "Enable change map on reset", VAR_ARCHIVE | VAR_ADMIN);
	consoleinput = Var_Get("consoleinput", "1", "0", "1", "Enable console input", VAR_ARCHIVE | VAR_ADMIN);
	contrail = Var_Get("contrail", "20000", "0", "1", "Set altitude for contrail effect (WB2.77)", VAR_ARCHIVE);
	countries = Var_Get("countries", "2", "0", "1", "Number of countries playing (not used yet)", VAR_ARCHIVE | VAR_ADMIN);
	countrytime = Var_Get("countrytime", "300", "0", "86400", "Time in seconds until can switch country", VAR_ARCHIVE);
	crcview = Var_Get("crcview", "0", "0", "1", "Enable prints CRC value in radio (WB2.77)", VAR_ARCHIVE);
	ctf = Var_Get("ctf", "0", "0", "1", "Enable capture the flag mode", VAR_ARCHIVE);
	currday = Var_Get("currday", "1", "1", "31", "Current day", VAR_ARCHIVE);
	currmonth = Var_Get("currmonth", "9", "1", "12", "Current month", VAR_ARCHIVE);
	curryear = Var_Get("curryear", "1939", "1910", "1950", "Current year", VAR_ARCHIVE);
	cvcapture = Var_Get("cvcapture", "1", "0", "1", "Enable CV capture when destroyed", VAR_ARCHIVE);
	cvdelay = Var_Get("cvdelay", "30", "10", "300", "Set delay in seconds between CV salvos", VAR_ARCHIVE);
	cvradarrange0 = Var_Get("cvradarrange0", "30", "0", "100000", "Set Country 0 CV radar range", VAR_ARCHIVE);
	cvradarrange1 = Var_Get("cvradarrange1", "50000", "0", "100000", "Set Country 1 CV radar range", VAR_ARCHIVE);
	cvradarrange2 = Var_Get("cvradarrange2", "50000", "0", "100000", "Set Country 2 CV radar range", VAR_ARCHIVE);
	cvradarrange3 = Var_Get("cvradarrange3", "50000", "0", "100000", "Set Country 3 CV radar range", VAR_ARCHIVE);
	cvradarrange4 = Var_Get("cvradarrange4", "50000", "0", "100000", "Set Country 4 CV radar range", VAR_ARCHIVE);
	cvrange = Var_Get("cvrange", "40000", "500", "45000", "Set CV artillary range", VAR_ARCHIVE); // max of 46339
	cvs = Var_Get("cvs", "1", "0", "10", "Set number of CVs in this map", VAR_ARCHIVE | VAR_ADMIN);
	cvsalvo = Var_Get("cvsalvo", "10", "1", "50", "Set how many bullets CV fires by salvo", VAR_ARCHIVE);
	cvspeed = Var_Get("cvspeed", "50", "0", "70", "Set max CV speed", VAR_ARCHIVE);
	database = Var_Get("database", "tabahost", "0", "0", "Set database name", VAR_ADMIN);
	dayhours = Var_Get("dayhours", "14", "10", "14", "Set how many hours have a day", VAR_ARCHIVE);
	dbpasswd = Var_Get("dbpasswd", "t4b4d4t4b4s3", "0", "0", "Set database password", VAR_ADMIN);
	dbuser = Var_Get("dbuser", "franz", "0", "0", "Set database username", VAR_ADMIN);
	dirname = Var_Get("dirname", "wb3europe", "0", "0", "Set map directory name", VAR_ARCHIVE | VAR_ADMIN);
	easymode = Var_Get("easymode", "0", "0", "2", "Enable easy mode (2 = forced easy mode)", VAR_ARCHIVE);
	emulatecollision = Var_Get("emulatecollision", "0", "1", "Emulate friendly collision (WB2.77)", NULL, VAR_ARCHIVE);
	enableottos = Var_Get("enableottos", "1", "0", "1", "Enable autogunners (OTTO)", VAR_ARCHIVE);
	endday = Var_Get("endday", "16", "1", "31", "End of TOD day", VAR_ADMIN);
	endmonth = Var_Get("endmonth", "8", "1", "12", "End of TOD month", VAR_ADMIN);
	endyear = Var_Get("endyear", "1945", "1910", "1950", "End of TOD year", VAR_ADMIN);
	enemydotsfly = Var_Get("enemydotsfly", "1", "0", "1", "Radar: all enemy aircraft shown?", VAR_ARCHIVE);
	enemydotstwr = Var_Get("enemydotstwr", "1", "0", "1", "Radar: enemy tower based on", VAR_ARCHIVE);
	enemyidlim = Var_Get("enemyidlim", "1500", "0", "40500", "Maximum range for display of enemy name/plane icons", VAR_ARCHIVE);
	enemyidlimbomber = Var_Get("enemyidlimbomber", "1500", "0", "40500", "Maximum range for display of enemy name/plane icons for bombers", VAR_ARCHIVE);
	enemynames = Var_Get("enemynames", "0", "0", "1", "Allow display of enemy names in icons", VAR_ARCHIVE);
	enemyplanes = Var_Get("enemyplanes", "1", "0", "1", "Allow display of enemy plane in icons", VAR_ARCHIVE);
	fields = Var_Get("fields", "21", "0", "150", "Number of fields in this map", VAR_ARCHIVE | VAR_ADMIN);
	flakmax = Var_Get("flakmax", "12000", "0", "50000", "Maximum altitude for fixed ack 88 guns", VAR_ARCHIVE);
	flypenalty = Var_Get("flypenalty", "150", "0", "600", "Set time slot (in seconds) between flights", VAR_ARCHIVE);
	friendlydotsfly = Var_Get("friendlydotsfly", "1", "0", "1", "Radar: all friendly aircraft shown?", VAR_ARCHIVE);
	friendlydotstwr = Var_Get("friendlydotstwr", "1", "0", "1", "Radar friendly tower based on", VAR_ARCHIVE);
	friendlyfire = Var_Get("friendlyfire", "1", "0", "1", "Enable friendly fire", VAR_ARCHIVE);
	friendlyidlim = Var_Get("friendlyidlim", "1500", "0", "40500", "Maximum range for display of friendly name/plane icons", VAR_ARCHIVE);
	friendlyidlimbomber = Var_Get("friendlyidlimbomber", "1500", "0", "40500", "Maximum range for display of friendly name/plane icons for bombers", VAR_ARCHIVE);
	fueldiv = Var_Get("fueldiv", "100", "0.01", "10.0", "Modifies the fuel flow of aircraft. 1 = normal", VAR_ARCHIVE);
	gruntcapture = Var_Get("gruntcapture", "1", "0", "1", "Allow grunts to be captured by driving vehicle", VAR_ARCHIVE);
	gruntshoot = Var_Get("gruntshoot", "1", "0", "1", "Allow grunts to be shot", VAR_ARCHIVE);
	gruntsmaxd = Var_Get("gruntsmaxd", "6000", "100", "12000", "Maximum distance from tower grunts can be dropped to start capture attack", VAR_ARCHIVE);
	gunrad = Var_Get("gunrad", "0.1", "0.01", "8.00", "Collision bubble for projectiles launched", VAR_ARCHIVE);
	gunstats = Var_Get("gunstats", "0", "0", "2", "Enable gunstats log. 1 = log, 2 = player and log", VAR_ARCHIVE);
	gwarning = Var_Get("gwarning", "0.5", "0.1", "3", "Set threshold to warn player about G overload", VAR_ARCHIVE);
	hideadmin = Var_Get("hideadmin", "1", "0", "1", "Hide admins from .ros and radio announcements", VAR_ARCHIVE | VAR_ADMIN);
	hostdomain = Var_Get("hostdomain", "localhost", "0", "0", "Set host domain", VAR_ADMIN);
	hostname = Var_Get("hostname", "Main Arena", "0", "0", "Set host name in arenalist", VAR_ARCHIVE | VAR_ADMIN);
	iconbombersoverride = Var_Get("iconbombersoverride", "1", "0", "1", "Enable override for bombers icons settings", VAR_ARCHIVE);
	iff = Var_Get("iff", "1", "0", "1", "Enable country identification in radar dots", VAR_ARCHIVE);
	initday = Var_Get("initday", "1", "1", "31", "Init of TOD day", VAR_ADMIN);
	initmonth = Var_Get("initmonth", "9", "1", "12", "Init of TOD month", VAR_ADMIN);
	inityear = Var_Get("inityear", "1939", "1910", "1950", "Init of TOD year", VAR_ADMIN);
	katyrange = Var_Get("katyrange", "8000", "500", "45000", "Set Katyusha fire range", VAR_ARCHIVE); // theorical max of 46339
	killcvtoreset = Var_Get("killcvtoreset", "1", "0", "1", "Set if need to capture all CVs to reset the map", VAR_ARCHIVE);
	landingcapture = Var_Get("landingcapture", "0", "0", "1", "Enable landing capture (don't need paratroopers)", VAR_ARCHIVE | VAR_ADMIN);
	lethality = Var_Get("lethality", "1", "0.1", "3", "Set weapons lethality coefficient", VAR_ARCHIVE | VAR_ADMIN);
	logfile_active = Var_Get("logfile_active", "1", "0", "1", "Enable logfiles", VAR_ARCHIVE | VAR_ADMIN);
	lograwposition = Var_Get("lograwposition", "0", "0", "1", "Enable log of RAW position (.srv and .cli files)", VAR_ARCHIVE);
	mapcycle = Var_Get("mapcycle", "1", "0", "1", "Enable mapcycle", VAR_ARCHIVE);
	mapflags = Var_Get("mapflags", "1", "0", "1023", "Set Map Flags", VAR_ARCHIVE);
	mapflagsenemy = Var_Get("mapflagsenemy", "1", "0", "1", "Show circles of enemy countries", VAR_ARCHIVE);
	mapflagsfly = Var_Get("mapflagsfly", "0", "0", "1", "Show circles while flying", VAR_ARCHIVE);
	mapflagsown = Var_Get("mapflagsown", "1", "0", "1", "Show circles of own country", VAR_ARCHIVE);
	mapflagstwr = Var_Get("mapflagstwr", "1", "0", "0", "Show circles while in twr", VAR_ARCHIVE);
	mapname = Var_Get("mapname", "europe", "0", "0", "Current map name", VAR_ARCHIVE | VAR_ADMIN);
	mapscale = Var_Get("mapscale", "20", "10", "50", "Size of map quadrants in miles", VAR_ARCHIVE | VAR_ADMIN);
	mapsize = Var_Get("mapsize", "128", "100", "500", "Size of entire map in miles", VAR_ARCHIVE | VAR_ADMIN);
	maxpilotg = Var_Get("maxpilotg", "11", "5", "50", "Maximum G load before pilot fatality", VAR_ARCHIVE);
	metar = Var_Get("metar", "0", "0", "1", "Enable metar based weather generation", VAR_ARCHIVE);
	midairs = Var_Get("midairs", "1", "0", "1", "Enable midair collisions detection", VAR_ARCHIVE);
	mortars = Var_Get("mortars", "10", "0", "20", "Set how many mortars JU52 and Li-2 can fire", VAR_ARCHIVE);
	mview = Var_Get("mview", "1", "0", "3", "Set who can use .view (OPs, Admins or both)", VAR_ARCHIVE | VAR_ADMIN);
	notanks = Var_Get("notanks", "0", "0", "1", "Don't allow players and arena to spawn tanks/hmack/katy", VAR_ARCHIVE);
	nowings = Var_Get("nowings", "0", "0", "1", "Don't allow players to use wingmen", VAR_ARCHIVE);
	obradar = Var_Get("obradar", "6000", "0", "12000", "Set on-board radar range for planes that have radar", VAR_ARCHIVE); // D20
	oldcapt = Var_Get("oldcapt", "1", "0", "1", "Enable old way to capture fields (destroy all structures and drop paras)", VAR_ARCHIVE);
	ottoaccuracy = Var_Get("ottoaccuracy", "5", "1", "10", "A general accuracy setting, this affects the otto's accuracy, reaction time and tracking", VAR_ARCHIVE);
	ottoacquirerange = Var_Get("ottoacquirerange", "1500", "0", "40500", "Range at which otto will acquire a target (call out and begin tracking)", VAR_ARCHIVE);
	ottoadjust = Var_Get("ottoadjust", "6", "1", "12", "Additional tweak value for accuracy/distance", VAR_ARCHIVE);
	ottoburstoff = Var_Get("ottoburstoff", "1.0", "0.5", "5.0", "Determines the length of the pause in between bursts. Proportional to target distance so pause decreases with range", VAR_ARCHIVE);
	ottoburston = Var_Get("ottoburston", "1.0", "0.5", "4.0", "Determines the length of a burst. Inverse to target distance so burst length increases as target gets nearer. Increasing this value increases the volume of fire", VAR_ARCHIVE);
	ottoburstonmax = Var_Get("ottoburstonmax", "3.0", "0.5", "4.0", "Works with ottoburston parameter in determining burst length by factoring in the hit ratio, so that as hits are achieved burst time is increased. Increasing this value increases the chances of inflicting damage", VAR_ARCHIVE);
	ottooverrides = Var_Get("ottooverrides", "0", "0", "511", "Determines which otto settings a player can override", VAR_ARCHIVE);
	ottorange = Var_Get("ottorange", "10", "1", "60", "Determines the range at which the otto will commence firing (x100 yd)", VAR_ARCHIVE);
	ottoretarget = Var_Get("ottoretarget", "0.2", "0.0", "10.0", "Determines frequency in seconds at which otto will look for new targets", VAR_ARCHIVE);
	overload = Var_Get("overload", "500", "10", "1500", "Set the threashold for server overload alert in milliseconds", VAR_ARCHIVE | VAR_ADMIN);
	parassmall = Var_Get("parassmall", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	parasmedium = Var_Get("parasmedium", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	paraslarge = Var_Get("paraslarge", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	paraspost = Var_Get("paraspost", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	parasvillage = Var_Get("parasvillage", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	parasport = Var_Get("parasport", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	parastown = Var_Get("parastown", "4", "1", "20", "Set the number of paratroopers needed to capture", VAR_ARCHIVE);
	planeatradar = Var_Get("planeatradar", "0", "0", "1", "Show plane 3D model in F1-radar view (WB2.77)", VAR_ARCHIVE);
	planerangelimit = Var_Get("planerangelimit", "1500", "0", "40500", "Maximum range for display of range icon", VAR_ARCHIVE);
	planerangelimitbomber = Var_Get("planerangelimitbomber", "1500", "0", "40500", "Maximum range for display of range icon for bombers", VAR_ARCHIVE);
	predictpos = Var_Get("predictpos", "0", "0", "4", "Enable predictor corrector algorithm", 0);
	printeject = Var_Get("printeject", "0", "0", "1", "Enable broadcast when player ejects", VAR_ARCHIVE);
	printkills = Var_Get("printkills", "1", "0", "1", "Enable broadcast when player is killed", VAR_ARCHIVE);
	radaralt = Var_Get("radaralt", "600", "0", "50000", "Set minimum radar altitude threashold", VAR_ARCHIVE);
	radarheight = Var_Get("radarheight", "20000", "0", "50000", "Set maximum radar altitude threashold", VAR_ARCHIVE);
	radarrange0 = Var_Get("radarrange0", "30", "0", "100000", "Country 0 radar range", VAR_ARCHIVE);
	radarrange1 = Var_Get("radarrange1", "121500", "0", "100000", "Country 1 radar range", VAR_ARCHIVE);
	radarrange2 = Var_Get("radarrange2", "121500", "0", "100000", "Country 2 radar range", VAR_ARCHIVE);
	radarrange3 = Var_Get("radarrange3", "121500", "0", "100000", "Country 3 radar range", VAR_ARCHIVE);
	radarrange4 = Var_Get("radarrange4", "121500", "0", "100000", "Country 4 radar range", VAR_ARCHIVE);
	rebuildtime = Var_Get("rebuildtime", "12.0", "0.5", "20.0", "Set rebuild time coefficient", VAR_ARCHIVE);
	respawncvs = Var_Get("respawncvs", "1", "0", "1", "Set if after killed, CV will respawn", VAR_ARCHIVE);
	rps = Var_Get("rps", "60", "0.1", "120.0", "Set RPS update rate", VAR_ARCHIVE);
	server_speeds = Var_Get("server_speeds", "0", "0.1", "5", "Enable print server speeds (value is a frequency multiplier)", VAR_ARCHIVE | VAR_ADMIN);
	skins = Var_Get("skins", "0", "0", "1", "Enable online skins", VAR_ADMIN);
	spawnred = Var_Get("spawnred", "0", "0", "150", "Set red's spawn field", VAR_ARCHIVE);
	spawngold = Var_Get("spawngold", "0", "0", "150", "Set gold's spawn field", VAR_ARCHIVE);
	sqlserver = Var_Get("sqlserver", "localhost", "0", "0", "Set SQL server URL", VAR_ADMIN);
	startalt = Var_Get("startalt", "0", "0", "30000", "Set starting altitude", VAR_ARCHIVE);
	structlim = Var_Get("structlim", "3", "0", "3", "Sets structural limits in effect. 1 = speed, 2 = G, 3 = both", VAR_ARCHIVE);
	tanksrange = Var_Get("tanksrange", "5000", "500", "45000", "Set Tanks fire range", VAR_ARCHIVE); // max of 46339
	teamkiller = Var_Get("teamkiller", "1", "0", "1", "Allow server set TK flag for teamkillers", VAR_ARCHIVE);
	teamkillstructs = Var_Get("teamkillstructs", "1", "0", "1", "Allow to damage friendly structures", VAR_ARCHIVE);
	testarena = Var_Get("testarena", "0", "0", "1", "Enable Test Arena features", VAR_ARCHIVE);
	thskins = Var_Get("thskins", "0", "0", "1", "Enable force TH-rules skins", VAR_ARCHIVE);
	timemult = Var_Get("timemult", "6", "1", "12", "Set arena time multiplier", VAR_ARCHIVE);
	timeout = Var_Get("timeout", "120", "0", "600", "Set away kick timeout in seconds", VAR_ARCHIVE);
	ttc = Var_Get("ttc", "1.2", "0.5", "20.0", NULL, VAR_ARCHIVE);
	verbose = Var_Get("verbose", VERBOSE_SMAX, "0", VERBOSE_SMAX, "Set log verbose threashold", VAR_ARCHIVE | VAR_ADMIN);
	wb3 = Var_Get("wb3", "0", "0", "2", "Set WB3 mode. 0 = WB2.77, 1 = 2007, 2 = 2008/2009", VAR_ARCHIVE | VAR_ADMIN);
	weather = Var_Get("weather", "0", "0", "3", "Set weather. 0 = Cloudy, 1 = Clear, 2 = Raining, 3 = Partialy Cloudy", VAR_ARCHIVE);
	whitelist = Var_Get("whitelist", "0", "0", "1", "Enable whitelist filter", VAR_ARCHIVE);
	wingstrikerng = Var_Get("wingstrikerng", "30000", "0", "150000", NULL, VAR_ARCHIVE);
	xwindvelocity = Var_Get("xwindvelocity", "0", "-100", "100", "EW component of wind, + from West", VAR_ARCHIVE);
	ywindvelocity = Var_Get("ywindvelocity", "0", "-100", "100", "NS component of wind, + from North", VAR_ARCHIVE);
	zwindvelocity = Var_Get("zwindvelocity", "0", "-100", "100", "Up-Down component of wind, - from ground", VAR_ARCHIVE);

	debug = Var_Get("debug", "0", "0", "0", NULL, 0);
	printqueries = Var_Get("printqueries", "0", "0", "0", NULL, 0);
	dpitch = Var_Get("dpitch", "0", "0", "0", NULL, 0);
	droll = Var_Get("droll", "0", "0", "0", NULL, 0);
	dyaw = Var_Get("dyaw", "0", "0", "0", NULL, 0);
}

/**
 CheckVars

 Check if vars was modified and make its action
 */

void CheckVars(void)
{
	u_int8_t modified = 0;
	u_int8_t i;
	u_int32_t date;
	var_t *var;

	if (logfile_active->modified)
	{
		modified = 1;

		if ((!logfile_active->value))
		{
			for(i = 0; i < MAX_LOGFILE; i++)
			{
				if(logfile[i])
				{
					fclose(logfile[i]);
					logfile[i] = NULL;
				}
			}
		}
	}

	if (database->modified || sqlserver->modified)
	{
		modified = 1;

		mysql_close(&my_sock);

		if (!mysql_real_connect(&my_sock, sqlserver->string, dbuser->string, dbpasswd->string, database->string, 3306, NULL /*unix_socket*/, 0))
		{
			BPrintf(RADIO_YELLOW, "ERROR: Sys_SQL_Init(): Failed to connect to %s, Error %s \n", sqlserver->string, mysql_error(&my_sock));
		}
		else
		{
			Com_Printf(VERBOSE_ALWAYS, "MySQL connected successfully to %s:%s\n", sqlserver->string, database->string);
		}

		LoadDamageModel(NULL);
		LoadAmmo(NULL);
	}

	if(ttc->modified)
	{
		modified = 1;

		GetTonnageToClose(FALSE);
	}

	if (dirname->modified)
	{
		modified = 1;

		arena->mapnum = -1;

		for (i = 0; i < MAX_MAPCYCLE; i++)
		{

			if (!Com_Strcmp(dirname->string, arena->mapcycle[i].mapname))
			{
				arena->mapnum = i;

				date = (arena->year * 10000)+(arena->month * 100) + arena->day;

				//				if(arena->mapcycle[i].date > date)
				break;
			}
		}

		/*  this area was commented to get current date from config.cfg so we can change arena and start next arena with a gap date
		 if(mapcycle->value)
		 {
		 if(i > 0  && arena->mapcycle[i - 1].date)
		 {
		 arena->day = (arena->mapcycle[i - 1].date - ((arena->mapcycle[i - 1].date / 100) * 100));
		 arena->month = (arena->mapcycle[i - 1].date - ((arena->mapcycle[i - 1].date / 10000) * 10000)) / 100;
		 arena->year = arena->mapcycle[i - 1].date / 10000;
		 }
		 else if(!i)
		 {
		 arena->day = initday->value;
		 arena->month = initmonth->value;
		 arena->year = inityear->value;
		 }
		 }
		 */
	}

	if (countrytime->modified)
	{
		modified = 1;

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				clients[i].countrytime = time(NULL) + countrytime->value;
			}
		}
	}

	if (hideadmin->modified)
	{
		modified = 1;

		date = 0;

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].attr == 1 && hideadmin->value)
				continue;

			if (clients[i].inuse && !clients[i].drone)
				date++;
		}

		arena->numplayers = date;

		UpdateIngameClients(0);
	}

	if (server_speeds->modified)
	{
		modified = 1;

		if (server_speeds->value >= 100)
		{
			Var_Set("server_speeds", "99");
		}

		if (server_speeds->value < 0)
		{
			Var_Set("server_speeds", "0");
		}
	}

	if (timemult->modified)
	{
		modified = 1;

		if (timemult->value >= 6000)
		{
			Var_Set("timemult", "5999");
		}

		if (timemult->value < 0)
		{
			Var_Set("timemult", "0");
		}

		CalcTimemultBasedOnTime();
	}

	if (wb3->value && weather->modified)
	{
		modified = 1;

		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				if(clients[i].thai) // CheckVars
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				if((weather->value == 2) && !clients[i].rain)
					WB3DotCommand(&clients[i], ".weather 0"); // cloudy
				else
					WB3DotCommand(&clients[i], ".weather %u", (u_int8_t)weather->value);
			}
		}
	}

	if (wb3->value && (gruntsmaxd->modified || gruntshoot->modified || gruntcapture->modified))
	{
		modified = 1;

		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				if(clients[i].thai) // CheckVars
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				WB3SendGruntConfig(&clients[i]);
			}
		}
	}

	if (radaralt->modified || mapflags->modified || mapflagsfly->modified || mapflagstwr->modified || mapflagsown->modified || mapflagsenemy->modified || planeatradar->modified
			|| friendlydotsfly->modified || enemydotsfly->modified || friendlydotstwr->modified || enemydotstwr->modified || planerangelimit->modified || enemyidlim->modified
			|| friendlyidlim->modified || ammomult->modified || xwindvelocity->modified || ywindvelocity->modified || zwindvelocity->modified || maxpilotg->modified || altv->modified
			|| fueldiv->modified || flakmax->modified || radarrange0->modified || radarrange1->modified || radarrange2->modified || radarrange3->modified || radarrange4->modified
			|| structlim->modified || enemyidlimbomber->modified || friendlyidlimbomber->modified || planerangelimitbomber->modified || arenaflags1->modified || arenaflags2->modified
			|| ackgrowco->modified || ackshrinkco->modified || ackmaxtrav->modified)
	{
		modified = 1;

		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				if(clients[i].thai) // CheckVars
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				SendArenaRules(&clients[i]);
			}
		}
	}

	if (arenaflags3->modified)
	{
		modified = 1;

		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				if(clients[i].thai) // CheckVars
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				WB3SendArenaFlags3(&clients[i]);
			}
		}
	}

	UpdateArenaStatus(FALSE);

	if(modified)
	{
		for (var = var_vars; var; var = var->next)
		{
			if (var->modified)
			{
				var->modified = 0;
			}
		}
	}
}

/**
 Var_SetFlags

 Set Flags for an existing variable
 */

var_t *Var_SetFlags(char *var_name, int flags)
{
	var_t *var;

	var = Var_FindVar(var_name);

	if (var)
	{
		var->flags = flags;
		return var;
	}

	return NULL;
}


/**
 Var_Get

 Put a new variable at vars array or returns it if already exist
 */

var_t *Var_Get(char *var_name, char *var_value, char *min, char *max, char *description, int flags)
{
	var_t *var;

	var = Var_FindVar(var_name);
	if (var)
	{
		return var;
	}

	if (!var_value)
		return NULL;

	var = Z_Malloc(sizeof(*var));
	var->name = CopyString(var_name);
	var->min = Com_Atof(min);
	var->max = Com_Atof(max);
	var->string = CopyString(var_value);
	var->value = Com_Atof(var->string);
	var->modified = 1;
	var->description = CopyString(description);

	// link the variable in
	var->next = var_vars;

	if (var)
	{
		var_vars = var;
	}
	else
		Com_Printf(VERBOSE_WARNING, "Var_Get(): var == NULL\n");

	var->flags = flags;

	return var;
}

/**
 Var_FindVar

 Find a variable at vars array and return a pointed to it
 */

var_t *Var_FindVar(char *var_name)
{
	var_t *var= NULL;

	for (var = var_vars; var; var = var->next)
	{
		if (!Com_Strcmp(var_name, var->name))
			return var;
	}

	return NULL;
}

/**
 Var_Set

 Change a variable value or creates a new one if it doesn't exists
 */

var_t *Var_Set(char *var_name, char *value)
{
	var_t *var;

	var = Var_FindVar(var_name);
	if (!var)
	{ // create it
		return Var_Get(var_name, value, "0", "0", NULL, 0);
	}

	if (var->flags & VAR_NOSET)
		return NULL;

	if (!Com_Strcmp(value, var->string))
		return var; // not changed

	if(var->min != var->max && (Com_Atof(value) > var->max || Com_Atof(value) < var->min))
	{
		return NULL;
	}
	else
	{
		var->modified = 1;
		free(var->string);
		var->string = CopyString(value);
		var->value = Com_Atof(var->string);
	}

	return var;
}

/**
 Var_VariableValue

 Searches the variable at vars array and return its value
 OBS: do not use it indiscriminately, may cause overprocessing
 */

double Var_VariableValue(char *var_name)
{
	var_t *var;

	var = Var_FindVar(var_name);
	if (!var)
		return 0;
	return Com_Atof(var->string);
}

/**
 Var_VariableString

 Searches the variable at vars array and return its string value
 OBS: do not use it indiscriminately, may cause overprocessing
 */

char *Var_VariableString(char *var_name)
{
	var_t *var;

	var = Var_FindVar(var_name);
	if (!var)
		return "";
	return var->string;
}

/**
 Var_WriteVariables

 Save all variables set with VAR_ARCHIVE flag in "path" file
 */

void Var_WriteVariables(char *path, client_t *client)
{
	var_t *var;
	char file[128];
	char buffer[MAX_PRINTMSG];
	FILE *fp;

	strcpy(file, path);
	strcat(file, ".cfg");

	if (!(fp = fopen(file, "w")))
	{
		PPrintf(client, RADIO_YELLOW, "WARNING: Error opening %s\n", path);
		return;
	}

	for (var = var_vars; var; var = var->next)
	{
		if (var->flags & VAR_ARCHIVE)
		{
			snprintf(buffer, sizeof(buffer), "set %s \"%s\"\n", var->name, var->string);
			fprintf(fp, "%s", buffer);
		}
	}
	fclose(fp);
}

/**
 UpdateArenaStatus

 Update table arena_status
 */

u_int8_t UpdateArenaStatus(u_int8_t uptime)
{
	u_int8_t i, j;
	var_t *upvars[51] =
	{ airshowsmoke, blackout, contrail, countrytime, currday, currmonth, curryear, cvcapture, cvdelay, cvradarrange1, cvradarrange3, cvrange, cvs, cvsalvo, cvspeed, dayhours, easymode,
			emulatecollision, enemyidlim, enemynames, enemyplanes, flakmax, flypenalty, friendlyfire, friendlyidlim, fueldiv, gunstats, iff, katyrange, mapname, midairs, mortars, mview, ottoaccuracy,
			ottoadjust, ottoburstoff, ottoburston, ottoburstonmax, ottorange, ottoretarget, ottooverrides, planerangelimit, radaralt, radarrange1, radarrange3, rebuildtime, rps,
			tanksrange, timemult, timeout, whitelist }; // TODO: remove paratroopers from DB

	sprintf(my_query, "UPDATE arena_status SET");

	for (i = 0, j = 0; i < 49; i++)
	{
		if (upvars[i]->modified)
		{
			sprintf(my_query, "%s %s = '%s',", my_query, upvars[i]->name, upvars[i]->string);
			j++;
		}
	}

	if (uptime || j)
	{
		sprintf(my_query, "%s uptime = '%ud %.2d:%.2d:%.2d'", my_query, arena->frame/8640000, (arena->frame/360000)%24, (arena->frame/6000)%60, (arena->frame/100)%60);

		if (d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			Com_Printf(VERBOSE_WARNING, "UpdateArenaStatus(): couldn't query UPDATE error %d: %s\nquery = %s\n", mysql_errno(&my_sock), mysql_error(&my_sock), my_query);
		}
	}

	return j; // return number of vars changed
}
