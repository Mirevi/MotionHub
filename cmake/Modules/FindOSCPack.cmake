IF (DEFINED ENV{OSCp_PATH})
    SET(OSCp_ROOT_DIR "$ENV{OSCp_PATH}") # alles  $ENV{XY} sind envrionment variablen des Betriebssystems
ENDIF()
SET(OSCp_ROOT_DIR
    "${OSCp_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for OpenSceneGraph")

# Look for the header file.
FIND_PATH(OSCp_INCLUDE_DIR NAMES OscTypes.h HINTS ${OSCp_ROOT_DIR}/include ) #OSG_INCLUDE_DIR gibt man in der CMAKE GUI an, wo sich die includes von OSG befinden

# Look for the library path
FIND_PATH(OSCp_LIBRARIES_DIR NAMES oscpack.lib HINTS ${OSCp_ROOT_DIR}/lib )

MARK_AS_ADVANCED(OSCp_LIBRARIES_DIR OSCp_INCLUDE_DIR) 
# handle the QUIETLY and REQUIRED arguments and set OPENVR_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSCp DEFAULT_MSG OSCp_LIBRARIES_DIR OSCp_INCLUDE_DIR)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK DEFAULT_MSG OPENVR_SDK_LIBRARIES OPENVR_SDK_INCLUDE_DIRS)


