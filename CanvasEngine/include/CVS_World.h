#ifndef CVS_WORLD_H
#define CVS_WORLD_H
#include CVS_PRECOMPILED_H
#include "CVS_Transform.h"
#include "CVS_RenderSystem.h"

class CVS_GameObject;
struct CVS_WorldSystem;

struct CVS_GameComponent{
	std::string name;
	int priority;
	CVS_GameObject* object;
	CVS_GameComponent(CVS_GameObject* object);
	virtual void Update() = 0;
};

struct CVS_CameraComponent:public CVS_GameComponent{
	CVS_Camera* cam;
	CVS_CameraComponent(CVS_GameObject* object, CVS_Camera* cam);
};

struct CVS_RenderComponent:public CVS_GameComponent{
	CVS_RenderNode* node;
	CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene);
	void Update();
};

class CVS_GameObject{
public:
	std::string name;
	CVS_TransformNode transformNode;
	std::vector<CVS_GameComponent*> components;
	std::vector<CVS_GameObject*> children;
	CVS_GameObject(std::string name = "Object");
	void addChildren(CVS_GameObject* child);
	void removeChildren(CVS_GameObject* child);
	void addComponent(CVS_GameComponent* component);
};

struct CVS_Scene{
public:
	std::vector<CVS_GameObject*> objects;
	CVS_WorldSystem* system;
	CVS_RenderScene* scene;
	CVS_Scene(CVS_WorldSystem* system, CVS_RenderScene* scene);
	void Update();
	bool loadFile(char* filepath);
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
	CVS_Scene* createnewScene();
};

#endif