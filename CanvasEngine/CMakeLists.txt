cmake_minimum_required( VERSION 2.8 )
find_package(OpenGL REQUIRED)

ADD_DEFINITIONS("-DGLEW_STATIC")

SET( INCLUDE_DIRS
${glew_INCLUDE_DIRS}
${glm_INCLUDE_DIRS}
${SDL2_INCLUDE_DIRS}
${SDL2_image_INCLUDE_DIRS}
${SDL2_ttf_INCLUDE_DIRS}
${FreeType_INCLUDE_DIRS}
${assimp_INCLUDE_DIRS}
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
)

parse_directory_name(PROJECT_NAME)
project( ${PROJECT_NAME} )
create_project_ex(CONSOLE "${INCLUDE_DIRS}" "${LINK_DIRS}" "${LINK_LIBS}")