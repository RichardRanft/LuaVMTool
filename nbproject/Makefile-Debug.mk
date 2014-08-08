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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/CApplication.o \
	${OBJECTDIR}/CVMInterface.o \
	${OBJECTDIR}/lua/lapi.o \
	${OBJECTDIR}/lua/lauxlib.o \
	${OBJECTDIR}/lua/lbaselib.o \
	${OBJECTDIR}/lua/lbitlib.o \
	${OBJECTDIR}/lua/lcode.o \
	${OBJECTDIR}/lua/lcorolib.o \
	${OBJECTDIR}/lua/lctype.o \
	${OBJECTDIR}/lua/ldblib.o \
	${OBJECTDIR}/lua/ldebug.o \
	${OBJECTDIR}/lua/ldo.o \
	${OBJECTDIR}/lua/ldump.o \
	${OBJECTDIR}/lua/lfunc.o \
	${OBJECTDIR}/lua/lgc.o \
	${OBJECTDIR}/lua/linit.o \
	${OBJECTDIR}/lua/liolib.o \
	${OBJECTDIR}/lua/llex.o \
	${OBJECTDIR}/lua/lmathlib.o \
	${OBJECTDIR}/lua/lmem.o \
	${OBJECTDIR}/lua/loadlib.o \
	${OBJECTDIR}/lua/lobject.o \
	${OBJECTDIR}/lua/lopcodes.o \
	${OBJECTDIR}/lua/loslib.o \
	${OBJECTDIR}/lua/lparser.o \
	${OBJECTDIR}/lua/lstate.o \
	${OBJECTDIR}/lua/lstring.o \
	${OBJECTDIR}/lua/lstrlib.o \
	${OBJECTDIR}/lua/ltable.o \
	${OBJECTDIR}/lua/ltablib.o \
	${OBJECTDIR}/lua/ltm.o \
	${OBJECTDIR}/lua/lua.o \
	${OBJECTDIR}/lua/luac.o \
	${OBJECTDIR}/lua/lundump.o \
	${OBJECTDIR}/lua/lvm.o \
	${OBJECTDIR}/lua/lzio.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib -L/usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/ -Wl,-rpath,/usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit -ldl /usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/libgvmomi-vix-1.13.2.so /usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/libvix.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vmtool

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vmtool: /usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/libgvmomi-vix-1.13.2.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vmtool: /usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/libvix.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vmtool: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vmtool ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/CApplication.o: CApplication.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CApplication.o CApplication.cpp

${OBJECTDIR}/CVMInterface.o: CVMInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CVMInterface.o CVMInterface.cpp

${OBJECTDIR}/lua/lapi.o: lua/lapi.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lapi.o lua/lapi.c

${OBJECTDIR}/lua/lauxlib.o: lua/lauxlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lauxlib.o lua/lauxlib.c

${OBJECTDIR}/lua/lbaselib.o: lua/lbaselib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lbaselib.o lua/lbaselib.c

${OBJECTDIR}/lua/lbitlib.o: lua/lbitlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lbitlib.o lua/lbitlib.c

${OBJECTDIR}/lua/lcode.o: lua/lcode.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lcode.o lua/lcode.c

${OBJECTDIR}/lua/lcorolib.o: lua/lcorolib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lcorolib.o lua/lcorolib.c

${OBJECTDIR}/lua/lctype.o: lua/lctype.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lctype.o lua/lctype.c

${OBJECTDIR}/lua/ldblib.o: lua/ldblib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ldblib.o lua/ldblib.c

${OBJECTDIR}/lua/ldebug.o: lua/ldebug.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ldebug.o lua/ldebug.c

${OBJECTDIR}/lua/ldo.o: lua/ldo.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ldo.o lua/ldo.c

${OBJECTDIR}/lua/ldump.o: lua/ldump.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ldump.o lua/ldump.c

${OBJECTDIR}/lua/lfunc.o: lua/lfunc.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lfunc.o lua/lfunc.c

${OBJECTDIR}/lua/lgc.o: lua/lgc.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lgc.o lua/lgc.c

${OBJECTDIR}/lua/linit.o: lua/linit.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/linit.o lua/linit.c

${OBJECTDIR}/lua/liolib.o: lua/liolib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/liolib.o lua/liolib.c

${OBJECTDIR}/lua/llex.o: lua/llex.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/llex.o lua/llex.c

${OBJECTDIR}/lua/lmathlib.o: lua/lmathlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lmathlib.o lua/lmathlib.c

${OBJECTDIR}/lua/lmem.o: lua/lmem.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lmem.o lua/lmem.c

${OBJECTDIR}/lua/loadlib.o: lua/loadlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/loadlib.o lua/loadlib.c

${OBJECTDIR}/lua/lobject.o: lua/lobject.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lobject.o lua/lobject.c

${OBJECTDIR}/lua/lopcodes.o: lua/lopcodes.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lopcodes.o lua/lopcodes.c

${OBJECTDIR}/lua/loslib.o: lua/loslib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/loslib.o lua/loslib.c

${OBJECTDIR}/lua/lparser.o: lua/lparser.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lparser.o lua/lparser.c

${OBJECTDIR}/lua/lstate.o: lua/lstate.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lstate.o lua/lstate.c

${OBJECTDIR}/lua/lstring.o: lua/lstring.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lstring.o lua/lstring.c

${OBJECTDIR}/lua/lstrlib.o: lua/lstrlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lstrlib.o lua/lstrlib.c

${OBJECTDIR}/lua/ltable.o: lua/ltable.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ltable.o lua/ltable.c

${OBJECTDIR}/lua/ltablib.o: lua/ltablib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ltablib.o lua/ltablib.c

${OBJECTDIR}/lua/ltm.o: lua/ltm.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/ltm.o lua/ltm.c

${OBJECTDIR}/lua/lua.o: lua/lua.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lua.o lua/lua.c

${OBJECTDIR}/lua/luac.o: lua/luac.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/luac.o lua/luac.c

${OBJECTDIR}/lua/lundump.o: lua/lundump.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lundump.o lua/lundump.c

${OBJECTDIR}/lua/lvm.o: lua/lvm.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lvm.o lua/lvm.c

${OBJECTDIR}/lua/lzio.o: lua/lzio.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lua/lzio.o lua/lzio.c

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -I/usr/include/vmware-vix -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

test.lua: scripts/test.lua 
	@echo Performing Custom Build Step
	ln -s test.lua ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/test.lua

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vmtool
	${RM} test.lua

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
