# CmakeLists for MireviMotionHub

include_directories(${CMAKE_BINARY_DIR}/src 
${K4A_INCLUDE_DIR}
${K4ABT_INCLUDE_DIR}
${Cudnn_INCLUDE_DIR}
${OSCp_INCLUDE_DIR}
${Qt5ForMireviMotionHub_INCLUDE_DIR}
${NATNET_SDK_INCLUDE_DIR}
${EIGEN_INCLUDE_DIR}
${XsensMVNSDK_INCLUDE_DIR}
${OSG_INCLUDE_DIR}
${OpenVR_INCLUDE_DIR}
)

link_directories(${CMAKE_BINARY_DIR}/lib
"${CMAKE_BINARY_DIR}/lib"
${K4A_LIBRARIES_DIR}
${K4ABT_LIBRARIES_DIR}
${Cudnn_LIBRARIES_DIR}
${OSCp_LIBRARIES_DIR}
${Qt5ForMireviMotionHub_LIBRARIES_DIR}
${NATNET_SDK_LIBRARIES_DIR}
${XsensMVNSDK_LIBRARIES_DIR}
${OSG_LIBRARIES_DIR}
${OpenVR_LIBRARIES_DIR}
)
#XsensMVNSDK
include_directories(${CMAKE_BINARY_DIR}/src ${XsensMVNSDK_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${XsensMVNSDK_LIBRARIES_DIR})

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil)
include_directories(${CMAKE_BINARY_DIR}/src/NetworkManagement)
include_directories(${CMAKE_BINARY_DIR}/src/TrackerManagement)
include_directories(${CMAKE_BINARY_DIR}/src/RenderManagement)
include_directories(${CMAKE_BINARY_DIR}/src/3DRenderManagement)
include_directories(${CMAKE_BINARY_DIR}/src/GestureManagement)
link_directories(${CMAKE_BINARY_DIR}/lib)

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

	Qt5::Core
	Qt5::Widgets
	Qt5::OpenGL
	
    debug NatNetLibStatic optimized NatNetLibStatic
	debug xstypes64 optimized xstypes64
	
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	debug TrackerManagement${CMAKE_DEBUG_POSTFIX} optimized TrackerManagement
	debug GestureManagement${CMAKE_DEBUG_POSTFIX} optimized GestureManagement
	debug RenderManagement${CMAKE_DEBUG_POSTFIX} optimized RenderManagement
	debug NetworkManagement${CMAKE_DEBUG_POSTFIX} optimized NetworkManagement

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
	debug osgQOpenGL${CMAKE_DEBUG_POSTFIX} optimized osgQOpenGL
	debug osgQt5${CMAKE_DEBUG_POSTFIX} optimized osgQt5
	debug openvr_api optimized openvr_api
)

set_target_properties(MireviMotionHub PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")