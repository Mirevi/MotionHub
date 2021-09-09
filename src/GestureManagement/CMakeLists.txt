# CmakeLists for GestureManagement

#include source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
link_directories(${CMAKE_BINARY_DIR}/lib)

#include source files
include_directories(${CMAKE_BINARY_DIR}/src 
${EIGEN_INCLUDE_DIR}
${OSG_INCLUDE_DIR})

#include  library pathes
link_directories(
${OSG_LIBRARIES_DIR})

file(GLOB_RECURSE GestureManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE GestureManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_library(GestureManagement SHARED ${GestureManagement_HEADER} ${GestureManagement_SRC})

target_link_libraries(GestureManagement
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	
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