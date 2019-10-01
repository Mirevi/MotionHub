IF (DEFINED ENV{K4ABT_PATH})
    SET(K4ABT_ROOT_DIR "$ENV{K4ABT_PATH}") # alles  $ENV{XY} sind envrionment variablen des Betriebssystems
ENDIF()
SET(K4ABT_ROOT_DIR
    "${K4ABT_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for OpenSceneGraph")

# Look for the header file.
FIND_PATH(K4ABT_INCLUDE_DIR NAMES k4abt.h HINTS ${K4ABT_ROOT_DIR}/include ) #OSG_INCLUDE_DIR gibt man in der CMAKE GUI an, wo sich die includes von OSG befinden


# Look for the library path
FIND_PATH(K4ABT_LIBRARIES_DIR NAMES k4abt.lib HINTS ${K4ABT_ROOT_DIR}/lib )

MARK_AS_ADVANCED(K4ABT_LIBRARIES_DIR K4ABT_INCLUDE_DIR) 
# handle the QUIETLY and REQUIRED arguments and set OPENVR_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(K4ABT DEFAULT_MSG K4ABT_LIBRARIES_DIR K4ABT_INCLUDE_DIR)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK DEFAULT_MSG OPENVR_SDK_LIBRARIES OPENVR_SDK_INCLUDE_DIRS)


