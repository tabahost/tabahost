#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Boid.o \
	${OBJECTDIR}/Plane.o \
	${OBJECTDIR}/Ship.o \
	${OBJECTDIR}/Vehicle.o \
	${OBJECTDIR}/arena.o \
	${OBJECTDIR}/client.o \
	${OBJECTDIR}/commands.o \
	${OBJECTDIR}/common.o \
	${OBJECTDIR}/drone.o \
	${OBJECTDIR}/game.o \
	${OBJECTDIR}/lua.o \
	${OBJECTDIR}/net.o \
	${OBJECTDIR}/scores.o \
	${OBJECTDIR}/sys.o \
	${OBJECTDIR}/vars.o \
	${OBJECTDIR}/wbserver.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -rdynamic -ffast-math -c -fmessage-length=0
CXXFLAGS=-m32 -rdynamic -ffast-math -c -fmessage-length=0

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lz -ldl -lmysqlclient -lGeoIP

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wbserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wbserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -m32 -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wbserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Boid.o: Boid.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Boid.o Boid.cpp

${OBJECTDIR}/Plane.o: Plane.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Plane.o Plane.cpp

${OBJECTDIR}/Ship.o: Ship.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Ship.o Ship.cpp

${OBJECTDIR}/Vehicle.o: Vehicle.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Vehicle.o Vehicle.cpp

${OBJECTDIR}/arena.o: arena.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/arena.o arena.cpp

${OBJECTDIR}/client.o: client.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/client.o client.cpp

${OBJECTDIR}/commands.o: commands.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/commands.o commands.cpp

${OBJECTDIR}/common.o: common.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/common.o common.cpp

${OBJECTDIR}/drone.o: drone.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/drone.o drone.cpp

${OBJECTDIR}/game.o: game.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/game.o game.cpp

${OBJECTDIR}/lua.o: lua.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua.o lua.cpp

${OBJECTDIR}/net.o: net.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/net.o net.cpp

${OBJECTDIR}/scores.o: scores.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/scores.o scores.cpp

${OBJECTDIR}/sys.o: sys.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/sys.o sys.cpp

${OBJECTDIR}/vars.o: vars.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/vars.o vars.cpp

${OBJECTDIR}/wbserver.o: wbserver.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -rdynamic -ffast-math -c -fmessage-length=0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/wbserver.o wbserver.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wbserver

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
