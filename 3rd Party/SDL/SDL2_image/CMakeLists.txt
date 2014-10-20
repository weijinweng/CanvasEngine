cmake_minimum_required( VERSION 2.8 )

ADD_DEFINITIONS("-DLOAD_PNG")

SET( INCLUDE_DIRS
${SDL2_INCLUDE_DIRS}
${libpng_INCLUDE_DIRS}
)
SET( LINK_DIRS
)
SET( LINK_LIBS
)

parse_directory_name(PROJECT_NAME)
project( ${PROJECT_NAME} )
create_project_ex(STATIC "${INCLUDE_DIRS}" "${LINK_DIRS}" "${LINK_LIBS}")
