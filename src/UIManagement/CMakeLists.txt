# CmakeLists for UIManagement

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
link_directories(${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE UIManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE UIManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_library(UIManagement SHARED ${UIManagement_HEADER} ${UIManagement_SRC})

target_link_libraries(UIManagement
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
)