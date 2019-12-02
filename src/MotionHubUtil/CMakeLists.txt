# CmakeLists for MotionHubUtil

file(GLOB_RECURSE MotionHubUtil_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE MotionHubUtil_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)

#eigen
include_directories(${CMAKE_BINARY_DIR}/src ${EIGEN_INCLUDE_DIR})


add_library(MotionHubUtil SHARED ${MotionHubUtil_HEADER} ${MotionHubUtil_SRC})

target_link_libraries(MotionHubUtil
)