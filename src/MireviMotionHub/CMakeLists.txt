# CmakeLists for MireviMotionHub

#k4a
include_directories(${CMAKE_BINARY_DIR}/src ${K4A_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${K4A_LIBRARIES_DIR})
#k4abt
include_directories(${CMAKE_BINARY_DIR}/src ${K4ABT_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${K4ABT_LIBRARIES_DIR})
#Cudnn
include_directories(${CMAKE_BINARY_DIR}/src ${Cudnn_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${Cudnn_LIBRARIES_DIR})
#OSCpack
include_directories(${CMAKE_BINARY_DIR}/src ${OSCp_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${OSCp_LIBRARIES_DIR})
#Qt
include_directories(${CMAKE_BINARY_DIR}/src ${Qt5ForMireviMotionHub_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${Qt5ForMireviMotionHub_LIBRARIES_DIR})
#NatNetSDK
include_directories(${CMAKE_BINARY_DIR}/src ${NATNET_SDK_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${NATNET_SDK_LIBRARIES_DIR})
#OpenCV
include_directories(${CMAKE_BINARY_DIR}/src ${OpenCV_INCLUDE_DIRS})

#eigen
include_directories(${CMAKE_BINARY_DIR}/src ${EIGEN_INCLUDE_DIR})

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
include_directories(${CMAKE_BINARY_DIR}/src/NetworkManagement)
include_directories(${CMAKE_BINARY_DIR}/src/TrackerManagement)
include_directories(${CMAKE_BINARY_DIR}/src/RenderManagement)
include_directories(${CMAKE_BINARY_DIR}/src/3DRenderManagement)
include_directories(${CMAKE_BINARY_DIR}/src/GestureManagement)
link_directories(${CMAKE_BINARY_DIR}/lib)

add_compile_definitions(NOMINMAX)

file(GLOB_RECURSE MotionHub_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE MotionHub_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
add_executable(MireviMotionHub ${MotionHub_HEADER} ${MotionHub_SRC})

target_link_libraries(MireviMotionHub
    debug k4a optimized k4a
	debug k4abt optimized k4abt
	debug cudnn optimized cudnn
	debug oscpack${CMAKE_DEBUG_POSTFIX} optimized oscpack
	debug Ws2_32 optimized Ws2_32
	debug winmm optimized winmm		
	debug Qt5Core${CMAKE_DEBUG_POSTFIX} optimized Qt5Core
	debug Qt5Widgets${CMAKE_DEBUG_POSTFIX} optimized Qt5Widgets
    debug NatNetLibStatic optimized NatNetLibStatic
	
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	debug TrackerManagement${CMAKE_DEBUG_POSTFIX} optimized TrackerManagement
	debug GestureManagement${CMAKE_DEBUG_POSTFIX} optimized GestureManagement
	debug RenderManagement${CMAKE_DEBUG_POSTFIX} optimized RenderManagement
	debug NetworkManagement${CMAKE_DEBUG_POSTFIX} optimized NetworkManagement
	debug FaceSynthesizing${CMAKE_DEBUG_POSTFIX} optimized FaceSynthesizing
	${OpenCV_LIBS}

)
add_compile_definitions(NOMINMAX)
set_target_properties(MireviMotionHub PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")