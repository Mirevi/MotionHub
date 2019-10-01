IF (DEFINED ENV{K4A_PATH})
    SET(K4A_ROOT_DIR "$ENV{K4A_PATH}") # alles  $ENV{XY} sind envrionment variablen des Betriebssystems
ENDIF()
SET(K4A_ROOT_DIR
    "${K4A_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for Azure Kinect Sensor SDK")

# Look for the header file.
FIND_PATH(K4A_INCLUDE_DIR NAMES k4a/k4a.h HINTS ${K4A_ROOT_DIR}/include ) #OSG_INCLUDE_DIR gibt man in der CMAKE GUI an, wo sich die includes von OSG befinden
# Look for the library path
FIND_PATH(K4A_LIBRARIES_DIR NAMES k4a.lib HINTS ${K4A_ROOT_DIR}/lib )

MARK_AS_ADVANCED(K4A_LIBRARIES_DIR K4A_INCLUDE_DIR) 
# handle the QUIETLY and REQUIRED arguments and set OPENVR_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(K4A DEFAULT_MSG K4A_LIBRARIES_DIR K4A_INCLUDE_DIR)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK DEFAULT_MSG OPENVR_SDK_LIBRARIES OPENVR_SDK_INCLUDE_DIRS)


