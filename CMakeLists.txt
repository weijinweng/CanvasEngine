cmake_minimum_required( VERSION 2.8 )
# Set the build type.  Options are:
#  Coverage       : w/  debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/  debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/  optimization
#  RelWithDebInfo : w/  debug symbols, w/  optimization
#  MinSizeRel     : w/o debug symbols, w/  optimization, stripped binaries

# Include necessary submodules
if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/../CMake")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../CMake")
else()
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
endif()
include(SetOutputDirectories )
include(SourceFileScanner)

#create solution
parse_directory_name(SOLUTION_NAME)
project( ${SOLUTION_NAME} )

# Add compile flags and preprocessor definitions
add_definitions( "-D_CRT_SECURE_NO_WARNINGS" "/FC")
# Enable Unicode
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")

# directx sdk 
find_package(DirectX REQUIRED)

#At last, cache include dirs and scan source files
setup_solution(SOLUTION_NAME)
#EOF