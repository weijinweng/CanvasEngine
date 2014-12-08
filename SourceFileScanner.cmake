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

# function(create_source_group sourceGroupName relativeSourcePath sourceFiles)
#
#
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

# function(parse_directory_name OUT_NAME)
#
#
#
function(parse_directory_name OUT_NAME)
string(REPLACE "/" ";" p2list "${CMAKE_CURRENT_SOURCE_DIR}")
list(REVERSE p2list)
list(GET p2list 0 temp)
set(${OUT_NAME} "${temp}" PARENT_SCOPE)
endfunction(parse_directory_name OUT_NAME)

# MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
#
#
#
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
function(REMOVE_FILE_EXTENSION inFiles outFiles)
	foreach(currFile ${inFiles})
		GET_FILENAME_COMPONENT(filePath ${currFile} PATH)
		GET_FILENAME_COMPONENT(fileNameWithoutExtension ${currFile} NAME_WE)
		set(filePathWithoutExtension "${filePath}/${fileNameWithoutExtension}")
		list(APPEND newFiles ${filePathWithoutExtension})
	endforeach()
	SET(${outFiles} "${newFiles}" PARENT_SCOPE)
endfunction()


#
#
#
#
function(setup_solution SOLUTION_NAME)
	#find all cmakelists files
	file(GLOB_RECURSE allProjects ${CMAKE_SOURCE_DIR}/CMakeLists.txt)
	list(REMOVE_ITEM allProjects ${CMAKE_SOURCE_DIR}/CMakeLists.txt)
	
	#Pre-Configure cache include dirs prior to adding subdirectories
	FOREACH(curFile ${allProjects})
		#get the directory of the cmakelists
		get_filename_component(fileDir ${curFile} DIRECTORY)
		
		# Pre-Pre-Configure Processing
		# Source file update and generation (flex and bison, github)
		if( PRECOMPILE_FLEX_AND_BISON AND NOT FLEX_EXECUTABLE STREQUAL "" AND NOT BISON_EXECUTABLE STREQUAL "")
			file(GLOB_RECURSE MY_FLEX_FILES ${fileDir}/*.l)
			file(GLOB_RECURSE MY_BISON_FILES ${fileDir}/*.y)
			
			if(MY_FLEX_FILES AND MY_BISON_FILES)
				REMOVE_FILE_EXTENSION(${MY_FLEX_FILES} flexFiles)
				REMOVE_FILE_EXTENSION(${MY_BISON_FILES} bisonFiles)
				SET( FlexArgs "-o${flexFiles}.flex.c" "${flexFiles}.l")
				SET( BisonArgs "-o${bisonFiles}.bison.c" "-d" "${bisonFiles}.y" )
				execute_process(COMMAND ${FLEX_EXECUTABLE} ${FlexArgs})
				execute_process(COMMAND ${BISON_EXECUTABLE} ${BisonArgs})
			endif()
		endif()
		
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
			FILE(RELATIVE_PATH newdir ${CMAKE_CURRENT_BINARY_DIR} ${_dir})
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
function(create_project mode includes linkDirs linkLibs)
#
#----- SCAN SOURCE -----
file(GLOB_RECURSE MY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE MY_CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
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

file(GLOB_RECURSE MY_MISC ${CMAKE_CURRENT_SOURCE_DIR}/*.l ${CMAKE_CURRENT_SOURCE_DIR}/*.y)
if( NOT MY_MISC STREQUAL "" )
	create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_MISC})
endif()

if( (MY_SRC STREQUAL "") AND (MY_HEADERS STREQUAL "") )
	message(FATAL_ERROR "Please insert at least one source file to use the CMakeLists.txt.")
endif()

#----- Shader Files -----
file(GLOB_RECURSE MY_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.vert	${CMAKE_CURRENT_SOURCE_DIR}/*.frag	${CMAKE_CURRENT_SOURCE_DIR}/*.glsl)
if( NOT MY_SHADERS STREQUAL "" )
	create_source_group("" "${CMAKE_CURRENT_SOURCE_DIR}/" ${MY_SHADERS})
endif()
#----- Default Precompiled Header -----
file(GLOB_RECURSE PRECOMPILED_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/*.pch.h)
file(GLOB_RECURSE PRECOMPILED_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/*.pch.cpp)

#----- Flex and Bison -----
if( USE_FLEX_AND_BISON )
	file(GLOB_RECURSE MY_FLEX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.l)
	file(GLOB_RECURSE MY_BISON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.y)
	# remove extension from .l and .y files
	REMOVE_FILE_EXTENSION(${MY_FLEX_FILES} flexFiles)
	REMOVE_FILE_EXTENSION(${MY_BISON_FILES} bisonFiles)
endif()
	
#------ DEPRECATRED RCC++ Include dirs -----
FOREACH(currentDir ${CURRENT_INCLUDE_DIRS})
	list(APPEND RCC++_IncludeDirs "${currentDir}\@") 
ENDFOREACH(currentDir ${CURRENT_INCLUDE_DIRS})

#message(${RCC++_IncludeDirs})
#string(REPLACE "/" "\\\\" WINDOWS_FORMAT_CURRENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
#add_definitions("-DCURRENT_INCLUDE_DIRS=${WINDOWS_FORMAT_CURRENT_DIRS}")





#------ INCLUDE DIRS AND LIBS -----
include(GenerateIDESettings)
# Must include self
include_directories( ${${PROJECT_NAME}_INCLUDE_DIRS} )
# Process include list, an element could be a list of dirs or a target name
FOREACH(currentName ${includes})
	if(EXISTS ${currentName})
		# if exists, it is a directory
		list(APPEND includeDirs ${currentName})
#		message("IT IS A DIR ${currentName}")
	else()
		# if doesn't exist, it is a target, we retrieve the include dirs by appending _INCLUDE_DIRS to its name
		list(APPEND includeDirs ${${currentName}_INCLUDE_DIRS})
		#message("A TARGET!! ${${currentName}_INCLUDE_DIRS}")
	endif()
ENDFOREACH(currentName ${includes})
#message("1111111111111111111111 ${includeDirs}")
include_directories(${includeDirs})
link_directories(${linkDirs})
link_libraries(${linkLibs})




#----- PRECOMPILED HEADER -----
if( NOT PRECOMPILED_HEADER_PATH STREQUAL "" AND NOT PRECOMPILED_SOURCE_PATH STREQUAL "")
IF(MSVC)
	GET_FILENAME_COMPONENT(PRECOMPILED_HEADER ${PRECOMPILED_HEADER_PATH} NAME)
	GET_FILENAME_COMPONENT(PRECOMPILED_SOURCE ${PRECOMPILED_SOURCE_PATH} NAME)
	
	message("Precompiled Header Found For ${PROJECT_NAME}: ${PRECOMPILED_HEADER} & ${PRECOMPILED_SOURCE}" )
	GET_FILENAME_COMPONENT(PRECOMPILED_BASENAME ${PRECOMPILED_HEADER} NAME_WE)
	SET(PRECOMPILED_BINARY "${PRECOMPILED_BASENAME}.pch")
	
	SET_SOURCE_FILES_PROPERTIES(${MY_SRC}
							PROPERTIES COMPILE_FLAGS "/Yu\"${PRECOMPILED_HEADER}\" /FI\"${PRECOMPILED_HEADER}\" /Fp\"${PRECOMPILED_BINARY}\""
									   OBJECT_DEPENDS "${PRECOMPILED_BINARY}")  
									   
	SET_SOURCE_FILES_PROPERTIES(${PRECOMPILED_SOURCE_PATH}
							PROPERTIES COMPILE_FLAGS "/Yc\"${PRECOMPILED_HEADER}\" /Fp\"${PRECOMPILED_BINARY}\""
								   OBJECT_OUTPUTS "${PRECOMPILED_BINARY}")
ENDIF(MSVC)

endif()


# Force C++ if there's any cpp file
if(MY_CPP_SRC)
	set_source_files_properties(${MY_SRC} PROPERTIES LANGUAGE CXX)
endif()

#----- CREATE TARGET -----
if(${mode} STREQUAL "STATIC")
	add_library (${PROJECT_NAME} STATIC ${MY_SRC} ${MY_HEADERS} ${MY_MISC})
elseif(${mode} STREQUAL "DYNAMIC" OR ${mode} STREQUAL "SHARED" )
	add_library (${PROJECT_NAME} SHARED ${MY_SRC} ${MY_HEADERS} ${MY_MISC})
elseif(${mode} STREQUAL "CONSOLE")
	add_executable (${PROJECT_NAME} ${MY_SRC} ${MY_HEADERS} ${MY_SHADERS} ${MY_RESOURCES} ${MY_MISC})
elseif(${mode} STREQUAL "WIN32")
	add_executable (${PROJECT_NAME} WIN32 ${MY_SRC} ${MY_HEADERS} ${MY_SHADERS} ${MY_RESOURCES} ${MY_MISC})
endif()

#------ force include -----
if( NOT MY_PRECOMPILED_HEADER STREQUAL "" AND NOT MY_PRECOMPILED_SOURCE STREQUAL "")
	#get_target_property(FLAGS ${PROJECT_NAME} COMPILE_FLAGS)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${FLAGS} /FI\"${MY_PRECOMPILED_HEADER}\"")
endif()


# TODO: OPTIMIZE THIS
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





#----- DEPRECATED copy command for shaders -----
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





#----- Flex and Bison -----
if( USE_FLEX_AND_BISON AND NOT FLEX_EXECUTABLE STREQUAL "" AND NOT BISON_EXECUTABLE STREQUAL "")
SET( FLEX_EXECUTABLE1 "${CMAKE_SOURCE_DIR}/3rdParty/FlexBison/bin/flex.exe")
SET( BISON_EXECUTABLE1 "${CMAKE_SOURCE_DIR}/3rdParty/FlexBison/bin/bison.exe")
	SET( FlexArgs "-o${MY_FLEX_FILES}.flex.c" "${MY_FLEX_FILES}.l")
	SET( BisonArgs "-o${MY_BISON_FILES}.bison.c" "-d" "${MY_BISON_FILES}.y" )
	
	#[[
	# Create target for the parser
	ADD_CUSTOM_TARGET(${PROJECT_NAME}Flex ALL
		COMMAND ${FLEX_EXECUTABLE} 
		-o"${flexFiles}.flex.c"
		"${flexFiles}.l"
		COMMENT "Creating .flex.c")
		
	ADD_CUSTOM_TARGET(${PROJECT_NAME}Bison ALL
		COMMAND ${BISON_EXECUTABLE}
		-o"${bisonFiles}.bison.c"
		-d "${bisonFiles}.y"
		COMMENT "Creating .bison.c")
	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}Flex)
	add_dependencies(${PROJECT_NAME}Flex ${PROJECT_NAME}Bison)
	 
	if( MSVC )
		SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
		SET_PROPERTY(TARGET ${PROJECT_NAME}Flex		PROPERTY FOLDER CMakePredefinedTargets)
		SET_PROPERTY(TARGET ${PROJECT_NAME}Bison		PROPERTY FOLDER CMakePredefinedTargets)
	endif()
	]]#
	
	
	ADD_CUSTOM_TARGET(${PROJECT_NAME}FlexBison echo "Generating Flex & Bison source files")
		
	ADD_CUSTOM_COMMAND(
	   DEPENDS ${flexFiles}.l
	   COMMAND ${FLEX_EXECUTABLE}
	   ARGS -o"${flexFiles}.flex.c"
			"${flexFiles}.l"
	   TARGET ${PROJECT_NAME}FlexBison
	   OUTPUTS ${flexFiles}.flex.c)
	   
	ADD_CUSTOM_COMMAND(
	   DEPENDS ${flexFiles}.y
	   COMMAND ${BISON_EXECUTABLE}
	   ARGS -o"${bisonFiles}.bison.c"
			"${bisonFiles}.y"
	   TARGET ${PROJECT_NAME}FlexBison
	   OUTPUTS ${bisonFiles}.bison.c)

	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}FlexBison)
		if( MSVC )
		SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
		SET_PROPERTY(TARGET ${PROJECT_NAME}FlexBison		PROPERTY FOLDER CMakePredefinedTargets)
		endif()
endif()





#----- PreBuild Resource Copy ------
if( NOT MY_SHADERS STREQUAL "" )
	add_custom_target(${PROJECT_NAME}PreBuild ALL
		COMMAND ${CMAKE_COMMAND}
		-DSrcDir=${CMAKE_CURRENT_SOURCE_DIR}
		-DDestDir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		-P ${CMAKE_MODULE_PATH}/CopyResource.cmake
		COMMENT "Copying resource files to the binary output directory")
		
	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}PreBuild)
		
	if( MSVC )
		SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
		SET_PROPERTY(TARGET ${PROJECT_NAME}PreBuild		PROPERTY FOLDER CMakePredefinedTargets)
	endif()
endif()

if( MSVC )
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
	SET_PROPERTY(TARGET ${PROJECT_NAME}		PROPERTY FOLDER ${filterDir})
endif()





#------ need linker language flag for header only static libraries -----
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
endfunction(create_project mode linkDirectories linLibraries)