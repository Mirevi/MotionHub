IF (DEFINED ENV{OSG_PATH})
    SET(OSG_ROOT_DIR "$ENV{OSG_PATH}") # alles  $ENV{XY} sind envrionment variablen des Betriebssystems
ENDIF()
SET(OSG_ROOT_DIR
    "${OSG_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for OpenSceneGraph")

# Look for the header file.
FIND_PATH(OSG_INCLUDE_DIR NAMES osg/Vec3 HINTS ${OSG_ROOT_DIR}/include ) #OSG_INCLUDE_DIR gibt man in der CMAKE GUI an, wo sich die includes von OSG befinden

# Look for the library path
FIND_PATH(OSG_LIBRARIES_DIR NAMES osg.lib HINTS ${OSG_ROOT_DIR}/lib )

MARK_AS_ADVANCED(OSG_LIBRARIES_DIR OSG_INCLUDE_DIR) 
# handle the QUIETLY and REQUIRED arguments and set OPENVR_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSG DEFAULT_MSG OSG_LIBRARIES_DIR OSG_INCLUDE_DIR)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK DEFAULT_MSG OPENVR_SDK_LIBRARIES OPENVR_SDK_INCLUDE_DIRS)


