
include(FetchContent)
message(STATUS "Download/Update Face-Synthesizer")
FetchContent_Declare(
  face-synthesizer
  GIT_REPOSITORY git@gitlab.com:reauso/face-synthesizer.git
  GIT_TAG origin/main
)
FetchContent_MakeAvailable(face-synthesizer)

set(FACE_SYNTHESIZER_SRC ${FETCHCONTENT_BASE_DIR}/face-synthesizer-src)
set(FACE_SYNTHESIZER_SYNERGYNET_FACEBOXES_UTIL_DIR ${FACE_SYNTHESIZER_SRC}/tracking/networks/synergy_net/FaceBoxes/utils)
set(FACE_SYNTHESIZER_NMS_BUILD_COMMAND ${Python3_EXECUTABLE} build.py build_ext --inplace)

execute_process(
    COMMAND ${FACE_SYNTHESIZER_NMS_BUILD_COMMAND} 
    WORKING_DIRECTORY ${FACE_SYNTHESIZER_SYNERGYNET_FACEBOXES_UTIL_DIR} 
    OUTPUT_QUIET
)

file(REMOVE_RECURSE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/face-synthesizer-src)
file(COPY ${FACE_SYNTHESIZER_SRC} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# create python venv
set(VENV_TORCH_VERSION "1.8.1" CACHE STRING "Version of python torch to use in the virtual environment.")
set(VENV_TORCHVISION_VERSION "0.9.1" CACHE FILEPATH "Version of python torchvision to use in the virtual environment.")

set(VENV ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/.venv)
set(VENV_REQUIREMENTS_TXT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/face-synthesizer-src/requirements.txt)

string(REPLACE "." "" CUDA_VERSION ${CMAKE_VS_PLATFORM_TOOLSET_CUDA})

set(VENV_INSTALL_TORCH_CMD ${VENV}/Scripts/pip.exe install torch==${VENV_TORCH_VERSION}+cu${CUDA_VERSION} torchvision==${VENV_TORCHVISION_VERSION}+cu${CUDA_VERSION} -f https://download.pytorch.org/whl/torch_stable.html)
set(VENV_INSTALL_PYK4A_CMD ${VENV}/Scripts/pip.exe install pyk4a --no-use-pep517 --global-option=build_ext)
set(VENV_INSTALL_REQUIREMENTS_TXT_CMD ${VENV}/Scripts/pip.exe install -r ${VENV_REQUIREMENTS_TXT})

message(STATUS "Create/Update Python Virtual Environment")
if(NOT EXISTS  ${VENV})
  execute_process(COMMAND ${Python3_EXECUTABLE} -m venv ${VENV} OUTPUT_QUIET)
endif()
execute_process(COMMAND ${VENV_INSTALL_TORCH_CMD} OUTPUT_QUIET)
execute_process(COMMAND ${VENV_INSTALL_PYK4A_CMD} OUTPUT_QUIET)
execute_process(COMMAND ${VENV_INSTALL_REQUIREMENTS_TXT_CMD} OUTPUT_QUIET)
message(STATUS "Finished creating Python Virtual Environment")