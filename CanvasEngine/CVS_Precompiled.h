#ifndef CVS_PRECOMPILED
#define CVS_PRECOMPILED


//STL externals
#include <stdio.h>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>



#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	//Windows only API
	#include <Windows.h>
	#include <dwmapi.h>
	#include <gdiplus.h>
	#include <strsafe.h>
#define CVS_WIN32 100
#endif

//GLEW for opengl
#include <gl\glew.h>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtx\simd_mat4.hpp"
#include "glm\gtx\simd_quat.hpp"
#include "glm\gtx\simd_vec4.hpp"

//Assimp for static files
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

//FBX for autodesk formats.
#include <fbxsdk.h>

//Freetype for openglText
#include <ft2build.h>
#include FT_FREETYPE_H

//Window flag Declaration
#define CVS_WINDOW_MDI 2
#define CVS_WINDOW 4
#define CVS_WINDOW_MDI_CHILD 8

//CVS enumerators
enum CVS_Enum{
	CVS_NULL = 0,
	CVS_RDS_DEFERRED = 3,
	CVS_RDS_FORWARD = 5,
	CVS_RDS_NONE = 6,
	CVS_FLOAT = 7,
	CVS_UINT = 9,
	CVS_INT = 10,
	CVS_STATIC_DRAW = 11,
	CVS_ARRAY_BUFFER = 13,
	CVS_TRIANGLES = 14,
	CVS_ELEMENT_BUFFER = 15,
};

GLenum convertToGLEnum(CVS_Enum enumerator);
void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to);
int iClamp(int value, int min, int max);

typedef glm::vec3 cvec3;
typedef glm::vec4 cvec4;
typedef glm::mat4 cmat4;
typedef glm::simdVec4 fcvec4;
typedef glm::simdMat4 fcmat4;
typedef glm::fquat cquat;
typedef glm::vec2 cvec2;

#endif