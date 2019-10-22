# CmakeLists for 3DRenderManagement

#osg
include_directories(${CMAKE_BINARY_DIR}/src ${OSG_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${OSG_LIBRARIES_DIR})

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
link_directories(${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE 3DRenderManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE 3DRenderManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_library(3DRenderManagement SHARED ${3DRenderManagement_HEADER} ${3DRenderManagement_SRC})

target_link_libraries(3DRenderManagement
    debug osg${CMAKE_DEBUG_POSTFIX} optimized osg
    debug osgDB${CMAKE_DEBUG_POSTFIX} optimized osgDB
    debug osgGA${CMAKE_DEBUG_POSTFIX} optimized osgGA
    debug osgUtil${CMAKE_DEBUG_POSTFIX} optimized osgUtil
    debug osgViewer${CMAKE_DEBUG_POSTFIX} optimized osgViewer
    debug OpenThreads${CMAKE_DEBUG_POSTFIX} optimized OpenThreads

	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil	
)