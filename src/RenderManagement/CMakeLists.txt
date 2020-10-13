# CmakeLists for RenderManagement

#k4a
include_directories(${CMAKE_BINARY_DIR}/src
${K4A_INCLUDE_DIR}
${K4ABT_INCLUDE_DIR}
${Cudnn_INCLUDE_DIR}
${NATNET_SDK_INCLUDE_DIR}
${OSCp_INCLUDE_DIR}
${EIGEN_INCLUDE_DIR}
${OpenCV_INCLUDE_DIRS}
)


#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" )
#k4abt
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" )
#Cudnn
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" )
#NatNetSDK
#link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" )

#eigen
link_directories(${CMAKE_BINARY_DIR}/lib
 "${CMAKE_BINARY_DIR}/lib"
${K4A_LIBRARIES_DIR}
${K4ABT_LIBRARIES_DIR}
${Cudnn_LIBRARIES_DIR}
${NATNET_SDK_LIBRARIES_DIR}
${OSCp_LIBRARIES_DIR}
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instructs Cmake/VisualS Studio to add a extra folder for the moc files
#set(AUTOGEN_TARGETS_FOLDER ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#include internal source files
include_directories(${CMAKE_BINARY_DIR}/src/MotionHubUtil ${Qt5ForMireviMotionHub_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib ${Qt5ForMireviMotionHub_LIBRARIES_DIR})

add_compile_definitions(NOMINMAX)

file(GLOB_RECURSE RenderManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE RenderManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
file(GLOB_RECURSE RenderManagement_QRC "${CMAKE_CURRENT_SRC_DIR}" *.qrc)
file(GLOB_RECURSE RenderManagement_UI "${CMAKE_CURRENT_SRC_DIR}" *.ui)
add_library(RenderManagement SHARED ${RenderManagement_HEADER} ${RenderManagement_SRC} ${RenderManagement_QRC} ${RenderManagement_UI})

target_link_libraries(RenderManagement
	Qt5::Widgets
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	debug TrackerManagement${CMAKE_DEBUG_POSTFIX} optimized TrackerManagement
	debug NetworkManagement${CMAKE_DEBUG_POSTFIX} optimized NetworkManagement

	
	debug k4a optimized k4a
	debug k4abt optimized k4abt
	debug cudnn optimized cudnn
	debug NatNetLibStatic optimized NatNetLibStatic
	#debug Qt5Core${CMAKE_DEBUG_POSTFIX} optimized Qt5Core
	#debug Qt5Widgets${CMAKE_DEBUG_POSTFIX} optimized Qt5Widgets
	${OpenCV_LIBS}
)