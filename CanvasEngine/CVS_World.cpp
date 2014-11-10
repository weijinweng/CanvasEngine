#include "Canvas.h"
#include "CVS_World.h"

CVS_Scene::CVS_Scene(CVS_WorldSystem* system, CVS_RenderScene* renderer)
{
	this->scene = renderer;
	this->system = system;
}

bool CVS_Scene::loadFile(char* filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	std::vector<CVS_Mesh*> meshes = GLOBALSTATEMACHINE.m_RenderSub.addMeshesFromaiScene(scene);

	for(int i = 0, e = scene->mRootNode->mNumChildren; i < e; ++i)
	{
		CVS_GameObject* newObject = new CVS_GameObject(scene->mRootNode->mChildren[i]);
	}
}


bool CVS_WorldSystem::Initialize()
{
	return true;
}

bool CVS_WorldSystem::Update()
{
	return true;
}

bool CVS_WorldSystem::End()
{
	return true;
}

CVS_Scene* CVS_WorldSystem::createNewScene()
{
	CVS_RenderScene* renderScene = GLOBALSTATEMACHINE.m_RenderSub.createNewScene();

	CVS_Scene* newScene = new CVS_Scene(this, renderScene);

	this->scenes.push_back(newScene);
	return newScene;
}