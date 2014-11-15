#include "Canvas.h"
#include "CVS_World.h"

CVS_GameComponent::CVS_GameComponent(CVS_GameObject* object) :object(object)
{

}

CVS_RenderComponent::CVS_RenderComponent(CVS_GameObject* object, CVS_RenderScene* scene) :CVS_GameComponent(object)
{
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

/*
std::vector<CVS_Mesh*> CVS_Scene::loadFBXScene(char* filepath)
{
	FbxIOSettings* ios = FbxIOSettings::Create(parentSys->fbxManager, IOSROOT);
	parentSys->fbxManager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(parentSys->fbxManager, "");

	// Make sure it's actually .fbx
	int lFileFormat = -1;
	if (!parentSys->fbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(filepath, lFileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = parentSys->fbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
	}

	bool importerStatus = importer->Initialize(filepath, -1, parentSys->fbxManager->GetIOSettings());

	if (!importerStatus)
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
		exit(-1);
	}

	bool hr = 0;
	auto scene = FbxScene::Create(parentSys->fbxManager, "My scene");
	if (!scene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}

	if (importer->Import(scene) == true)
	{
		// Set the scene status flag to refresh 
		// the scene in the first timer callback.
		//mStatus = MUST_BE_REFRESHED;

		// Convert Axis System to what is used in this example, if needed
		FbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(scene);
		}

		// Convert Unit System to what is used in this example, if needed
		// 		FbxSystemUnit SceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
		// 		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		// 		{
		// 			//The unit in this example is centimeter.
		// 			FbxSystemUnit::cm.ConvertScene(scene);
		// 		}

		FbxArray<FbxString*> mAnimStackNameArray;

		// Get the list of all the animation stack.
		scene->FillAnimStackNameArray(mAnimStackNameArray);

		// Get the list of all the cameras in the scene.
		//FillCameraArray(scene, mCameraArray);

		// Convert mesh, NURBS and patch into triangle mesh
		FbxGeometryConverter lGeomConverter(parentSys->fbxManager);
		lGeomConverter.Triangulate(scene, true);

		// Split meshes per material, so that we only have one material per mesh (for VBO support)
		lGeomConverter.SplitMeshesPerMaterial(scene, true);


		//LoadCacheRecursive(scene, mCurrentAnimLayer, mFileName, mSupportVBO);

		// Convert any .PC2 point cache data into the .MC format for 
		// vertex cache deformer playback.
		//PreparePointCacheData(scene, mCache_Start, mCache_Stop);

		// Get the list of pose in the scene
		//FillPoseArray(scene, mPoseArray);

		// Initialize the frame period.
		//mFrameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());

		hr = true;
	}
	else
	{
		printf("Unable to import file %s.\n", filepath);
		printf("Error reported: ", importer->GetStatus().GetErrorString());
	}
	importer->Destroy();

	return initSceneFromFbxScene(scene);
}

*/
bool CVS_Scene::loadFile(char* filename)
{
	std::string file = filename;
	std::string extension = file.substr(file.find_last_of('.'), file.size());
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (extension == ".fbx")
	{
		printf("CVS_Scene: Loading %s.\n", filename);
		//return loadFBXScene(filename);
	}
	else
	{
		Assimp::Importer importer;
		const aiScene* aiscene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
		std::vector<CVS_Mesh*> meshes = GLOBALSTATEMACHINE.m_RenderSub.addMeshesFromaiScene(aiscene);

		for (int i = 0, e = aiscene->mRootNode->mNumChildren; i < e; ++i)
		{
			CVS_GameObject* newObject = new CVS_GameObject(aiscene->mRootNode->mChildren[i], this->scene, meshes);
		}
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