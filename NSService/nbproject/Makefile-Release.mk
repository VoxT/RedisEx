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
CND_PLATFORM=GNU-Linux
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
	${OBJECTDIR}/ZApiExceptionHandler.o \
	${OBJECTDIR}/ZApiMsgHandler.o \
	${OBJECTDIR}/ZApiUtil.o \
	${OBJECTDIR}/ZConfigReader.o \
	${OBJECTDIR}/ZHandlerFactory.o \
	${OBJECTDIR}/ZHttpServer.o \
	${OBJECTDIR}/ZLogUtil.o \
	${OBJECTDIR}/ZRedisUtil.o \
	${OBJECTDIR}/inireader/INIReader.o \
	${OBJECTDIR}/inireader/ini.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++1y -static-libgcc -static-libstdc++
CXXFLAGS=-std=c++1y -static-libgcc -static-libstdc++

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/usr/local/lib/libPocoNet.a /usr/local/lib/libPocoUtil.a /usr/local/lib/libPocoJSON.a /usr/local/lib/libPocoXML.a /usr/local/lib/libPocoData.a /usr/local/lib/libPocoFoundation.a /usr/local/lib/libPocoCrypto.a /usr/local/lib/libPocoNetSSL.a -lpthread /usr/local/lib/libhiredis.a /usr/lib/x86_64-linux-gnu/librt.a /usr/lib/x86_64-linux-gnu/libz.a /usr/lib/x86_64-linux-gnu/libssl.a /usr/lib/x86_64-linux-gnu/libcrypto.a -ldl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoNet.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoUtil.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoJSON.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoXML.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoData.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoFoundation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoCrypto.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libPocoNetSSL.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/local/lib/libhiredis.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/lib/x86_64-linux-gnu/librt.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/lib/x86_64-linux-gnu/libz.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/lib/x86_64-linux-gnu/libssl.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: /usr/lib/x86_64-linux-gnu/libcrypto.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice ${OBJECTFILES} ${LDLIBSOPTIONS} -s

${OBJECTDIR}/ZApiExceptionHandler.o: ZApiExceptionHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZApiExceptionHandler.o ZApiExceptionHandler.cpp

${OBJECTDIR}/ZApiMsgHandler.o: ZApiMsgHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZApiMsgHandler.o ZApiMsgHandler.cpp

${OBJECTDIR}/ZApiUtil.o: ZApiUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZApiUtil.o ZApiUtil.cpp

${OBJECTDIR}/ZConfigReader.o: ZConfigReader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZConfigReader.o ZConfigReader.cpp

${OBJECTDIR}/ZHandlerFactory.o: ZHandlerFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZHandlerFactory.o ZHandlerFactory.cpp

${OBJECTDIR}/ZHttpServer.o: ZHttpServer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZHttpServer.o ZHttpServer.cpp

${OBJECTDIR}/ZLogUtil.o: ZLogUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZLogUtil.o ZLogUtil.cpp

${OBJECTDIR}/ZRedisUtil.o: ZRedisUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZRedisUtil.o ZRedisUtil.cpp

${OBJECTDIR}/inireader/INIReader.o: inireader/INIReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/inireader
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inireader/INIReader.o inireader/INIReader.cpp

${OBJECTDIR}/inireader/ini.o: inireader/ini.c 
	${MKDIR} -p ${OBJECTDIR}/inireader
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inireader/ini.o inireader/ini.c

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DWRITE_LOG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nsservice

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
