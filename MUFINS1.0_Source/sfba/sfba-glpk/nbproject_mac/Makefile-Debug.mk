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
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/DPA.o \
	${OBJECTDIR}/Driver.o \
	${OBJECTDIR}/Elements.o \
	${OBJECTDIR}/GFA.o \
	${OBJECTDIR}/GNI.o \
	${OBJECTDIR}/Matrix.o \
	${OBJECTDIR}/Model.o \
	${OBJECTDIR}/MtxLP.o \
	${OBJECTDIR}/RPN.o \
	${OBJECTDIR}/RecordParser.o \
	${OBJECTDIR}/SplitLP.o \
	${OBJECTDIR}/Text.o \
	${OBJECTDIR}/TxtParser.o \
	${OBJECTDIR}/glpk/amd_1.o \
	${OBJECTDIR}/glpk/amd_2.o \
	${OBJECTDIR}/glpk/amd_aat.o \
	${OBJECTDIR}/glpk/amd_control.o \
	${OBJECTDIR}/glpk/amd_defaults.o \
	${OBJECTDIR}/glpk/amd_dump.o \
	${OBJECTDIR}/glpk/amd_info.o \
	${OBJECTDIR}/glpk/amd_order.o \
	${OBJECTDIR}/glpk/amd_post_tree.o \
	${OBJECTDIR}/glpk/amd_postorder.o \
	${OBJECTDIR}/glpk/amd_preprocess.o \
	${OBJECTDIR}/glpk/amd_valid.o \
	${OBJECTDIR}/glpk/colamd.o \
	${OBJECTDIR}/glpk/glpapi01.o \
	${OBJECTDIR}/glpk/glpapi02.o \
	${OBJECTDIR}/glpk/glpapi03.o \
	${OBJECTDIR}/glpk/glpapi04.o \
	${OBJECTDIR}/glpk/glpapi05.o \
	${OBJECTDIR}/glpk/glpapi06.o \
	${OBJECTDIR}/glpk/glpapi07.o \
	${OBJECTDIR}/glpk/glpapi08.o \
	${OBJECTDIR}/glpk/glpapi09.o \
	${OBJECTDIR}/glpk/glpapi10.o \
	${OBJECTDIR}/glpk/glpapi11.o \
	${OBJECTDIR}/glpk/glpapi12.o \
	${OBJECTDIR}/glpk/glpapi13.o \
	${OBJECTDIR}/glpk/glpapi14.o \
	${OBJECTDIR}/glpk/glpapi15.o \
	${OBJECTDIR}/glpk/glpapi16.o \
	${OBJECTDIR}/glpk/glpapi17.o \
	${OBJECTDIR}/glpk/glpapi18.o \
	${OBJECTDIR}/glpk/glpapi19.o \
	${OBJECTDIR}/glpk/glpavl.o \
	${OBJECTDIR}/glpk/glpbfd.o \
	${OBJECTDIR}/glpk/glpbfx.o \
	${OBJECTDIR}/glpk/glpcpx.o \
	${OBJECTDIR}/glpk/glpdmp.o \
	${OBJECTDIR}/glpk/glpdmx.o \
	${OBJECTDIR}/glpk/glpenv01.o \
	${OBJECTDIR}/glpk/glpenv02.o \
	${OBJECTDIR}/glpk/glpenv03.o \
	${OBJECTDIR}/glpk/glpenv04.o \
	${OBJECTDIR}/glpk/glpenv05.o \
	${OBJECTDIR}/glpk/glpenv06.o \
	${OBJECTDIR}/glpk/glpenv07.o \
	${OBJECTDIR}/glpk/glpenv08.o \
	${OBJECTDIR}/glpk/glpfhv.o \
	${OBJECTDIR}/glpk/glpgmp.o \
	${OBJECTDIR}/glpk/glphbm.o \
	${OBJECTDIR}/glpk/glpini01.o \
	${OBJECTDIR}/glpk/glpini02.o \
	${OBJECTDIR}/glpk/glpios01.o \
	${OBJECTDIR}/glpk/glpios02.o \
	${OBJECTDIR}/glpk/glpios03.o \
	${OBJECTDIR}/glpk/glpios04.o \
	${OBJECTDIR}/glpk/glpios05.o \
	${OBJECTDIR}/glpk/glpios06.o \
	${OBJECTDIR}/glpk/glpios07.o \
	${OBJECTDIR}/glpk/glpios08.o \
	${OBJECTDIR}/glpk/glpios09.o \
	${OBJECTDIR}/glpk/glpios10.o \
	${OBJECTDIR}/glpk/glpios11.o \
	${OBJECTDIR}/glpk/glpios12.o \
	${OBJECTDIR}/glpk/glpipm.o \
	${OBJECTDIR}/glpk/glplib01.o \
	${OBJECTDIR}/glpk/glplib02.o \
	${OBJECTDIR}/glpk/glplib03.o \
	${OBJECTDIR}/glpk/glplpf.o \
	${OBJECTDIR}/glpk/glplpx01.o \
	${OBJECTDIR}/glpk/glplpx02.o \
	${OBJECTDIR}/glpk/glplpx03.o \
	${OBJECTDIR}/glpk/glpluf.o \
	${OBJECTDIR}/glpk/glplux.o \
	${OBJECTDIR}/glpk/glpmat.o \
	${OBJECTDIR}/glpk/glpmpl01.o \
	${OBJECTDIR}/glpk/glpmpl02.o \
	${OBJECTDIR}/glpk/glpmpl03.o \
	${OBJECTDIR}/glpk/glpmpl04.o \
	${OBJECTDIR}/glpk/glpmpl05.o \
	${OBJECTDIR}/glpk/glpmpl06.o \
	${OBJECTDIR}/glpk/glpmps.o \
	${OBJECTDIR}/glpk/glpnet01.o \
	${OBJECTDIR}/glpk/glpnet02.o \
	${OBJECTDIR}/glpk/glpnet03.o \
	${OBJECTDIR}/glpk/glpnet04.o \
	${OBJECTDIR}/glpk/glpnet05.o \
	${OBJECTDIR}/glpk/glpnet06.o \
	${OBJECTDIR}/glpk/glpnet07.o \
	${OBJECTDIR}/glpk/glpnet08.o \
	${OBJECTDIR}/glpk/glpnet09.o \
	${OBJECTDIR}/glpk/glpnpp01.o \
	${OBJECTDIR}/glpk/glpnpp02.o \
	${OBJECTDIR}/glpk/glpnpp03.o \
	${OBJECTDIR}/glpk/glpnpp04.o \
	${OBJECTDIR}/glpk/glpnpp05.o \
	${OBJECTDIR}/glpk/glpqmd.o \
	${OBJECTDIR}/glpk/glprgr.o \
	${OBJECTDIR}/glpk/glprng01.o \
	${OBJECTDIR}/glpk/glprng02.o \
	${OBJECTDIR}/glpk/glpscf.o \
	${OBJECTDIR}/glpk/glpscl.o \
	${OBJECTDIR}/glpk/glpsdf.o \
	${OBJECTDIR}/glpk/glpspm.o \
	${OBJECTDIR}/glpk/glpspx01.o \
	${OBJECTDIR}/glpk/glpspx02.o \
	${OBJECTDIR}/glpk/glpsql.o \
	${OBJECTDIR}/glpk/glpssx01.o \
	${OBJECTDIR}/glpk/glpssx02.o \
	${OBJECTDIR}/glpk/glptsp.o \
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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sfba-glpk

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sfba-glpk: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sfba-glpk ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/DPA.o: DPA.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DPA.o DPA.cpp

${OBJECTDIR}/Driver.o: Driver.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Driver.o Driver.cpp

${OBJECTDIR}/Elements.o: Elements.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Elements.o Elements.cpp

${OBJECTDIR}/GFA.o: GFA.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GFA.o GFA.cpp

${OBJECTDIR}/GNI.o: GNI.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GNI.o GNI.cpp

${OBJECTDIR}/Matrix.o: Matrix.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Matrix.o Matrix.cpp

${OBJECTDIR}/Model.o: Model.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Model.o Model.cpp

${OBJECTDIR}/MtxLP.o: MtxLP.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MtxLP.o MtxLP.cpp

${OBJECTDIR}/RPN.o: RPN.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RPN.o RPN.cpp

${OBJECTDIR}/RecordParser.o: RecordParser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RecordParser.o RecordParser.cpp

${OBJECTDIR}/SplitLP.o: SplitLP.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SplitLP.o SplitLP.cpp

${OBJECTDIR}/Text.o: Text.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Text.o Text.cpp

${OBJECTDIR}/TxtParser.o: TxtParser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TxtParser.o TxtParser.cpp

${OBJECTDIR}/glpk/amd_1.o: glpk/amd_1.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_1.o glpk/amd_1.c

${OBJECTDIR}/glpk/amd_2.o: glpk/amd_2.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_2.o glpk/amd_2.c

${OBJECTDIR}/glpk/amd_aat.o: glpk/amd_aat.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_aat.o glpk/amd_aat.c

${OBJECTDIR}/glpk/amd_control.o: glpk/amd_control.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_control.o glpk/amd_control.c

${OBJECTDIR}/glpk/amd_defaults.o: glpk/amd_defaults.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_defaults.o glpk/amd_defaults.c

${OBJECTDIR}/glpk/amd_dump.o: glpk/amd_dump.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_dump.o glpk/amd_dump.c

${OBJECTDIR}/glpk/amd_info.o: glpk/amd_info.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_info.o glpk/amd_info.c

${OBJECTDIR}/glpk/amd_order.o: glpk/amd_order.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_order.o glpk/amd_order.c

${OBJECTDIR}/glpk/amd_post_tree.o: glpk/amd_post_tree.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_post_tree.o glpk/amd_post_tree.c

${OBJECTDIR}/glpk/amd_postorder.o: glpk/amd_postorder.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_postorder.o glpk/amd_postorder.c

${OBJECTDIR}/glpk/amd_preprocess.o: glpk/amd_preprocess.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_preprocess.o glpk/amd_preprocess.c

${OBJECTDIR}/glpk/amd_valid.o: glpk/amd_valid.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/amd_valid.o glpk/amd_valid.c

${OBJECTDIR}/glpk/colamd.o: glpk/colamd.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/colamd.o glpk/colamd.c

${OBJECTDIR}/glpk/glpapi01.o: glpk/glpapi01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi01.o glpk/glpapi01.c

${OBJECTDIR}/glpk/glpapi02.o: glpk/glpapi02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi02.o glpk/glpapi02.c

${OBJECTDIR}/glpk/glpapi03.o: glpk/glpapi03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi03.o glpk/glpapi03.c

${OBJECTDIR}/glpk/glpapi04.o: glpk/glpapi04.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi04.o glpk/glpapi04.c

${OBJECTDIR}/glpk/glpapi05.o: glpk/glpapi05.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi05.o glpk/glpapi05.c

${OBJECTDIR}/glpk/glpapi06.o: glpk/glpapi06.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi06.o glpk/glpapi06.c

${OBJECTDIR}/glpk/glpapi07.o: glpk/glpapi07.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi07.o glpk/glpapi07.c

${OBJECTDIR}/glpk/glpapi08.o: glpk/glpapi08.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi08.o glpk/glpapi08.c

${OBJECTDIR}/glpk/glpapi09.o: glpk/glpapi09.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi09.o glpk/glpapi09.c

${OBJECTDIR}/glpk/glpapi10.o: glpk/glpapi10.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi10.o glpk/glpapi10.c

${OBJECTDIR}/glpk/glpapi11.o: glpk/glpapi11.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi11.o glpk/glpapi11.c

${OBJECTDIR}/glpk/glpapi12.o: glpk/glpapi12.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi12.o glpk/glpapi12.c

${OBJECTDIR}/glpk/glpapi13.o: glpk/glpapi13.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi13.o glpk/glpapi13.c

${OBJECTDIR}/glpk/glpapi14.o: glpk/glpapi14.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi14.o glpk/glpapi14.c

${OBJECTDIR}/glpk/glpapi15.o: glpk/glpapi15.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi15.o glpk/glpapi15.c

${OBJECTDIR}/glpk/glpapi16.o: glpk/glpapi16.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi16.o glpk/glpapi16.c

${OBJECTDIR}/glpk/glpapi17.o: glpk/glpapi17.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi17.o glpk/glpapi17.c

${OBJECTDIR}/glpk/glpapi18.o: glpk/glpapi18.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi18.o glpk/glpapi18.c

${OBJECTDIR}/glpk/glpapi19.o: glpk/glpapi19.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpapi19.o glpk/glpapi19.c

${OBJECTDIR}/glpk/glpavl.o: glpk/glpavl.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpavl.o glpk/glpavl.c

${OBJECTDIR}/glpk/glpbfd.o: glpk/glpbfd.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpbfd.o glpk/glpbfd.c

${OBJECTDIR}/glpk/glpbfx.o: glpk/glpbfx.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpbfx.o glpk/glpbfx.c

${OBJECTDIR}/glpk/glpcpx.o: glpk/glpcpx.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpcpx.o glpk/glpcpx.c

${OBJECTDIR}/glpk/glpdmp.o: glpk/glpdmp.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpdmp.o glpk/glpdmp.c

${OBJECTDIR}/glpk/glpdmx.o: glpk/glpdmx.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpdmx.o glpk/glpdmx.c

${OBJECTDIR}/glpk/glpenv01.o: glpk/glpenv01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv01.o glpk/glpenv01.c

${OBJECTDIR}/glpk/glpenv02.o: glpk/glpenv02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv02.o glpk/glpenv02.c

${OBJECTDIR}/glpk/glpenv03.o: glpk/glpenv03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv03.o glpk/glpenv03.c

${OBJECTDIR}/glpk/glpenv04.o: glpk/glpenv04.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv04.o glpk/glpenv04.c

${OBJECTDIR}/glpk/glpenv05.o: glpk/glpenv05.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv05.o glpk/glpenv05.c

${OBJECTDIR}/glpk/glpenv06.o: glpk/glpenv06.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv06.o glpk/glpenv06.c

${OBJECTDIR}/glpk/glpenv07.o: glpk/glpenv07.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv07.o glpk/glpenv07.c

${OBJECTDIR}/glpk/glpenv08.o: glpk/glpenv08.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpenv08.o glpk/glpenv08.c

${OBJECTDIR}/glpk/glpfhv.o: glpk/glpfhv.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpfhv.o glpk/glpfhv.c

${OBJECTDIR}/glpk/glpgmp.o: glpk/glpgmp.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpgmp.o glpk/glpgmp.c

${OBJECTDIR}/glpk/glphbm.o: glpk/glphbm.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glphbm.o glpk/glphbm.c

${OBJECTDIR}/glpk/glpini01.o: glpk/glpini01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpini01.o glpk/glpini01.c

${OBJECTDIR}/glpk/glpini02.o: glpk/glpini02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpini02.o glpk/glpini02.c

${OBJECTDIR}/glpk/glpios01.o: glpk/glpios01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios01.o glpk/glpios01.c

${OBJECTDIR}/glpk/glpios02.o: glpk/glpios02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios02.o glpk/glpios02.c

${OBJECTDIR}/glpk/glpios03.o: glpk/glpios03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios03.o glpk/glpios03.c

${OBJECTDIR}/glpk/glpios04.o: glpk/glpios04.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios04.o glpk/glpios04.c

${OBJECTDIR}/glpk/glpios05.o: glpk/glpios05.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios05.o glpk/glpios05.c

${OBJECTDIR}/glpk/glpios06.o: glpk/glpios06.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios06.o glpk/glpios06.c

${OBJECTDIR}/glpk/glpios07.o: glpk/glpios07.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios07.o glpk/glpios07.c

${OBJECTDIR}/glpk/glpios08.o: glpk/glpios08.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios08.o glpk/glpios08.c

${OBJECTDIR}/glpk/glpios09.o: glpk/glpios09.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios09.o glpk/glpios09.c

${OBJECTDIR}/glpk/glpios10.o: glpk/glpios10.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios10.o glpk/glpios10.c

${OBJECTDIR}/glpk/glpios11.o: glpk/glpios11.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios11.o glpk/glpios11.c

${OBJECTDIR}/glpk/glpios12.o: glpk/glpios12.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpios12.o glpk/glpios12.c

${OBJECTDIR}/glpk/glpipm.o: glpk/glpipm.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpipm.o glpk/glpipm.c

${OBJECTDIR}/glpk/glplib01.o: glpk/glplib01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplib01.o glpk/glplib01.c

${OBJECTDIR}/glpk/glplib02.o: glpk/glplib02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplib02.o glpk/glplib02.c

${OBJECTDIR}/glpk/glplib03.o: glpk/glplib03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplib03.o glpk/glplib03.c

${OBJECTDIR}/glpk/glplpf.o: glpk/glplpf.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplpf.o glpk/glplpf.c

${OBJECTDIR}/glpk/glplpx01.o: glpk/glplpx01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplpx01.o glpk/glplpx01.c

${OBJECTDIR}/glpk/glplpx02.o: glpk/glplpx02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplpx02.o glpk/glplpx02.c

${OBJECTDIR}/glpk/glplpx03.o: glpk/glplpx03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplpx03.o glpk/glplpx03.c

${OBJECTDIR}/glpk/glpluf.o: glpk/glpluf.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpluf.o glpk/glpluf.c

${OBJECTDIR}/glpk/glplux.o: glpk/glplux.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glplux.o glpk/glplux.c

${OBJECTDIR}/glpk/glpmat.o: glpk/glpmat.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmat.o glpk/glpmat.c

${OBJECTDIR}/glpk/glpmpl01.o: glpk/glpmpl01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmpl01.o glpk/glpmpl01.c

${OBJECTDIR}/glpk/glpmpl02.o: glpk/glpmpl02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmpl02.o glpk/glpmpl02.c

${OBJECTDIR}/glpk/glpmpl03.o: glpk/glpmpl03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmpl03.o glpk/glpmpl03.c

${OBJECTDIR}/glpk/glpmpl04.o: glpk/glpmpl04.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmpl04.o glpk/glpmpl04.c

${OBJECTDIR}/glpk/glpmpl05.o: glpk/glpmpl05.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmpl05.o glpk/glpmpl05.c

${OBJECTDIR}/glpk/glpmpl06.o: glpk/glpmpl06.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmpl06.o glpk/glpmpl06.c

${OBJECTDIR}/glpk/glpmps.o: glpk/glpmps.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpmps.o glpk/glpmps.c

${OBJECTDIR}/glpk/glpnet01.o: glpk/glpnet01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet01.o glpk/glpnet01.c

${OBJECTDIR}/glpk/glpnet02.o: glpk/glpnet02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet02.o glpk/glpnet02.c

${OBJECTDIR}/glpk/glpnet03.o: glpk/glpnet03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet03.o glpk/glpnet03.c

${OBJECTDIR}/glpk/glpnet04.o: glpk/glpnet04.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet04.o glpk/glpnet04.c

${OBJECTDIR}/glpk/glpnet05.o: glpk/glpnet05.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet05.o glpk/glpnet05.c

${OBJECTDIR}/glpk/glpnet06.o: glpk/glpnet06.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet06.o glpk/glpnet06.c

${OBJECTDIR}/glpk/glpnet07.o: glpk/glpnet07.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet07.o glpk/glpnet07.c

${OBJECTDIR}/glpk/glpnet08.o: glpk/glpnet08.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet08.o glpk/glpnet08.c

${OBJECTDIR}/glpk/glpnet09.o: glpk/glpnet09.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnet09.o glpk/glpnet09.c

${OBJECTDIR}/glpk/glpnpp01.o: glpk/glpnpp01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnpp01.o glpk/glpnpp01.c

${OBJECTDIR}/glpk/glpnpp02.o: glpk/glpnpp02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnpp02.o glpk/glpnpp02.c

${OBJECTDIR}/glpk/glpnpp03.o: glpk/glpnpp03.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnpp03.o glpk/glpnpp03.c

${OBJECTDIR}/glpk/glpnpp04.o: glpk/glpnpp04.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnpp04.o glpk/glpnpp04.c

${OBJECTDIR}/glpk/glpnpp05.o: glpk/glpnpp05.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpnpp05.o glpk/glpnpp05.c

${OBJECTDIR}/glpk/glpqmd.o: glpk/glpqmd.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpqmd.o glpk/glpqmd.c

${OBJECTDIR}/glpk/glprgr.o: glpk/glprgr.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glprgr.o glpk/glprgr.c

${OBJECTDIR}/glpk/glprng01.o: glpk/glprng01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glprng01.o glpk/glprng01.c

${OBJECTDIR}/glpk/glprng02.o: glpk/glprng02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glprng02.o glpk/glprng02.c

${OBJECTDIR}/glpk/glpscf.o: glpk/glpscf.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpscf.o glpk/glpscf.c

${OBJECTDIR}/glpk/glpscl.o: glpk/glpscl.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpscl.o glpk/glpscl.c

${OBJECTDIR}/glpk/glpsdf.o: glpk/glpsdf.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpsdf.o glpk/glpsdf.c

${OBJECTDIR}/glpk/glpspm.o: glpk/glpspm.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpspm.o glpk/glpspm.c

${OBJECTDIR}/glpk/glpspx01.o: glpk/glpspx01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpspx01.o glpk/glpspx01.c

${OBJECTDIR}/glpk/glpspx02.o: glpk/glpspx02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpspx02.o glpk/glpspx02.c

${OBJECTDIR}/glpk/glpsql.o: glpk/glpsql.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpsql.o glpk/glpsql.c

${OBJECTDIR}/glpk/glpssx01.o: glpk/glpssx01.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpssx01.o glpk/glpssx01.c

${OBJECTDIR}/glpk/glpssx02.o: glpk/glpssx02.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glpssx02.o glpk/glpssx02.c

${OBJECTDIR}/glpk/glptsp.o: glpk/glptsp.c 
	${MKDIR} -p ${OBJECTDIR}/glpk
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glpk/glptsp.o glpk/glptsp.c

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sfba-glpk

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
