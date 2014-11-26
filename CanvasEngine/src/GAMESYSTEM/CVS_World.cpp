#include "Canvas.h"
#include "CVS_World.h"

//TODO: remove this include
#include "CVS_Mesh.h"

CVS_GameComponent::CVS_GameComponent(CVS_GameObject* object) :object(object)
{

}

CVS_RenderComponent::CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene) :CVS_GameComponent(object)
{
	this->m_pNode = scene->createNewNode();
	this->m_pNode->msgData = this;
	printf("%p\n", this);
	this->priority;
}

void CVS_RenderComponent::Update()
{
	this->m_pNode->modelMatrix = object->transformNode.transform.transform;
	this->m_pNode->mesh->UpdateSkeleton(this->m_pNode);
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
	
	RenderComponent->m_pNode->setMesh(meshes[node->mMeshes[0]]);
	
	this->addComponent(RenderComponent);

	printf("Created GameObject %s\n", name.c_str());
}

CVS_GameObject::CVS_GameObject(CVS_RenderScene* scene, std::vector<CVS_Mesh*>)
{

}

CVS_GameObject::CVS_GameObject(std::string _name /*= "Object"*/) : name(_name)
{
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
	UpdateTransformMatrix();

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
	this->m_pScene = renderer;
	this->m_pWorld = system;
}

bool CVS_WorldSystem::Initialize()
{
	return true;
}

bool CVS_WorldSystem::Update()
{
	for (auto i : this->scenes)
	{
		for (auto j : i->m_objects)
		{
			j->Update();
		}
	}
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