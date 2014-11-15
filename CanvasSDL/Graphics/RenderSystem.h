#pragma once
#include "Canvas.h"
#include "TransformNode.h"
#include "Texture.h"

namespace Canvas{

	class RenderSys;
	class CVS_Mesh;
	class renderProgram;
	class renderNode;
	class Scene;
	class Camera;
	class Light;

	struct Animation {
	public:

	};

	//Render nodes's shader texture.
	struct shaderTextureID{
	public:
		RenderSys* parent;
		std::string name;
		Texture* texture;
		shaderTextureID(std::string name, RenderSys* rendersys);
		shaderTextureID(char*name);
		void setTexture(std::string textureName);
		//Efficient way to set texture indexvoid setTexture(int textureIndex);
		void addTexture(char* filePath);
		~shaderTextureID();
	};

	//        Scene - setACamera
	//       /     \ 
	//    Programs   transformNodes
	//        /         \ GetAllMatrices
	//    renderNodes
	//       /    \
	//    Mesh    transformNode
	//    



	//A rendersystem that resides in a window, manages a window's graphics output.
	class RenderSys: public Subsystem{
	public:
		//Memory management within the RenderSys
		GLuint VAO2d;
		SDL_GLContext glContext;
		std::vector<Scene> scenes;
		int maxMesh;
		std::vector<CVS_Mesh> m_meshes;
		int maxTextures;
		std::vector<Texture> textures;
		int maxShaders;
		std::vector<renderProgram> programs;
		FbxManager* fbxManager;
		RenderSys();
		bool initialize(SDL_Window* windowHandler);
		bool end();
		void draw();
		void draw(int x, int y, int width, int height);
		void loadMesh(char* path);
		void loadScene(char* path);
		void unloadAllMemory();
		renderProgram* createNewProgram(std::string name, char* vertPath, char* fragPath);
		Scene* createNewScene();
		Texture* createNewTexture(std::string name);
		CVS_Mesh* createNewMesh(std::string Mesh);
		renderProgram* getProgram(std::string name);
		Texture* getTexture(std::string name);
		std::vector<CVS_Mesh*> initMeshesFromAiScene(aiMesh** mesh, int meshCount);
		std::vector<CVS_Mesh*> initMeshesFromFbxNode(FbxNode* _pNode);
		void DrawTextureRect(Texture* texture, float x = 0, float y = 0, float width = 0, float height = 0);
		void init2D();
		~RenderSys();
	};

	//Window management
	class CanvasWindow{
	public:
		SDL_Window* windowHandler;
		RenderSys renderer;
		int width;
		int height;
		int x;
		int y;
		bool fullSreen;
		//Create a window of certain height and width.
		CanvasWindow(int x, int y, int width, int height, int flags);
		//Window management tools.
		void Minimize();
		void Maximize();
		void Move(int x, int y);
		void Resize(int width, int height);
		~CanvasWindow();
		static CanvasWindow* CVS_CreateWindow(int x, int y, int width, int height, int flags);
	};


};