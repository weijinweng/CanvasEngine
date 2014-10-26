#include "CVS_World.h"
#include "Canvas.h"

CVS_GameComponent::CVS_GameComponent(CVS_GameObject* object):object(object)
{
}

CVS_RenderComponent::CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene):CVS_GameComponent(object)
{
	//Lower priority will use enum later.
	priority = 10;
	node = scene->createNewNode();
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
	CVS_GameObject* newObject = new CVS_GameObject();
	CVS_RenderComponent* newComponent = new CVS_RenderComponent(newObject, scene);

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
	for(int i = 0, e = scene->mRootNode->mNumChildren; i < e; ++i)
	{
		objects.push_back(createGameObjectsFromaiNodes(scene->mRootNode->mChildren[i], meshes));
	}
	return true;
}

CVS_Scene::CVS_Scene(CVS_WorldSystem* system, CVS_RenderScene* Scene):system(system), scene(Scene)
{

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