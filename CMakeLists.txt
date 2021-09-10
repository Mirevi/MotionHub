# top level cmake file MireviMotionHub

cmake_minimum_required (VERSION 3.0 FATAL_ERROR)
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
	
# Building with C++17 standard (mandadory)	
set(CMAKE_CXX_STANDARD 17)	
set(CMAKE_CXX_STANDARD_REQUIRED ON)	
set(CMAKE_CXX_EXTENSIONS OFF)

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
#FIND_PACKAGE( OpenSceneGraph REQUIRED ) # Cmake will look for cmake scripts in /cmake/Modules/. This examples searches "FindOpenSceneGraph.cmake"
#FIND_PACKAGE( AzureKinectBodyTrackingSDK REQUIRED )
#FIND_PACKAGE( AzureKinectSensorSDK REQUIRED )
#FIND_PACKAGE( OSCPack REQUIRED )
#FIND_PACKAGE( Cudnn REQUIRED )

####################################################################################################
###################################### HOW TO: PREPARE DEPS ########################################
####################################################################################################
# Only for developers who change the online dep files in this document:
# Check, if all files up to date of the zip
# Zip it
# Upload it to the server
# Create Hash: Power shell: "Get-FileHash XYZ.zip -Algorithm SHA256" 
# Put the hash to lower case: Shortcut in Visual Studio to lower case is CTRL + U for selected text
# Add new Hash (in lowercase) to this CMakeList
# Check if the URL is still ok (usually the version will be incremented)
# Check if the CHECK_IF_DEPS_EXIST path and file is correct
# Commit and push changes (don't forget to add FindXYForMireviMotionHub.cmake)
# At the End: Test and build it with cmake and Visual Studio
####################################################################################################

#QT5ForMireviMotionHub -> downloads dependency from web
cmake_policy(SET CMP0020 NEW) #supresses CMake Warnings to CMP2020
set(EXPECTED_SHA256 3e6ca0d8a030cbffd2e2896c478730b0b45468f6294e1c46c8023d6e5cd066cc )
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/qt-5.15.2/)
set(URL http://claymore.mirevi.com/qt-5.15.2.zip)
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/QtCore/5.15.2/QtCore/private/qcoreapplication_p.h")
FIND_PACKAGE( Qt5ForMireviMotionHub REQUIRED )

# OSG (How-to-pack OSG below)
set( EXPECTED_SHA256 87efd7368c42f424d2a0993fa370b059685452a9bb326ad0720dd78281797ea9 )
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/osg-3.6.5/)
set( URL https://claymore.mirevi.com/osg-3.6.5.zip)
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/osg/Object")
FIND_PACKAGE( OSGForMireviMotionHub REQUIRED )

# NatNetSDK (NaturalPoint Optitrack Tracking SDK - Streaming over network)
set( EXPECTED_SHA256 29c71ebbc2374f4ddfb5af2fa45dd1c4c0def076c4a595ab3b046192bd86fd5d ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/netnatsdk-2100/)
set( URL https://claymore.mirevi.com/netnatsdk-2100.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/NatNetClient.h")
FIND_PACKAGE( NatNetSDK REQUIRED )

# Eigen
set( EXPECTED_SHA256 7f41249a613fd47d2d5ad10d3ff1d01a1e47267ba531a083c2ad40cf4eae990c ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/eigen337/)  
set( URL https://claymore.mirevi.com/mirevimotionhub/eigen337.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/Dense")
FIND_PACKAGE( Eigen REQUIRED )


# AzureKinectSensorSDK
#file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../deps/azurekinectsensorsdk)
#set( EXPECTED_SHA256 c4936269fcd6fd52b726e504230aa3dff2db3d6eff30dc93227c8edc26bc9987 ) 
#set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/azurekinectsensorsdk/)
#set( URL https://claymore.mirevi.com/mirevimotionhub/azurekinectsensorsdk0130.zip )
#getsTheDepsNameFromURL("${URL}")
#set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/k4a/k4a.h")
SET(K4A_ROOT "C:/Program Files/Azure Kinect SDK v1.4.0" CACHE PATH "Location of K4A SDK Root Directory")
FIND_PACKAGE( AzureKinectSensorSDK REQUIRED )

# AzureKinectBodyTrackingSDK
#file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../deps/azurekinectbodytrackingsdk)
#set( EXPECTED_SHA256 a4205d3c9d54b202327ef5a31673b301981d7ec1fb672db2e85d238ab68fefcc ) 
#set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/azurekinectbodytrackingsdk/)  
#set( URL https://claymore.mirevi.com/mirevimotionhub/azurekinectbodytrackingsdk0101.zip )
#getsTheDepsNameFromURL("${URL}")
#set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/k4abt.h")
SET(K4ABT_ROOT "C:/Program Files/Azure Kinect Body Tracking SDK" CACHE PATH "Location of K4A Body Tracking SDK Root Directory")
FIND_PACKAGE( AzureKinectBodyTrackingSDK REQUIRED )

# cudnn
set( EXPECTED_SHA256 0ed5de000420fdd6f4d36418e9ac6202ab3ae8a031712db624739ad4c58df4b3 ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/cudnn101windows10x64_76224/)
set( URL https://claymore.mirevi.com/mirevimotionhub/cudnn101windows10x64_76224.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/cudnn.h")
FIND_PACKAGE( Cudnn REQUIRED )

# OSCpack
set( EXPECTED_SHA256 5582c525d2bddbb0eab709be9cb54ba4ef026b062c84ba511b0243af2963b871 ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/oscpack-1.1.0/) 
set( URL https://claymore.mirevi.com/mirevimotionhub/oscpack-1.1.0.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/OscTypes.h")
FIND_PACKAGE( OSCPack REQUIRED )

# xSens MVN development pack
set( EXPECTED_SHA256 16c8f756a41f7ed92243f0892350f725e5d0f7263724e997e59310e85669b413 ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/xsens-mvn-1.0.6/) 
set( URL https://claymore.mirevi.com/mirevimotionhub/xsens-mvn-1.0.6.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/include/xstypes.h")
FIND_PACKAGE( XsensMVNSDK REQUIRED )

# OpenVR
set( EXPECTED_SHA256 a090f06c07fcb80920ec5fcc0bb2315dd2e08b610c3099c43563eb9e912de4e5 ) 
set(CURRENT_DEPS_PATH ${CMAKE_BINARY_DIR}/../deps/openvr-1.14.15/) 
set( URL https://claymore.mirevi.com/mirevimotionhub/openvr-1.14.15.zip )
getsTheDepsNameFromURL("${URL}")
set(CHECK_IF_DEPS_EXIST "${CURRENT_DEPS_PATH}/headers/openvr.h")
FIND_PACKAGE( OpenVR REQUIRED )
file(COPY ${CURRENT_DEPS_PATH}/bin/win64/openvr_api.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${CURRENT_DEPS_PATH}/bin/win64/openvr_api.dll.sig DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${CURRENT_DEPS_PATH}/bin/win64/openvr_api.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)

option(BUILD_WITH_MULTI_PROCESSOR_COMPILER "Faster build but can lead to erros during build such as stops." ON)
if(BUILD_WITH_MULTI_PROCESSOR_COMPILER)
	include(ProcessorCount)
	ProcessorCount(N)
	message(STATUS "Multi Processor Compiler (/MP) for Visual Studio activated. Found ${N} logical cores.")
else()
	message(STATUS "Multi Processor Compiler (/MP) for Visual Studio deactivated.")
endif()

# Add subdir with additional CMakeLists.txt and files
add_subdirectory("${CMAKE_SOURCE_DIR}/src/MireviMotionHub") # CMake look for further CMakeLists.txt in that directory
add_subdirectory("${CMAKE_SOURCE_DIR}/src/TrackerManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/NetworkManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/RenderManagement")
# add_subdirectory("${CMAKE_SOURCE_DIR}/src/3DRenderManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/GestureManagement")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/MotionHubUtil")

message(STATUS "Copiyng files (.dll, .pdb, etc.) to bin")

# copy Qt dlls
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Widgetsd.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Widgets.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Guid.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Gui.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Cored.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Core.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5OpenGL.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5OpenGLd.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)

# copy qt platform dlls
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/platforms)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../plugins/platforms/qwindows.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/platforms)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../plugins/platforms/qwindowsd.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/platforms)

# copy pdbs
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Widgetsd.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Widgets.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Guid.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Gui.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Cored.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5Core.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5OpenGL.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../bin/Qt5OpenGLd.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../plugins/platforms/qwindows.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin/platforms)
file(COPY ${Qt5ForMireviMotionHub_INCLUDE_DIR}/../plugins/platforms/qwindowsd.pdb DESTINATION ${CMAKE_BINARY_DIR}/bin/platforms)

# copy misc dlls
file(COPY ${K4A_LIBRARIES_DIR}/../bin/k4a.dll							DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${K4A_LIBRARIES_DIR}/../bin/depthengine_2_0.dll 				DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${K4ABT_LIBRARIES_DIR}/../bin/k4abt.dll 						DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${K4ABT_LIBRARIES_DIR}/../bin/onnxruntime.dll 				DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${K4ABT_LIBRARIES_DIR}/../bin/dnn_model_2_0.onnx 				DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${K4ABT_INCLUDE_DIR}/../../tools/cublas64_100.dll				DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${K4ABT_INCLUDE_DIR}/../../tools/cudart64_100.dll				DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY deps/cudnn101windows10x64_76224/bin/cudnn64_7.dll 			DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY deps/xsens-mvn-1.0.6/bin/xstypes64.dll						DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${OSG_INCLUDE_DIR}/../bin/									DESTINATION ${CMAKE_BINARY_DIR}/bin FILES_MATCHING PATTERN "*.dll")
file(COPY ${OSG_INCLUDE_DIR}/../bin/									DESTINATION ${CMAKE_BINARY_DIR}/bin FILES_MATCHING PATTERN "*.pdb")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/data/)

set_target_properties(MireviMotionHub PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MireviMotionHub)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type (default RelWithDepInfo)" FORCE)

