#include "Canvas.h"
#include "CVS_World.h"

CVS_GameComponent::CVS_GameComponent(CVS_GameObject* object) :object(object)
{

}

CVS_RenderComponent::CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene) :CVS_GameComponent(object)
{
	this->name = "RenderComponent";
	this->node = scene->createNewNode();
	this->node->msgData = this;
	printf("%p\n", this);
	this->priority;
}

void CVS_RenderComponent::Update()
{
	this->node->modelMatrix = object->transformNode.transform.transform;
}

CVS_GameObject::CVS_GameObject(const aiNode* node,
								CVS_RenderScene* scene,
								std::vector<CVS_Mesh*> meshes)
{
	for (int i = 0, e = node->mNumChildren; i < e; ++i)
	{
		this->children.push_back(new CVS_GameObject(node->mChildren[i], scene, meshes));
	}
	this->name = std::string(node->mName.C_Str());
	CVS_RenderComponent* RenderComponent = new CVS_RenderComponent(this, scene);
	
	RenderComponent->node->setMesh(meshes[node->mMeshes[0]]);
	
	this->addComponent(RenderComponent);

	printf("Created GameObject %s\n", name.c_str());
}

void CVS_GameObject::UpdateTransformMatrix()
{
	this->transformNode.transform.calculateMatrix();
}

void CVS_GameObject::addChildren(CVS_GameObject* child)
{
	this->children.push_back(child);
	transformNode.children.push_back(&child->transformNode);
	child->setParent(this);
}

void CVS_GameObject::setParent(CVS_GameObject* parent)
{
	if (this->parent != NULL)
	{
		this->parent->removeChildren(this);
	}
	this->parent = parent;

	transformNode.parent = &parent->transformNode;
}

CVS_GameComponent* CVS_GameObject::getComponent(std::string cname)
{
	for (int i = 0, e = components.size(); i < e; ++i)
	{
		if (components[i]->name == cname)
		{
			return components[i];
		}
	}
	printf("Shit got nothing here\n");
	return NULL;
}

void CVS_GameObject::removeChildren(CVS_GameObject* child)
{
	for (auto it = children.begin(), e = children.end(); it != e; it++)
	{
		if ((*it) == child)
		{
			children.erase(it);
			break;
		}
	}

	for (auto it = transformNode.children.begin(), et = transformNode.children.end(); it != et; it++)
	{
		if ((*it) == &child->transformNode)
		{
			transformNode.children.erase(it);
			break;
		}
	}
	printf("Removed child %s\n", child->name.c_str());
}

void CVS_GameObject::Update()
{
	for (int i = 0, e = components.size(); i < e; ++i)
	{
		components[i]->Update();
	}
}

void CVS_GameObject::addComponent(CVS_GameComponent* component)
{
	for (auto it = components.begin(), et = components.end(); it != et; it++)
	{
		if (component->priority <= (*it)->priority)
		{
			components.insert(it, component);
			return;
		}
	}
	components.push_back(component);
}
CVS_Scene::CVS_Scene(CVS_WorldSystem* system, CVS_RenderScene* renderer)
{
	this->scene = renderer;
	this->system = system;
}

bool CVS_Scene::loadFile(char* filename)
{
	Assimp::Importer importer;
	const aiScene* aiscene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	std::vector<CVS_Mesh*> meshes = GLOBALSTATEMACHINE.m_RenderSub.addMeshesFromaiScene(aiscene);
	
	if (aiscene == NULL)
		return false;

	for(int i = 0, e = aiscene->mRootNode->mNumChildren; i < e; ++i)
	{
		CVS_GameObject* newObject = new CVS_GameObject(aiscene->mRootNode->mChildren[i], this->scene, meshes);
		this->objects.push_back(newObject);
	}
	return true;
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