# CmakeLists for NetworkManagement

#OSCpack
include_directories(${CMAKE_BINARY_DIR}/src ${OSCp_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${OSCp_LIBRARIES_DIR})

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil
${OSG_INCLUDE_DIR})

link_directories(${CMAKE_BINARY_DIR}/lib
${OSG_LIBRARIES_DIR})

#eigen
include_directories(${CMAKE_BINARY_DIR}/src ${EIGEN_INCLUDE_DIR})

file(GLOB_RECURSE NetworkManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE NetworkManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_library(NetworkManagement SHARED ${NetworkManagement_HEADER} ${NetworkManagement_SRC})

target_link_libraries(NetworkManagement
	debug oscpack${CMAKE_DEBUG_POSTFIX} optimized oscpack
	debug Ws2_32 optimized Ws2_32
	debug winmm optimized winmm
	
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
	
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
)
