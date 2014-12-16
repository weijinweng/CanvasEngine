cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
LOAD_PNG
LOAD_JPG
)
SET( INCLUDE
SDL2
libpng
jpeg
)
SET( LINK
)

create_project(STATIC "${MACROS}" "${INCLUDE}" "${LINK}")