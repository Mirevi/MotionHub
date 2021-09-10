# CmakeLists for MotionHubUtil

if(BUILD_WITH_MULTI_PROCESSOR_COMPILER)
	include(ProcessorCount)
	ProcessorCount(N)
	if(NOT N EQUAL 0)
		SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP${N} ")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
	endif()
endif()

file(GLOB_RECURSE MotionHubUtil_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE MotionHubUtil_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)

#include source files
include_directories(${CMAKE_BINARY_DIR}/src
${EIGEN_INCLUDE_DIR}
${OSG_INCLUDE_DIR})

#include library pathes
link_directories(${CMAKE_BINARY_DIR}/lib
${OSG_LIBRARIES_DIR})


file(GLOB_RECURSE IK_FILES ${CMAKE_CURRENT_SRC_DIR} IK*.h IK*.cpp)
source_group("IK" FILES ${IK_FILES})

add_library(MotionHubUtil SHARED ${MotionHubUtil_HEADER} ${MotionHubUtil_SRC})

target_link_libraries(MotionHubUtil
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
)