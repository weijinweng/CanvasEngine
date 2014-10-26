#include "CVS_World.h"
#include "Canvas.h"

CVS_GameComponent::CVS_GameComponent(CVS_GameObject* object):object(object)
{
}

CVS_RenderComponent::CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene):CVS_GameComponent(object)
{
	//Lower priority will use enum later.
	priority = 10;
	if(scene == NULL)
	{
		printf("Okay so wtf is going on\n");
	}
	node = scene->createNewNode();
	object->addComponent(this);
}

void CVS_RenderComponent::Update()
{
	node->modelMatrix = object->transformNode.transform.transform;
}

CVS_GameObject::CVS_GameObject(std::string name):name(name)
{
}

void CVS_GameObject::addComponent(CVS_GameComponent* component)
{
	if(components.begin() == components.end())
	{
		components.push_back(component);
		return;
	}
	//Insert components by priority. Priority are used to determin update time.
	//E.g physics should update first, then rendernode should get the transform.
	for(auto it = components.begin(), et = components.end(); it!=et; it++)
	{
		if((*it)->priority <= component->priority)
		{
			components.insert(it, component);
			return;
		}
	}
	components.push_back(component);
}

void CVS_GameObject::addChildren(CVS_GameObject* child)
{
	this->children.push_back(child);
}

bool CVS_Scene::loadFile(char* filepath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_FlipUVs|aiProcess_GenSmoothNormals);
	if(scene == NULL)
	{
		printf("Assimp load error\n");
		return false;
	}
	std::vector<CVS_Mesh*> meshes = GLOBALSTATEMACHINE.m_RenderSub.addMeshesFromaiScene(scene);

	return initializeGameObjectsFromaiScene(scene, meshes); 
}

CVS_GameObject* CVS_Scene::createGameObjectsFromaiNodes(const aiNode* node, std::vector<CVS_Mesh*> meshes)
{
	printf("Creating game objects %s\n", node->mName.data);
	CVS_GameObject* newObject = new CVS_GameObject();
	newObject->name = std::string(node->mName.data);
	CVS_RenderComponent* newComponent = new CVS_RenderComponent(newObject, this->scene);
	printf("memory initialized\n");
	newComponent->node->setMesh(meshes[node->mMeshes[0]]);
	for(int i = 0, e = node->mNumChildren; i < e; ++i)
	{
		newObject->addChildren(this->createGameObjectsFromaiNodes(node->mChildren[i], meshes));
	}

	cmat4 assMatrix(1.0f);

	copyAiMatrixToGLM(&node->mTransformation, assMatrix);

	glm::vec4 transform(0,0,0,1);
	transform = assMatrix*transform;
	//get translate
	newObject->transformNode.transform.translation.x = transform.x;
	newObject->transformNode.transform.translation.y = transform.y;
	newObject->transformNode.transform.translation.z = transform.z;
	printf("translate x%f y%f z%f\n", transform.x, transform.y, transform.z);
	//get scale
	transform = glm::vec4(1,0,0,1);

	float scale = 1;
	transform = assMatrix * transform;
	scale = glm::length(glm::vec3(transform.x, transform.y, transform.z) - newObject->transformNode.transform.translation)/scale;
	newObject->transformNode.transform.scale *= scale;
	printf("scale is %f\n", scale);

	glm::mat4 rotationMatrix = glm::translate(assMatrix, newObject->transformNode.transform.translation * (-1.0f));

	rotationMatrix = glm::scale(rotationMatrix, glm::vec3(1/scale));

	glm::quat rotation = glm::quat_cast(rotationMatrix);
		/*transform -= glm::vec4(this->transform.translation,1);
		transform2 -= glm::vec4(this->transform.translation,1);

		float angx = transform.x/(std::sqrt(std::pow(transform.x,2) + std::pow(transform.z,2));
		float angy = 

		glm::vec3 v(1.0f,0.0f,0.0f);
		glm::vec3 u(transform.x, transform.y, transform.z);
		*/
		/*
		float cos_theta = glm::dot(glm::normalize(u), glm::normalize(v));
		float angle = glm::acos(cos_theta);
		glm::vec3 w = glm::normalize(glm::cross(u,v));
		glm::quat rotation(angle,w);
		glm::quat
		*/
		/*
		glm::vec3 w = glm::cross(u, v);
		glm::quat q = glm::quat(1.f + glm::dot(u, v), w.x, w.y, w.z);
		glm::fquat rotation(glm::normalize(q));*/



//	glm::fquat rotation(glm::vec3(transform.x,transform.y,transform.z), glm::vec3(1,0,0));
	newObject->transformNode.transform.orientation = rotation;
	return newObject;
}

bool CVS_Scene::initializeGameObjectsFromaiScene(const aiScene* scene, std::vector<CVS_Mesh*> meshes)
{
	printf("Loading scene\n");
	for(int i = 0, e = scene->mRootNode->mNumChildren; i < e; ++i)
	{
		objects.push_back(createGameObjectsFromaiNodes(scene->mRootNode->mChildren[i], meshes));
	}
	return true;
}

CVS_Scene::CVS_Scene(CVS_WorldSystem* system, CVS_RenderScene* Scene):system(system), scene(Scene)
{

}

void CVS_Scene::Update()
{
	for(int i = 0, e = this->objects.size(); i < e; ++i)
	{

	}
}

bool CVS_WorldSystem::Initialize()
{
	return true;
}

bool CVS_WorldSystem::Update()
{
	for(int i = 0; i < scenes.size(); ++i)
	{
		scenes[i]->Update();
	}
	
	return true;
}

bool CVS_WorldSystem::End()
{
	return true;
}

CVS_Scene* CVS_WorldSystem::createnewScene()
{
	CVS_RenderScene* render = GLOBALSTATEMACHINE.m_RenderSub.createNewScene();
	if(render == NULL)
	{
		printf("Error CREATING NEW SCENE\n");
	}
	scenes.push_back(new CVS_Scene(this, render));
	return scenes.back();
}