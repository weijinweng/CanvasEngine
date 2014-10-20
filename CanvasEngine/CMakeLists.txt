cmake_minimum_required( VERSION 2.8 )
find_package(OpenGL REQUIRED)

ADD_DEFINITIONS("-DGLEW_STATIC")
#ADD_DEFINITIONS("-DASSIMP_BUILD_NO_EXPORT")

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
	"C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x86"
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
)

parse_directory_name(PROJECT_NAME)
project( ${PROJECT_NAME} )
create_project_ex(CONSOLE "${INCLUDE_DIRS}" "${LINK_DIRS}" "${LINK_LIBS}")