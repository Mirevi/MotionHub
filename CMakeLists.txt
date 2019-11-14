# top level cmake file MireviMotionHub

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (MireviMotionHub)

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

#Util function for extracting the deps name from a given URL (e.g.: the
# funtion returns "qt-5920" from the URL http://mirevi.de/claymore_deps/qt-5920.zip)
function(getsTheDepsNameFromURL "${URL}")
	string(FIND "${URL}" / FILENAME_STRING_POS REVERSE) # searches for the last / and returns the position in the string
	math(EXPR FILENAME_STRING_POS "${FILENAME_STRING_POS} + 1") # increments the string position since it would otherwise contain the /
	string(LENGTH "${URL}" FILENAME_STRING_LENGTH) # get the length of the whole string
	math(EXPR LENGTH_OF_DEPS_NAME "${FILENAME_STRING_LENGTH} - ${FILENAME_STRING_POS} - 4") # calculates the length of the deps name 
	string(SUBSTRING "${URL}" "${FILENAME_STRING_POS}" "${LENGTH_OF_DEPS_NAME}" DEPS_NAME) # gets the file name as string
	#message(STATUS "LENGTH_OF_DEPS_NAME ist ${LENGTH_OF_DEPS_NAME}")
	#message(STATUS "CURRENT_DEPS_PATH ist ${URL}")
	#message(STATUS "FILENAME_STRING_POS ist ${FILENAME_STRING_POS}")
	#message(STATUS "FILENAME_STRING_LENGTH ist ${FILENAME_STRING_LENGTH}")
	#message(STATUS "AUSGABE ist ${DEPS_NAME}")
	set(DEPS_NAME "${DEPS_NAME}" PARENT_SCOPE) #set the DEPS_NAME for the upper scope from where the function was called with PARENT_SCOPE
endfunction(getsTheDepsNameFromURL)

# CMake Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
FIND_PACKAGE( OpenSceneGraph REQUIRED ) # Cmake will look for cmake scripts in /cmake/Modules/. This examples searches "FindOpenSceneGraph.cmake"
FIND_PACKAGE( AzureKinectBodyTrackingSDK REQUIRED )
FIND_PACKAGE( AzureKinectSensorSDK REQUIRED )
FIND_PACKAGE( OSCPack REQUIRED )
FIND_PACKAGE( Cudnn REQUIRED )

#QT5ForMireviMotionHub -> downloads dependency from web
set(EXPECTED_SHA256 60ba1f48a17c9156fbe70d25c1bc65df9cf4a1d93d172d115ac85253e23d9c2d )
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/qt-5920/)
set(URL http://mirevi.de/claymore_deps/qt-5920.zip)
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/QtCore/5.9.2/QtCore/private/qcoreapplication_p.h")
FIND_PACKAGE( Qt5ForMireviMotionHub REQUIRED )

# NatNetSDK (NaturalPoint Optitrack Tracking SDK - Streaming over network)
set( EXPECTED_SHA256 29c71ebbc2374f4ddfb5af2fa45dd1c4c0def076c4a595ab3b046192bd86fd5d ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/netnatsdk-2100/)  # TODO: Does not exist yet
set( URL https://claymore.mirevi.com/netnatsdk-2100.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/NatNetClient.h")
FIND_PACKAGE( NatNetSDK REQUIRED )

# Add subdir with additional CMakeLists.txt and files
add_subdirectory("${CMAKE_SOURCE_DIR}/src/MireviMotionHub") # CMake look for further CMakeLists.txt in that directory
add_subdirectory("${CMAKE_SOURCE_DIR}/src/TrackerManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/NetworkManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/RenderManagement")
# add_subdirectory("${CMAKE_SOURCE_DIR}/src/3DRenderManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/GestureManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/MotionHubUtil")