cmake_minimum_required( VERSION 2.8 )

# create_source_group(relativeSourcePath sourceGroupName files)
#
# Creates a source group with the specified name relative to the relative path
# specified.
#
# Parameters:
#    - sourceGroupName: Name of the source group to create.
#    - relativeSourcePath: Relative path to the files.
#    - sourceFiles: Files to add to the source group.
#
# For example if you have the following directory structure:
#
#    - ExampleApplication
#        - include
#            - Main.h
#                - Window
#                    Window.h
#        - source
#            - Main.cpp
#                - Window
#                    Window.cpp
#
# You can get your list of files and call create_source_group the following way
#
#    file(GLOB_RECURSE my_source_files ${CMAKE_CURRENT_SOURCE_DIR}/source/*)
#    create_source_group("Source Files"
#	"${CMAKE_CURRENT_SOURCE_DIR}/source" ${my_source_files})
#    file(GLOB_RECURSE my_header_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
#    create_source_group("Header Files"
#	"${CMAKE_CURRENT_SOURCE_DIR}/include" ${my_header_files})
#    add_executable(ExampleApplication ${my_source_files} ${my_header_files})
#
# Then the generated solution would look like this
#
#    - ExampleApplication (project)
#        - Header Files
#            - Main.h
#                - Window
#                    Window.h
#        - Source Files
#            - Main.cpp
#                - Window
#                    Window.cpp
#

function(create_source_group sourceGroupName relativeSourcePath sourceFiles)
FOREACH(currentSourceFile ${ARGN})
	FILE(RELATIVE_PATH folder ${relativeSourcePath} ${currentSourceFile})
	get_filename_component(filename ${folder} NAME)
	string(REPLACE ${filename} "" folder ${folder})
	if(NOT folder STREQUAL "")
		string(REGEX REPLACE "/+$" "" folderlast ${folder})
		string(REPLACE "/" "\\" folderlast ${folderlast})
		SOURCE_GROUP("${sourceGroupName}\\${folderlast}" FILES ${currentSourceFile})
	endif(NOT folder STREQUAL "")
ENDFOREACH(currentSourceFile ${ARGN})

FOREACH(currentSourceFile ${sourceFiles})
	FILE(RELATIVE_PATH folder ${relativeSourcePath} ${currentSourceFile})
	get_filename_component(filename ${folder} NAME)
	string(REPLACE ${filename} "" folder ${folder})
	if(NOT folder STREQUAL "")
		string(REGEX REPLACE "/+$" "" folderlast ${folder})
		string(REPLACE "/" "\\" folderlast ${folderlast})
		SOURCE_GROUP("${sourceGroupName}\\${folderlast}" FILES ${currentSourceFile})
	endif(NOT folder STREQUAL "")
ENDFOREACH(currentSourceFile ${sourceFiles})
endfunction(create_source_group)

#function(parse_directory_name PROJECT_NAME)
#
#
#
function(parse_directory_name PROJECT_NAME)
string(REPLACE "/" ";" p2list "${CMAKE_CURRENT_SOURCE_DIR}")
list(REVERSE p2list)
list(GET p2list 0 temp)
set(PROJECT_NAME "${temp}" PARENT_SCOPE)
endfunction(parse_directory_name PROJECT_NAME)

#
#
#
#
function(setup_solution PROJECT_NAME)
	file(GLOB_RECURSE allProjects ${CMAKE_SOURCE_DIR}/ CMakeLists.txt)
	list(REMOVE_ITEM allProjects ${CMAKE_SOURCE_DIR}/CMakeLists.txt)
	FOREACH(curFile ${allProjects})
		get_filename_component(fileDir ${curFile} DIRECTORY)
		add_subdirectory( ${fileDir} )
		message(STATUS "Included: " ${fileDir})
	ENDFOREACH(curFile ${allProjects})
endfunction(setup_solution PROJECT_NAME)
#
#
#
#
function(create_project mode)

file(GLOB_RECURSE MY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ *.cpp *.c)
if( NOT MY_SRC STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_SRC})
endif()

file(GLOB_RECURSE MY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/ *.h *.inl)
if( NOT MY_HEADERS STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_HEADERS})
endif()

if( (MY_SRC STREQUAL "") AND (MY_HEADERS STREQUAL "") )
message(FATAL_ERROR "Please insert at least one .cpp or .h file in to either src or include directory respectively.")
endif()

set (CURRENT_INCLUDE_DIRS "")
foreach (_headerFile ${MY_HEADERS})
	get_filename_component(_dir ${_headerFile} PATH)
	list (APPEND CURRENT_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES CURRENT_INCLUDE_DIRS)
include_directories( ${CURRENT_INCLUDE_DIRS} )

set(${PROJECT_NAME}_INCLUDE_DIRS "${CURRENT_INCLUDE_DIRS}" CACHE STRING "")
include_directories( ${${PROJECT_NAME}_INCLUDE_DIRS} )



FOREACH(currentDir ${CURRENT_INCLUDE_DIRS})
list(APPEND RCC++_IncludeDirs "${currentDir}\@") 
ENDFOREACH(currentDir ${CURRENT_INCLUDE_DIRS})

#message(${RCC++_IncludeDirs})
string(REPLACE "/" "\\\\" WINDOWS_FORMAT_CURRENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions("-DCURRENT_INCLUDE_DIRS=${WINDOWS_FORMAT_CURRENT_DIRS}")

#------ set target -----
include(GenerateVcxprojUserSettings)

if(${mode} STREQUAL "STATIC")
	add_library (${PROJECT_NAME} STATIC ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "DYNAMIC")
	add_library (${PROJECT_NAME} SHARED ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "CONSOLE")
	add_executable (${PROJECT_NAME} ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "WIN32")
	add_executable (${PROJECT_NAME} WIN32 ${MY_SRC} ${MY_HEADERS})
endif()

#------ set filter directory -----
string(REPLACE "/" ";" sourceDirList "${CMAKE_SOURCE_DIR}")
string(REPLACE "/" ";" currSourceDirList "${CMAKE_CURRENT_SOURCE_DIR}")
list(REVERSE currSourceDirList)
list(REMOVE_AT currSourceDirList 0)
list(REVERSE currSourceDirList)
foreach(sourceDir ${sourceDirList})
list(REMOVE_AT currSourceDirList 0)
endforeach()
list(LENGTH currSourceDirList listLength)
string(REPLACE ";" "/" filterDir "${currSourceDirList}")

if( MSVC )
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
	SET_PROPERTY(TARGET ${PROJECT_NAME}		PROPERTY FOLDER ${filterDir})
endif()

#------ need linker language flag for header only static libraries -----
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
endfunction(create_project mode)


#
#
#
#
function(create_project_ex mode includeDirs linkDirs linkLibs)

file(GLOB_RECURSE MY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ *.cpp *.c)
if( NOT MY_SRC STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_SRC})
endif()

file(GLOB_RECURSE MY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/ *.h *.inl)
if( NOT MY_HEADERS STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_HEADERS})
endif()

if( (MY_SRC STREQUAL "") AND (MY_HEADERS STREQUAL "") )
message(FATAL_ERROR "Please insert at least one .cpp or .h file in to either src or include directory respectively.")
endif()

set (CURRENT_INCLUDE_DIRS "")
foreach (_headerFile ${MY_HEADERS})
	get_filename_component(_dir ${_headerFile} PATH)
	list (APPEND CURRENT_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES CURRENT_INCLUDE_DIRS)
include_directories( ${CURRENT_INCLUDE_DIRS} )

set(${PROJECT_NAME}_INCLUDE_DIRS "${CURRENT_INCLUDE_DIRS}" CACHE STRING "")
include_directories( ${${PROJECT_NAME}_INCLUDE_DIRS} )

#------ RCC++ Include dirs -----
FOREACH(currentDir ${CURRENT_INCLUDE_DIRS})
list(APPEND RCC++_IncludeDirs "${currentDir}\@") 
ENDFOREACH(currentDir ${CURRENT_INCLUDE_DIRS})

#message(${RCC++_IncludeDirs})
string(REPLACE "/" "\\\\" WINDOWS_FORMAT_CURRENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions("-DCURRENT_INCLUDE_DIRS=${WINDOWS_FORMAT_CURRENT_DIRS}")

#------ set target -----
include(GenerateVcxprojUserSettings)

include_directories(${includeDirs})
link_directories(${linkDirs})
link_libraries(${linkLibs})

if(${mode} STREQUAL "STATIC")
	add_library (${PROJECT_NAME} STATIC ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "DYNAMIC")
	add_library (${PROJECT_NAME} SHARED ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "CONSOLE")
	add_executable (${PROJECT_NAME} ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "WIN32")
	add_executable (${PROJECT_NAME} WIN32 ${MY_SRC} ${MY_HEADERS})
endif()

#------ set filter directory -----
string(REPLACE "/" ";" sourceDirList "${CMAKE_SOURCE_DIR}")
string(REPLACE "/" ";" currSourceDirList "${CMAKE_CURRENT_SOURCE_DIR}")
list(REVERSE currSourceDirList)
list(REMOVE_AT currSourceDirList 0)
list(REVERSE currSourceDirList)
foreach(sourceDir ${sourceDirList})
list(REMOVE_AT currSourceDirList 0)
endforeach()
list(LENGTH currSourceDirList listLength)
string(REPLACE ";" "/" filterDir "${currSourceDirList}")

if( MSVC )
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
	SET_PROPERTY(TARGET ${PROJECT_NAME}		PROPERTY FOLDER ${filterDir})
endif()

#------ need linker language flag for header only static libraries -----
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
endfunction(create_project_ex mode linkDirectories linLibraries)