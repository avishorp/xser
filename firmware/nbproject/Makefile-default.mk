#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/usb_descriptors.o ${OBJECTDIR}/config.o ${OBJECTDIR}/usb_stack/usb_device.o ${OBJECTDIR}/usb_stack/usb_function_cdc.o ${OBJECTDIR}/usb_stack/usb_function_hid.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/usb_descriptors.o.d ${OBJECTDIR}/config.o.d ${OBJECTDIR}/usb_stack/usb_device.o.d ${OBJECTDIR}/usb_stack/usb_function_cdc.o.d ${OBJECTDIR}/usb_stack/usb_function_hid.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/usb_descriptors.o ${OBJECTDIR}/config.o ${OBJECTDIR}/usb_stack/usb_device.o ${OBJECTDIR}/usb_stack/usb_function_cdc.o ${OBJECTDIR}/usb_stack/usb_function_hid.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F24K50
MP_PROCESSOR_OPTION_LD=18f24k50
MP_LINKER_DEBUG_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_descriptors.o: usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/usb_descriptors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_descriptors.o   usb_descriptors.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_descriptors.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_descriptors.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/config.o: config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/config.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/config.o   config.c 
	@${DEP_GEN} -d ${OBJECTDIR}/config.o 
	@${FIXDEPS} "${OBJECTDIR}/config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_stack/usb_device.o: usb_stack/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/usb_stack 
	@${RM} ${OBJECTDIR}/usb_stack/usb_device.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_stack/usb_device.o   usb_stack/usb_device.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_stack/usb_device.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_stack/usb_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_stack/usb_function_cdc.o: usb_stack/usb_function_cdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/usb_stack 
	@${RM} ${OBJECTDIR}/usb_stack/usb_function_cdc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_stack/usb_function_cdc.o   usb_stack/usb_function_cdc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_stack/usb_function_cdc.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_stack/usb_function_cdc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_stack/usb_function_hid.o: usb_stack/usb_function_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/usb_stack 
	@${RM} ${OBJECTDIR}/usb_stack/usb_function_hid.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_stack/usb_function_hid.o   usb_stack/usb_function_hid.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_stack/usb_function_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_stack/usb_function_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_descriptors.o: usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/usb_descriptors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_descriptors.o   usb_descriptors.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_descriptors.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_descriptors.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/config.o: config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/config.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/config.o   config.c 
	@${DEP_GEN} -d ${OBJECTDIR}/config.o 
	@${FIXDEPS} "${OBJECTDIR}/config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_stack/usb_device.o: usb_stack/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/usb_stack 
	@${RM} ${OBJECTDIR}/usb_stack/usb_device.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_stack/usb_device.o   usb_stack/usb_device.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_stack/usb_device.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_stack/usb_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_stack/usb_function_cdc.o: usb_stack/usb_function_cdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/usb_stack 
	@${RM} ${OBJECTDIR}/usb_stack/usb_function_cdc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_stack/usb_function_cdc.o   usb_stack/usb_function_cdc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_stack/usb_function_cdc.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_stack/usb_function_cdc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/usb_stack/usb_function_hid.o: usb_stack/usb_function_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/usb_stack 
	@${RM} ${OBJECTDIR}/usb_stack/usb_function_hid.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"usb_stack" -I"usb_stack/USB" -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/usb_stack/usb_function_hid.o   usb_stack/usb_function_hid.c 
	@${DEP_GEN} -d ${OBJECTDIR}/usb_stack/usb_function_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/usb_stack/usb_function_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
