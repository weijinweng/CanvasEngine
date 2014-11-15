#pragma once
#include "Canvas.h"
#include "CVS_Mesh.h"
#include "RenderNode.h"
#include "Light.h"
#include "Camera.h"
#include "RenderProgram.h"

namespace Canvas
{
	//Scene 
	class Scene
	{
	public:
		GLuint shadowMapFrameBuffer;
		GLuint light_ubo;
		int active_Lights_num;
		renderProgram* shadowProgram;
		std::vector<Camera*> cameras;
		Light lights[10];
		RenderSys* parentSys;
		transformNode root;
		std::vector<renderProgramID> programs;
		std::vector<renderNode> renderables;
		Scene(RenderSys* parent, char* filepath);
		Scene(RenderSys* parent);
		std::vector<CVS_Mesh*> loadFromFile(char* filepath);
		std::vector<CVS_Mesh*> loadFBXScene(char* filepath);
		std::vector<CVS_Mesh*> initSceneFromAiScene(const aiScene* scene);
		std::vector<CVS_Mesh*> initSceneFromFbxScene(const FbxScene* _scene);
		void initRenderNodesFromAiScene(aiNode* root, std::vector<CVS_Mesh*> meshes);
		transformNode* addRenderNodesFromAiNodes(aiNode* child, std::vector<CVS_Mesh*> meshes);
		transformNode* addRenderNodesFromFbxNodes(FbxNode* child, std::vector<CVS_Mesh*> meshes);
		transformNode* createNewNodeFromRenderNode(aiNode* child, std::vector<CVS_Mesh*> meshes);
		renderNode* getNode(std::string nodeName);
		//Generate our buffer.
		void generateLightBlock();
		void Render();
		void Render(int index);
		void getRenderProgram(std::string programName);
		//Shadow functions.
		void generateShadow();
		void initializeShadowMap(int index);
		void setShadowShader(std::string name = "Shadow");
		void testShadowMap();
		void activateShadow(std::string name);
		//Lighting functions.
		Light* activateLight(int index, std::string name);
		void deactivateLight(int index);
		Light* addSpotLight(std::string name, glm::vec3 position, glm::vec3 target, float cosCutoff);
		Light* addDirectionalLight(std::string name, glm::vec3 direction);
		~Scene();
	};
}