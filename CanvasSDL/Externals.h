#ifndef PRECOMPILED_H
#define PROCOMPILED_H

//STL externals
#include <stdio.h>
#include <fstream>
#include <string>
#include <list>
#include <vector>
//SDL supplements and extensions
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include <SDL_syswm.h>
//Windows only API
#include <Windows.h>
#include <dwmapi.h>
#undef main
//GLEW for opengl
#include <gl\glew.h>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtc\type_ptr.hpp"
//Assimp for static files
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
//FBX for autodesk formats.
#include <fbxsdk.h>

#endif
