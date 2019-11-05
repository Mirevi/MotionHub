# CmakeLists for UIManagement

#k4a
include_directories(${CMAKE_BINARY_DIR}/src ${K4A_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${K4A_LIBRARIES_DIR})
#k4abt
include_directories(${CMAKE_BINARY_DIR}/src ${K4ABT_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${K4ABT_LIBRARIES_DIR})
#Cudnn
include_directories(${CMAKE_BINARY_DIR}/src ${Cudnn_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib "${CMAKE_BINARY_DIR}/lib" ${Cudnn_LIBRARIES_DIR})

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

file(GLOB_RECURSE UIManagement_HEADER "${CMAKE_CURRENT_SRC_DIR}" *.h)
file(GLOB_RECURSE UIManagement_SRC "${CMAKE_CURRENT_SRC_DIR}" *.cpp)
file(GLOB_RECURSE UIManagement_QRC "${CMAKE_CURRENT_SRC_DIR}" *.qrc)
file(GLOB_RECURSE UIManagement_UI "${CMAKE_CURRENT_SRC_DIR}" *.ui)
add_library(UIManagement SHARED ${UIManagement_HEADER} ${UIManagement_SRC} ${UIManagement_QRC} ${UIManagement_UI})

target_link_libraries(UIManagement
	Qt5::Widgets
	debug MotionHubUtil${CMAKE_DEBUG_POSTFIX} optimized MotionHubUtil
	debug TrackerManagement${CMAKE_DEBUG_POSTFIX} optimized TrackerManagement
	debug k4a optimized k4a
	debug k4abt optimized k4abt
	debug cudnn optimized cudnn
	#debug Qt5Core${CMAKE_DEBUG_POSTFIX} optimized Qt5Core
	#debug Qt5Widgets${CMAKE_DEBUG_POSTFIX} optimized Qt5Widgets
	
)