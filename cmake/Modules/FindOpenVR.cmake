if(EXISTS "${CHECK_IF_DEPS_EXIST}")
	message(STATUS "Found ${DEPS_NAME}. No download necessary. Skipping downloading ${DEPS_NAME}")
else()
	############################################################################
	# Start Download Dependency ################################################

	set( DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/../deps/temp_raw_downloads)

	set( ${TARGET} deps )

	# Check whether the source has been downloaded. If true, skip it.
	# Useful for external downloads like homebrew.
	if(USE_EXISTING_SRC_DIR)
	  if(EXISTS "${CURRENT_DEPS_PATH}" AND IS_DIRECTORY "${CURRENT_DEPS_PATH}")
		file(GLOB EXISTED_FILES "${CURRENT_DEPS_PATH}/*")
		if(EXISTED_FILES)
		  message(STATUS "${CURRENT_DEPS_PATH} is found and not empty, skipping download and extraction. ")
		  return()
		endif()
	  endif()
	  message(FATAL_ERROR "USE_EXISTING_SRC_DIR set to ON, but '${CURRENT_DEPS_PATH}' does not exist or is empty.")
	endif()

	# Taken from ExternalProject_Add.  Let's hope we can drop this one day when
	# ExternalProject_Add allows you to disable SHOW_PROGRESS on the file download.
	if(TIMEOUT)
	  set(timeout_args TIMEOUT ${timeout})
	  set(timeout_msg "${timeout} seconds")
	else()
	  set(timeout_args "")
	  set(timeout_msg "none")
	endif()

	string(REGEX MATCH "[^/\\?]*$" fname "${URL}")
	if(NOT "${fname}" MATCHES "(\\.|=)(bz2|tar|tgz|tar\\.gz|zip)$")
	  string(REGEX MATCH "([^/\\?]+(\\.|=)(bz2|tar|tgz|tar\\.gz|zip))/.*$" match_result "${URL}")
	  set(fname "${CMAKE_MATCH_1}")
	endif()
	if(NOT "${fname}" MATCHES "(\\.|=)(bz2|tar|tgz|tar\\.gz|zip)$")
	  message(FATAL_ERROR "error at dependency ${DEPS_NAME}: Could not extract tarball filename from url:\n  ${url}")
	endif()
	string(REPLACE ";" "-" fname "${fname}")

	set(file ${DOWNLOAD_DIR}/${fname})
	message(STATUS "file: ${file}")

	message(STATUS "downloading ${DEPS_NAME}...
		 src='${URL}'
		 dst='${file}'
		 timeout='${timeout_msg}'")

	file(DOWNLOAD ${URL} ${file}
	  ${timeout_args}
	  ${hash_args}
	  STATUS status
	  LOG log)

	list(GET status 0 status_code)
	list(GET status 1 status_string)

	if(NOT status_code EQUAL 0)
	  message(FATAL_ERROR "error at dependency ${DEPS_NAME}: downloading '${URL}' failed
	  status_code: ${status_code}
	  status_string: ${status_string}
	  log: ${log}
	")
	endif()

	# We could avoid computing the SHA256 entirely if a NULL_SHA256 was given,
	# but we want to warn users of an empty file.
	file(SHA256 ${file} ACTUAL_SHA256)
	if(NOT EXPECTED_SHA256 STREQUAL ACTUAL_SHA256)
	  # Wasn't a NULL SHA256 and we didn't match, so we fail.
	  message(FATAL_ERROR
		"error at dependency ${DEPS_NAME}: Failed to download ${URL}.  Expected a SHA256 of "
		"${EXPECTED_SHA256} but got ${ACTUAL_SHA256} instead.")
	endif()

	message(STATUS "downloading ${DEPS_NAME}... done")

	# Slurped from a generated extract-TARGET.cmake file.
	message(STATUS "extracting ${DEPS_NAME}...
		 src='${file}'
		 dst='${CURRENT_DEPS_PATH}'")

	if(NOT EXISTS "${file}")
	  message(FATAL_ERROR "error at dependency ${DEPS_NAME}: : file to extract does not exist: '${file}'")
	endif()

	# Prepare a space for extracting:
	#
	set(i 1234)
	while(EXISTS "${CURRENT_DEPS_PATH}/../ex-${TARGET}${i}")
	  math(EXPR i "${i} + 1")
	endwhile()
	set(ut_dir "${CURRENT_DEPS_PATH}/../ex-${TARGET}${i}")
	file(MAKE_DIRECTORY "${ut_dir}")

	# Extract it:
	#
	message(STATUS "extracting ${DEPS_NAME}... [tar xfz]")
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${file}
	  WORKING_DIRECTORY ${ut_dir}
	  RESULT_VARIABLE rv)

	if(NOT rv EQUAL 0)
	  message(STATUS "extracting ${DEPS_NAME}... [error clean up]")
	  file(REMOVE_RECURSE "${ut_dir}")
	  message(FATAL_ERROR "error at dependency ${DEPS_NAME}: : extract of '${file}' failed")
	endif()

	# Analyze what came out of the tar file:
	#
	message(STATUS "extracting ${DEPS_NAME}... [analysis]")
	file(GLOB contents "${ut_dir}/*")
	list(LENGTH contents n)
	if(NOT n EQUAL 1 OR NOT IS_DIRECTORY "${contents}")
	  set(contents "${ut_dir}")
	endif()

	# Move "the one" directory to the final directory:
	#
	message(STATUS "extracting ${DEPS_NAME}... [rename]")
	file(REMOVE_RECURSE ${CURRENT_DEPS_PATH})
	get_filename_component(contents ${contents} ABSOLUTE)
	file(RENAME ${contents} ${CURRENT_DEPS_PATH})

	# Clean up:
	#
	message(STATUS "extracting ${DEPS_NAME}... [clean up]")
	file(REMOVE_RECURSE "${ut_dir}")

	message(STATUS "extracting ${DEPS_NAME}... done")
	# END Download Dependency ################################################
	##########################################################################
endif()

# Look for the header file.
FIND_PATH(OpenVR_INCLUDE_DIR NAMES openvr.h HINTS ${CURRENT_DEPS_PATH}/headers )

# Look for the library path
FIND_PATH(OpenVR_LIBRARIES_DIR NAMES openvr_api.lib HINTS ${CURRENT_DEPS_PATH}/lib/win64 )

MARK_AS_ADVANCED(OpenVR_LIBRARIES_DIR OpenVR_INCLUDE_DIR) 
# handle the QUIETLY and REQUIRED arguments and set OPENVR_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVR DEFAULT_MSG OpenVR_LIBRARIES_DIR OpenVR_INCLUDE_DIR)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK DEFAULT_MSG OPENVR_SDK_LIBRARIES OPENVR_SDK_INCLUDE_DIRS)


