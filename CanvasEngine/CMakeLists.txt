cmake_minimum_required( VERSION 2.8 )


#----- Enable Unicode -----
#add_definitions(-DUNICODE -D_UNICODE)

ADD_DEFINITIONS("-DGLEW_STATIC")
ADD_DEFINITIONS("-DFBXSDK_SHARED")

#---
find_package(OpenGL REQUIRED)

SET( INCLUDE_DIRS
${glew_INCLUDE_DIRS}
${glm_INCLUDE_DIRS}
${SDL2_INCLUDE_DIRS}
${SDL2_image_INCLUDE_DIRS}
${SDL2_ttf_INCLUDE_DIRS}
${FreeType_INCLUDE_DIRS}
${assimp_INCLUDE_DIRS}
${fbxsdk_INCLUDE_DIRS}
)
SET( LINK_DIRS
${DirectX_LIBRARY_PATHS}
)
SET( LINK_LIBS
${OPENGL_LIBRARIES}
FreeType
assimp
SDL2
SDL2_image
SDL2_ttf
glew
glm
zlib
libpng
jpeg
#${fbxsdk_SOURCE_DIR}/lib/x86/debug/libfbxsdk-md.lib
${fbxsdk_SOURCE_DIR}/lib/x86/debug/libfbxsdk.lib
Gdiplus.lib
Comctl32.lib
Dwmapi.lib
Msimg32.lib
)

parse_directory_name(PROJECT_NAME)
project( ${PROJECT_NAME} )
create_project_ex(WIN32 "${INCLUDE_DIRS}" "${LINK_DIRS}" "${LINK_LIBS}")