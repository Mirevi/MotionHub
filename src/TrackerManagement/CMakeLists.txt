# CmakeLists for TrackerManagement

if(BUILD_WITH_MULTI_PROCESSOR_COMPILER)
	include(ProcessorCount)
	ProcessorCount(N)
	if(NOT N EQUAL 0)
		SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP${N} ")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
	endif()
endif()

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil 
${CMAKE_BINARY_DIR}/src/RenderManagement)
#include internal library pathes
link_directories(${CMAKE_BINARY_DIR}/lib)

#include external source files
include_directories(${CMAKE_BINARY_DIR}/src 
${K4A_INCLUDE_DIR}
${K4ABT_INCLUDE_DIR}
${Cudnn_INCLUDE_DIR}
${NATNET_SDK_INCLUDE_DIR}
${EIGEN_INCLUDE_DIR}
${OSCp_INCLUDE_DIR}
${OSG_INCLUDE_DIR}
${XsensMVNSDK_INCLUDE_DIR}
${OpenVR_INCLUDE_DIR}
)

#include external library pathes
link_directories(${CMAKE_BINARY_DIR}/lib
 "${CMAKE_BINARY_DIR}/lib"
${K4A_LIBRARIES_DIR}
${K4ABT_LIBRARIES_DIR}
${Cudnn_LIBRARIES_DIR}
${NATNET_SDK_LIBRARIES_DIR}
${OSCp_LIBRARIES_DIR}
${OSG_LIBRARIES_DIR}
${XsensMVNSDK_LIBRARIES_DIR}
${OpenVR_LIBRARIES_DIR}
)

file(GLOB_RECURSE TrackerManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE TrackerManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)

file(GLOB_RECURSE CAPTURY_FILES ${CMAKE_CURRENT_SRC_DIR} PublicStructs.h)
source_group("captury" FILES ${CAPTURY_FILES})

file(GLOB_RECURSE XSENS_FILES ${CMAKE_CURRENT_SRC_DIR} xs_*.h xs_*.cpp)
source_group("xsens" FILES ${XSENS_FILES})

file(GLOB_RECURSE OPENVR_FILES ${CMAKE_CURRENT_SRC_DIR} OpenVR*.h OpenVR*.cpp)
source_group("OpenVR" FILES ${OPENVR_FILES})

add_library(TrackerManagement SHARED ${TrackerManagement_HEADER} ${TrackerManagement_SRC})

target_link_libraries(TrackerManagement
    debug k4a optimized k4a
	debug k4abt optimized k4abt
	debug cudnn optimized cudnn
	debug Ws2_32 optimized Ws2_32
	debug winmm optimized winmm
    debug NatNetLibStatic optimized NatNetLibStatic
    debug legacy_stdio_definitions optimized legacy_stdio_definitions
	debug oscpack${CMAKE_DEBUG_POSTFIX} optimized oscpack
	debug xstypes64 optimized xstypes64
	debug openvr_api optimized openvr_api
	
	debug osg${CMAKE_DEBUG_POSTFIX} optimized osg
	debug osgDB${CMAKE_DEBUG_POSTFIX} optimized osgDB
	debug osgGA${CMAKE_DEBUG_POSTFIX} optimized osgGA
	debug osgFX${CMAKE_DEBUG_POSTFIX} optimized osgFX
	debug osgText${CMAKE_DEBUG_POSTFIX} optimized osgText
	debug osgUtil${CMAKE_DEBUG_POSTFIX} optimized osgUtil
	debug osgViewer${CMAKE_DEBUG_POSTFIX} optimized osgViewer
	debug osgShadow${CMAKE_DEBUG_POSTFIX} optimized osgShadow
	debug OpenThreads${CMAKE_DEBUG_POSTFIX} optimized OpenThreads
	debug osgText${CMAKE_DEBUG_POSTFIX} optimized osgText
	
	debug NetworkManagement${CMAKE_DEBUG_POSTFIX} optimized NetworkManagement
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
)