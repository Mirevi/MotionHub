#   CmakeLists for CMakeOSGHelloWorld

include_directories(${CMAKE_BINARY_DIR}/src ${OSG_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${OSG_LIBRARIES_DIR})
file(GLOB_RECURSE OSGHELLOWORLD_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE OSGHELLOWORLD_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_executable(CMakeOSGHelloWorld ${OSGHELLOWORLD_HEADER} ${OSGHELLOWORLD_SRC})

set(CMAKE_DEBUG_POSTFIX "d")

target_link_libraries(CMakeOSGHelloWorld
    debug osg${CMAKE_DEBUG_POSTFIX} optimized osg
    debug osgDB${CMAKE_DEBUG_POSTFIX} optimized osgDB
    debug osgGA${CMAKE_DEBUG_POSTFIX} optimized osgGA
    debug osgUtil${CMAKE_DEBUG_POSTFIX} optimized osgUtil
    debug osgViewer${CMAKE_DEBUG_POSTFIX} optimized osgViewer
    debug OpenThreads${CMAKE_DEBUG_POSTFIX} optimized OpenThreads
	
)