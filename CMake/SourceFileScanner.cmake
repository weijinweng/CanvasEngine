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
function(parse_directory_name OUT_NAME)
string(REPLACE "/" ";" p2list "${CMAKE_CURRENT_SOURCE_DIR}")
list(REVERSE p2list)
list(GET p2list 0 temp)
set(${OUT_NAME} "${temp}" PARENT_SCOPE)
endfunction(parse_directory_name OUT_NAME)

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)
#
#
#
#
function(setup_solution SOLUTION_NAME)
	#find all cmakelists files
	file(GLOB_RECURSE allProjects ${CMAKE_SOURCE_DIR}/CMakeLists.txt)
	list(REMOVE_ITEM allProjects ${CMAKE_SOURCE_DIR}/CMakeLists.txt)
			
	#Pre cache include dirs prior to adding subdirectory
	FOREACH(curFile ${allProjects})
		#get the directory of the cmakelists
		get_filename_component(fileDir ${curFile} DIRECTORY)
		
		#parse the directory name for caching project specific include dirs
		string(REPLACE "/" ";" p2list "${fileDir}")
		list(REVERSE p2list)
		list(GET p2list 0 PROJECT_NAME)
		
		#scan all headers
		file(GLOB_RECURSE MY_HEADERS ${fileDir}/*.h ${fileDir}/*.hpp ${fileDir}/*.inl)
		if( NOT MY_HEADERS STREQUAL "" )
		create_source_group("" "${fileDir}/" ${MY_HEADERS})
		endif()
		
		#remove duplicates and parse directories
		set(CURRENT_INCLUDE_DIRS "")
		set(_headerFile "")
		foreach (_headerFile ${MY_HEADERS})
			get_filename_component(_dir ${_headerFile} PATH)
			list (APPEND CURRENT_INCLUDE_DIRS ${_dir})
		endforeach()
		list(REMOVE_DUPLICATES CURRENT_INCLUDE_DIRS)
		#include current include dirs and cache the content
		unset(${PROJECT_NAME}_INCLUDE_DIRS CACHE)
		set(${PROJECT_NAME}_INCLUDE_DIRS "${CURRENT_INCLUDE_DIRS}" CACHE STRING "")
		
	ENDFOREACH(curFile ${allProjects})

	#Include sub directories now
	FOREACH(curFile ${allProjects})
		get_filename_component(fileDir ${curFile} DIRECTORY)
		add_subdirectory( ${fileDir} )
		message(STATUS "Project Added: " ${fileDir})
	ENDFOREACH(curFile ${allProjects})
endfunction(setup_solution SOLUTION_NAME)
#
#
#
#
function(create_project_ex mode includeDirs linkDirs linkLibs)

#----- Scan source -----
file(GLOB_RECURSE MY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
if( NOT MY_SRC STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_SRC})
endif()

file(GLOB_RECURSE MY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
if( NOT MY_HEADERS STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_HEADERS})
endif()

file(GLOB_RECURSE MY_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.rc)
if( NOT MY_RESOURCES STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_RESOURCES})
endif()

if( (MY_SRC STREQUAL "") AND (MY_HEADERS STREQUAL "") )
message(FATAL_ERROR "Please insert at least one .cpp or .h file in to either src or include directory respectively.")
endif()

#----- Miscellaneous ------
#----- add glsl shader files to solution -----
file(GLOB_RECURSE
	MY_SHADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/*.glsl
	)
	
file(GLOB_RECURSE
	MY_PRECOMPILED_HEADER
	${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
	)
file(GLOB_RECURSE
	MY_PRECOMPILED_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp
	)
	
if( NOT MY_SHADERS STREQUAL "" )
create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_SHADERS})
endif()

#------ RCC++ Include dirs -----
FOREACH(currentDir ${CURRENT_INCLUDE_DIRS})
list(APPEND RCC++_IncludeDirs "${currentDir}\@") 
ENDFOREACH(currentDir ${CURRENT_INCLUDE_DIRS})

#message(${RCC++_IncludeDirs})
string(REPLACE "/" "\\\\" WINDOWS_FORMAT_CURRENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions("-DCURRENT_INCLUDE_DIRS=${WINDOWS_FORMAT_CURRENT_DIRS}")

#------ set target -----
include(GenerateVcxprojUserSettings)

include_directories( ${${PROJECT_NAME}_INCLUDE_DIRS} )
include_directories(${includeDirs})
link_directories(${linkDirs})
link_libraries(${linkLibs})

if( NOT MY_PRECOMPILED_HEADER STREQUAL "" AND NOT MY_PRECOMPILED_SOURCE STREQUAL "")
IF(MSVC)
	GET_FILENAME_COMPONENT(MY_PRECOMPILED_SOURCE ${MY_PRECOMPILED_SOURCE} NAME)
	GET_FILENAME_COMPONENT(MY_PRECOMPILED_HEADER ${MY_PRECOMPILED_HEADER} NAME)
	
	message(${MY_PRECOMPILED_HEADER})
	message(STATUS "${PROJECT_NAME} is using precompiled header stdafx.h & stdafx.cpp")
	GET_FILENAME_COMPONENT(PrecompiledBasename ${MY_PRECOMPILED_HEADER} NAME_WE)
	SET(PrecompiledBinary "${PrecompiledBasename}.pch")
	
	SET_SOURCE_FILES_PROPERTIES(${Sources}
							PROPERTIES COMPILE_FLAGS "/Yu\"${MY_PRECOMPILED_HEADER}\" /FI\"${MY_PRECOMPILED_HEADER}\" /Fp\"${PrecompiledBinary}\""
									   OBJECT_DEPENDS "${PrecompiledBinary}")  
	SET_SOURCE_FILES_PROPERTIES(${MY_PRECOMPILED_SOURCE}
							PROPERTIES COMPILE_FLAGS "/Yc\"${MY_PRECOMPILED_HEADER}\" /Fp\"${PrecompiledBinary}\""
								   OBJECT_OUTPUTS "${PrecompiledBinary}")
ENDIF(MSVC)

endif()




if(${mode} STREQUAL "STATIC")
	add_library (${PROJECT_NAME} STATIC ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "DYNAMIC" OR ${mode} STREQUAL "SHARED" )
	add_library (${PROJECT_NAME} SHARED ${MY_SRC} ${MY_HEADERS})
elseif(${mode} STREQUAL "CONSOLE")
	add_executable (${PROJECT_NAME} ${MY_SRC} ${MY_HEADERS} ${MY_SHADERS} ${MY_RESOURCES})
elseif(${mode} STREQUAL "WIN32")
	add_executable (${PROJECT_NAME} WIN32 ${MY_SRC} ${MY_HEADERS} ${MY_SHADERS} ${MY_RESOURCES})
endif()

#------ force include -----
if( NOT MY_PRECOMPILED_HEADER STREQUAL "" AND NOT MY_PRECOMPILED_SOURCE STREQUAL "")
	#get_target_property(FLAGS ${PROJECT_NAME} COMPILE_FLAGS)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${FLAGS} /FI\"${MY_PRECOMPILED_HEADER}\"")
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

#----- copy command for shaders -----
#[[
if( NOT MY_SHADERS STREQUAL "" )
add_custom_target(copy)
foreach(ShaderFile ${MY_SHADERS})
	add_custom_command(TARGET copy PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E
	copy ${ShaderFile} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach()
endif()
]]#

#[[
IF(FORCE_GLOBAL_GENERATE)
	FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/some_file" "whatever")
	SET(FORCE_GLOBAL_GENERATE OFF BOOL "" FORCE)
ENDIF(FORCE_GLOBAL_GENERATE)
IF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/some_file)
	CONFIGURE_FILE(${CMAKE_CURRENT_BINARY_DIR}/some_file
	${CMAKE_CURRENT_BINARY_DIR}/some_file.done)
ENDIF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/some_file)
]]#

if( NOT MY_SHADERS STREQUAL "" )
	add_custom_target(CopyResource ALL
		COMMAND ${CMAKE_COMMAND}
		-DSrcDir=${CMAKE_CURRENT_SOURCE_DIR}
		-DDestDir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		-P ${CMAKE_MODULE_PATH}/CopyResource.cmake
		COMMENT "Copying resource files to the binary output directory")
		
	add_dependencies(${PROJECT_NAME} CopyResource)
		
	if( MSVC )
		SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
		SET_PROPERTY(TARGET CopyResource		PROPERTY FOLDER CMakePredefinedTargets)
	endif()
endif()

if( MSVC )
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
	SET_PROPERTY(TARGET ${PROJECT_NAME}		PROPERTY FOLDER ${filterDir})
endif()

#------ need linker language flag for header only static libraries -----
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
endfunction(create_project_ex mode linkDirectories linLibraries)