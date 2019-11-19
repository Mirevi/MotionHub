# CmakeLists for NetworkManagement

#OSCpack
include_directories(${CMAKE_BINARY_DIR}/src ${OSCp_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${OSCp_LIBRARIES_DIR})

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
link_directories(${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE NetworkManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE NetworkManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_library(NetworkManagement SHARED ${NetworkManagement_HEADER} ${NetworkManagement_SRC})

target_link_libraries(NetworkManagement
	debug oscpack${CMAKE_DEBUG_POSTFIX} optimized oscpack
	debug Ws2_32 optimized Ws2_32
	debug winmm optimized winmm
	
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
)
