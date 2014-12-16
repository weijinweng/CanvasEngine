cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
	ASSIMP_BUILD_NO_OWN_ZLIB
)
SET( INCLUDE_DIRS
	zlib
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")
