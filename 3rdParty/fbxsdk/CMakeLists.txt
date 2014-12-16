cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
)
SET( INCLUDE_DIRS
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
	Set(mode "debug")
else()
	SET(mode "release")
endif()

configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/${mode}/libfbxsdk.dll
${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
