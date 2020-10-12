# CmakeLists for TrackerManagement

#k4a
#include_directories(${CMAKE_BINARY_DIR}/src ${K4A_INCLUDE_DIR})
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${K4A_LIBRARIES_DIR})
#k4abt
#include_directories(${CMAKE_BINARY_DIR}/src ${K4ABT_INCLUDE_DIR})
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${K4ABT_LIBRARIES_DIR})
#Cudnn
#include_directories(${CMAKE_BINARY_DIR}/src ${Cudnn_INCLUDE_DIR})
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${Cudnn_LIBRARIES_DIR})
#OSCPack
#include_directories(${CMAKE_BINARY_DIR}/src ${OSCp_INCLUDE_DIR})
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${OSCp_LIBRARIES_DIR})
#NatNetSDK
#include_directories(${CMAKE_BINARY_DIR}/src ${NatNetSDK_INCLUDE_DIR})
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${NatNetSDK_LIBRARIES_DIR})



#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
link_directories(${CMAKE_BINARY_DIR}/lib)



include_directories(${CMAKE_BINARY_DIR}/src 
${K4A_INCLUDE_DIR}
${K4ABT_INCLUDE_DIR}
${Cudnn_INCLUDE_DIR}
${NATNET_SDK_INCLUDE_DIR}
${EIGEN_INCLUDE_DIR}
${OSCp_INCLUDE_DIR}
${OpenCV_INCLUDE_DIRS}
)




link_directories(${CMAKE_BINARY_DIR}/lib
 "${CMAKE_BINARY_DIR}/lib"
${K4A_LIBRARIES_DIR}
${K4ABT_LIBRARIES_DIR}
${Cudnn_LIBRARIES_DIR}
${NATNET_SDK_LIBRARIES_DIR}
${OSCp_LIBRARIES_DIR}
)


file(GLOB_RECURSE TrackerManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE TrackerManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
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
	
	debug NetworkManagement${CMAKE_DEBUG_POSTFIX} optimized NetworkManagement
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	${OpenCV_LIBS}
)