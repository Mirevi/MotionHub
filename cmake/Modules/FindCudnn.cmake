IF (DEFINED ENV{Cudnn_PATH})
    SET(Cudnn_ROOT_DIR "$ENV{Cudnn_PATH}") # alles  $ENV{XY} sind envrionment variablen des Betriebssystems
ENDIF()
SET(Cudnn_ROOT_DIR
    "${Cudnn_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for NVIDIA CUDA Deep Neural Network library")

# Look for the header file.
FIND_PATH(Cudnn_INCLUDE_DIR NAMES cudnn.h HINTS ${Cudnn_ROOT_DIR}/include ) #OSG_INCLUDE_DIR gibt man in der CMAKE GUI an, wo sich die includes von OSG befinden

# Look for the library path
FIND_PATH(Cudnn_LIBRARIES_DIR NAMES cudnn.lib HINTS ${Cudnn_ROOT_DIR}/lib/x64 )

MARK_AS_ADVANCED(Cudnn_LIBRARIES_DIR Cudnn_INCLUDE_DIR) 
# handle the QUIETLY and REQUIRED arguments and set OPENVR_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Cudnn DEFAULT_MSG Cudnn_LIBRARIES_DIR Cudnn_INCLUDE_DIR)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK DEFAULT_MSG OPENVR_SDK_LIBRARIES OPENVR_SDK_INCLUDE_DIRS)


