#ifndef CVS_WORLD_H
#define CVS_WORLD_H

#include "CVS_Transform.h"
#include "CVS_RenderSystem.h"

class CVS_GameObject;
struct CVS_WorldSystem;
struct CVS_Scene;

struct CVS_GameComponent{
	std::string name;
	int priority;
	CVS_GameObject* object;
	CVS_GameComponent(CVS_GameObject* object);
	virtual void Update() = 0;
};

struct CVS_SceneComponent{
	std::string name;
	int priority;
	CVS_GameObject* object;
	CVS_SceneComponent(CVS_Scene* scene);
	virtual void Update() = 0;
	virtual void onMerge(CVS_Scene* other) = 0;
};

struct CVS_CameraComponent:public CVS_GameComponent{
	CVS_CameraComponent(CVS_GameObject* object, CVS_Camera* cam);
};

struct CVS_RenderComponent:public CVS_GameComponent{
	CVS_RenderNode* m_pNode;
	CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene);
	void Update();
};

class CVS_GameObject{
public:
	CVS_GameObject* parent;
	std::string name;
	CVS_TransformNode transformNode;
	std::vector<CVS_GameComponent*> components;
	std::vector<CVS_GameObject*> children;
	CVS_GameObject(std::string name = "Object");
	CVS_GameObject(const aiNode* node, CVS_RenderScene* scene, std::vector<CVS_Mesh*>);
	CVS_GameObject(CVS_RenderScene* scene, std::vector<CVS_Mesh*>);
	void UpdateTransformMatrix();
	void setParent(CVS_GameObject* parent);
	void addChildren(CVS_GameObject* child);
	void removeChildren(CVS_GameObject* child);
	void addComponent(CVS_GameComponent* component);

	void Update();
};

struct CVS_Scene{
public:
	std::vector<CVS_GameObject*> m_objects;
	CVS_WorldSystem* m_pWorld;
	CVS_RenderScene* m_pScene;
	CVS_Scene(CVS_WorldSystem* system, CVS_RenderScene* scene);
	void Update();
	bool loadFile(char* filepath);
	FbxScene* _loadFBXScene(char* filepath);
	bool initializeGameObjectsFromaiScene(const aiScene* scene, std::vector<CVS_Mesh*> meshes);
	CVS_GameObject* createGameObjectsFromaiNodes(const aiNode* node, std::vector<CVS_Mesh*> meshes);
	//void Merge(CVS_Scene* other);
};

struct CVS_WorldSystem:public CVS_SubSystem{
public:
	std::vector<CVS_Scene*> scenes;
	bool Initialize();
	bool Update();
	bool End();
	CVS_Scene* createNewScene();
};

#endif