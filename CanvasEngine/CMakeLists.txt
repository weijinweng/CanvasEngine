# Use GL and DX
find_package(OpenGL REQUIRED)
find_package(DirectX REQUIRED)
# Preprocessor Definitions
SET( MACROS
	FBXSDK_SHARED
	GLEW_STATIC
)
# Can include project name and directory
SET( INCLUDE
	# ----- OpenGL & Window -----
	glew
	glm
	fbxsdk
	SDL2
	SDL2_image
	SDL2_ttf
	# ----- App Specific -----
	zlib
	assimp
)
# Can include project name and absolute path to library
SET( LINK
	# ----- OpenGL & Window -----
	${OPENGL_LIBRARIES}
	glew
	SDL2
	SDL2_image
	SDL2_ttf
	# ----- App Specific -----
	zlib
	libpng
	jpeg
	${fbxsdk_SOURCE_DIR}/lib/x86/debug/libfbxsdk.lib
	assimp
	Gdiplus.lib
	Comctl32.lib
	Dwmapi.lib
	Msimg32.lib
	"${DirectX_LIBRARY_PATHS}/x86/DxErr.lib"
	"${DirectX_LIBRARY_PATHS}/x86/dxguid.lib"
	"${DirectX_LIBRARY_PATHS}/x86/dinput8.lib"
)

# Create project -- DO NOT EDIT ANYTHING BELOW THIS LINE --
# mode can be set to DYNAMIC, STATIC, CONSOLE, or WIN32
create_project(WIN32 "${MACROS}" "${INCLUDE}" "${LINK}")
