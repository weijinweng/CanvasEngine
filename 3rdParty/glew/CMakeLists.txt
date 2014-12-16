cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
	GLEW_STATIC
)
SET( INCLUDE_DIRS
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")
