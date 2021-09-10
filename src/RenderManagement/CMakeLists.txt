# CmakeLists for RenderManagement

if(BUILD_WITH_MULTI_PROCESSOR_COMPILER)
	include(ProcessorCount)
	ProcessorCount(N)
	if(NOT N EQUAL 0)
		SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP${N} ")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
		SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP${N}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N}")
	endif()
endif()

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil ${Qt5ForMireviMotionHub_INCLUDE_DIR})
#include internal library pathes
link_directories(${CMAKE_BINARY_DIR}/lib ${Qt5ForMireviMotionHub_LIBRARIES_DIR})

#include source files
include_directories(${CMAKE_BINARY_DIR}/src
${K4A_INCLUDE_DIR}
${K4ABT_INCLUDE_DIR}
${Cudnn_INCLUDE_DIR}
${NATNET_SDK_INCLUDE_DIR}
${OSCp_INCLUDE_DIR}
${EIGEN_INCLUDE_DIR}
${XsensMVNSDK_INCLUDE_DIR}
${OSG_INCLUDE_DIR}
${Qt5ForMireviMotionHub_INCLUDE_DIR}
${OpenVR_INCLUDE_DIR}
)

#include library pathes
link_directories(${CMAKE_BINARY_DIR}/lib
 "${CMAKE_BINARY_DIR}/lib"
${K4A_LIBRARIES_DIR}
${K4ABT_LIBRARIES_DIR}
${Cudnn_LIBRARIES_DIR}
${NATNET_SDK_LIBRARIES_DIR}
${OSCp_LIBRARIES_DIR}
${OSG_LIBRARIES_DIR}
${XsensMVNSDK_LIBRARIES_DIR}
${OpenVR_LIBRARIES_DIR}
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run QT MOC automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instructs Cmake/VisualS Studio to add a extra folder for the MOC files
#set(AUTOGEN_TARGETS_FOLDER ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE RenderManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE RenderManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
file(GLOB_RECURSE RenderManagement_QRC "${CMAKE_CURRENT_SRC_DIR}" *.qrc)
file(GLOB_RECURSE RenderManagement_UI "${CMAKE_CURRENT_SRC_DIR}" *.ui)
add_library(RenderManagement SHARED ${RenderManagement_HEADER} ${RenderManagement_SRC} ${RenderManagement_QRC} ${RenderManagement_UI})

target_link_libraries(RenderManagement
	Qt5::Core
	Qt5::Widgets
	Qt5::OpenGL
	
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	debug TrackerManagement${CMAKE_DEBUG_POSTFIX} optimized TrackerManagement
	debug NetworkManagement${CMAKE_DEBUG_POSTFIX} optimized NetworkManagement

	debug k4a optimized k4a
	debug k4abt optimized k4abt
	debug cudnn optimized cudnn
	debug NatNetLibStatic optimized NatNetLibStatic
	#debug Qt5Core${CMAKE_DEBUG_POSTFIX} optimized Qt5Core
	#debug Qt5Widgets${CMAKE_DEBUG_POSTFIX} optimized Qt5Widgets
	debug xstypes64 optimized xstypes64
	
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