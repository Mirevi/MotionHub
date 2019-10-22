# CmakeLists for GestureManagement

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
link_directories(${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE GestureManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE GestureManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_library(GestureManagement SHARED ${GestureManagement_HEADER} ${GestureManagement_SRC})

target_link_libraries(GestureManagement
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
)