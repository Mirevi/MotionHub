# top level cmake file

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (MotionHub)

include_directories("${CMAKE_SOURCE_DIR}/src")

# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(CMAKE_DEBUG_POSTFIX "d")

# CMake Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
FIND_PACKAGE( OpenSceneGraph REQUIRED ) # Cmake will look for cmake scripts in /cmake/Modules/. This examples searches "FindOpenSceneGraph.cmake"
FIND_PACKAGE( AzureKinectBodyTrackingSDK REQUIRED )
FIND_PACKAGE( AzureKinectSensorSDK REQUIRED )
FIND_PACKAGE( OSCPack REQUIRED )
FIND_PACKAGE( Cudnn REQUIRED )
# Add subdir with additional CMakeLists.txt and files
add_subdirectory("${CMAKE_SOURCE_DIR}/src") # CMake look for further CMakeLists.txt in that directory

set_target_properties(MotionHub PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")