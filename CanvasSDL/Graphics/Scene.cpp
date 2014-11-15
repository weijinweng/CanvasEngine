#include "Scene.h"
#include "RenderProgram.h"

void printAiNode(const aiNode* node, int depth)
{
	printf("aiNode with meshes%d\n", node->mNumMeshes);
	for (int i = 0; i < node->mNumChildren; i++)
	{
		printf("-");
		for (int j = 0; j < depth; j++)
		{
			printf("-");
		}
		printAiNode(node->mChildren[i], depth + 1);
	}
}

namespace Canvas
{
	Scene::Scene(RenderSys* parent) :parentSys(parent), active_Lights_num(0)
	{
		this->renderables.reserve(20);
		this->getRenderProgram("Standard");
		this->setShadowShader();
		printf("initialization capacity %d\n", renderables.capacity());
	}

	Scene::Scene(RenderSys* parent, char* filepath) :parentSys(parent), active_Lights_num(0)
	{
		this->renderables.reserve(20);
		this->getRenderProgram("Standard");
		this->loadFromFile(filepath);
	}

	std::vector<CVS_Mesh*> Scene::loadFBXScene(char* filepath)
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
			lGeomConverter.Triangulate(scene, /*replace*/true);

			// Split meshes per material, so that we only have one material per mesh (for VBO support)
			lGeomConverter.SplitMeshesPerMaterial(scene, /*replace*/true);


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

	std::vector<CVS_Mesh*> Scene::loadFromFile(char* filepath)
	{
		std::string file = std::string(filepath);
		std::string extension = file.substr(file.find_last_of('.'), file.size());
		printf("%s\n", extension.c_str());
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (extension == ".fbx")
		{
			printf("%s\n", extension.c_str());
			return loadFBXScene(filepath);
		}
		if (parentSys == NULL)
		{
			printf("Error parentSys does not exists\n");
		}
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filepath, aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
		if (scene)
		{
			return initSceneFromAiScene(scene);
		}

		std::vector<CVS_Mesh*> meshes;

		return meshes;
	}

	void Scene::generateLightBlock()
	{
		glGenBuffers(1, &light_ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);

		const GLchar* uniformNames[1] = {
			"Lights.light"
		};

		GLuint uniformIndices;

		GLuint program = programs[0].program->programID;

		glGetUniformIndices(programs[0].program->programID, 1, uniformNames, &uniformIndices);

		GLint uniformOffsets[1];
		glGetActiveUniformsiv(program, 1, &uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);

		GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "Lights");

		printf("light index is%d\n", uniformBlockIndex);



		GLsizei uniformBlockSize(0);
		glGetActiveUniformBlockiv(program, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

		printf("uniform block size %d\n", uniformBlockSize);

		const GLchar *names[] =
		{
			"Lights.light[0].position",
			"Lights.light[0].ambient",
			"Lights.light[0].diffuse",
			"Lights.light[0].specular",
			"Lights.light[0].constant_attenuation",
			"Lights.light[0].linear_attenuation",
			"Lights.light[0].quadratic_attenuation",
			"Lights.light[0].spot_direction",
			"Lights.light[0].spot_cutoff",
			"Lights.light[0].spot_exponent",
			"Lights.light[0].type",
			"Lights.light[1].position",
			"Lights.light[1].ambient",
			"Lights.light[1].diffuse",
			"Lights.light[1].specular",
			"Lights.light[1].constant_attenuation",
			"Lights.light[1].linear_attenuation",
			"Lights.light[1].quadratic_attenuation",
			"Lights.light[1].spot_direction",
			"Lights.light[1].spot_cutoff",
			"Lights.light[1].spot_exponent",
			"Lights.light[1].type",
			"Lights.light[2].position",
			"Lights.light[2].ambient",
			"Lights.light[2].diffuse",
			"Lights.light[2].specular",
			"Lights.light[2].constant_attenuation",
			"Lights.light[2].linear_attenuation",
			"Lights.light[2].quadratic_attenuation",
			"Lights.light[2].spot_direction",
			"Lights.light[2].spot_cutoff",
			"Lights.light[2].spot_exponent",
			"Lights.light[2].type",
			"Lights.light[3].position",
			"Lights.light[3].ambient",
			"Lights.light[3].diffuse",
			"Lights.light[3].specular",
			"Lights.light[3].constant_attenuation",
			"Lights.light[3].linear_attenuation",
			"Lights.light[3].quadratic_attenuation",
			"Lights.light[3].spot_direction",
			"Lights.light[3].spot_cutoff",
			"Lights.light[3].spot_exponent",
			"Lights.light[3].type",
			"Lights.light[4].position",
			"Lights.light[4].ambient",
			"Lights.light[4].diffuse",
			"Lights.light[4].specular",
			"Lights.light[4].constant_attenuation",
			"Lights.light[4].linear_attenuation",
			"Lights.light[4].quadratic_attenuation",
			"Lights.light[4].spot_direction",
			"Lights.light[4].spot_cutoff",
			"Lights.light[4].spot_exponent",
			"Lights.light[4].type",
			"Lights.light[5].position",
			"Lights.light[5].ambient",
			"Lights.light[5].diffuse",
			"Lights.light[5].specular",
			"Lights.light[5].constant_attenuation",
			"Lights.light[5].linear_attenuation",
			"Lights.light[5].quadratic_attenuation",
			"Lights.light[5].spot_direction",
			"Lights.light[5].spot_cutoff",
			"Lights.light[5].spot_exponent",
			"Lights.light[5].type",
			"Lights.light[6].position",
			"Lights.light[6].ambient",
			"Lights.light[6].diffuse",
			"Lights.light[6].specular",
			"Lights.light[6].constant_attenuation",
			"Lights.light[6].linear_attenuation",
			"Lights.light[6].quadratic_attenuation",
			"Lights.light[6].spot_direction",
			"Lights.light[6].spot_cutoff",
			"Lights.light[6].spot_exponent",
			"Lights.light[6].type",
			"Lights.light[7].position",
			"Lights.light[7].ambient",
			"Lights.light[7].diffuse",
			"Lights.light[7].specular",
			"Lights.light[7].constant_attenuation",
			"Lights.light[7].linear_attenuation",
			"Lights.light[7].quadratic_attenuation",
			"Lights.light[7].spot_direction",
			"Lights.light[7].spot_cutoff",
			"Lights.light[7].spot_exponent",
			"Lights.light[7].type",
			"Lights.light[8].position",
			"Lights.light[8].ambient",
			"Lights.light[8].diffuse",
			"Lights.light[8].specular",
			"Lights.light[8].constant_attenuation",
			"Lights.light[8].linear_attenuation",
			"Lights.light[8].quadratic_attenuation",
			"Lights.light[8].spot_direction",
			"Lights.light[8].spot_cutoff",
			"Lights.light[8].spot_exponent",
			"Lights.light[8].type",
			"Lights.light[9].position",
			"Lights.light[9].ambient",
			"Lights.light[9].diffuse",
			"Lights.light[9].specular",
			"Lights.light[9].constant_attenuation",
			"Lights.light[9].linear_attenuation",
			"Lights.light[9].quadratic_attenuation",
			"Lights.light[9].spot_direction",
			"Lights.light[9].spot_cutoff",
			"Lights.light[9].spot_exponent",
			"Lights.light[9].type"
		};

		GLuint indices[110];

		glGetUniformIndices(program, 110, names, indices);

		/*for(int i = 0; i < 110; ++i)
		{
		printf("Indice index %d with value %d\n",i, indices[i]);
		}*/

		std::vector<GLint> lightUniformOffsets(110);

		glGetActiveUniformsiv(program, lightUniformOffsets.size(), indices, GL_UNIFORM_OFFSET, &lightUniformOffsets[0]);


		GLint* offsets = &lightUniformOffsets[0];

		const unsigned int uboSize(uniformBlockSize);
		std::vector<char> buffer(uboSize);

		int offset;



		/*(for(int i = 0; i < 110;++i)
		{
		printf("offset for index %d is %d\n", i, offsets[i]);
		}
		*/
		for (unsigned int n = 0, e = 10; n < e; ++n)
		{
			printf("lighting it up\n");
			// Light position (vec4)
			offset = offsets[0 + n * 11];
			for (int i = 0; i < 4; ++i)
			{
				*(reinterpret_cast<float*> (&buffer[0] + offset)) =
					lights[n].properties.position[i];
				offset += sizeof (GLfloat);
			}
			// Light ambient color (vec4)
			offset = offsets[1 + n * 11];
			for (int i = 0; i < 4; ++i)
			{
				*(reinterpret_cast<float*> (&buffer[0] + offset)) =
					lights[n].properties.ambient[i];
				offset += sizeof (GLfloat);
			}
			// Light diffuse color (vec4)
			offset = offsets[2 + n * 11];
			for (int i = 0; i < 4; ++i)
			{
				*(reinterpret_cast<float*> (&buffer[0] + offset)) =
					lights[n].properties.diffuse[i];
				offset += sizeof (GLfloat);
			}
			// Light specular color (vec4)
			offset = offsets[3 + n * 11];
			for (int i = 0; i < 4; ++i)
			{
				*(reinterpret_cast<float*> (&buffer[0] + offset)) =
					lights[n].properties.specular[i];
				offset += sizeof (GLfloat);
			}
			// Light constant attenuation (float)
			offset = offsets[4 + n * 11];
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n].properties.constAttenuation;
			// Light linear attenuation (float)
			offset = offsets[5 + n * 11];
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n].properties.linearAttentuation;
			// Light quadratic attenuation (float)
			offset = offsets[6 + n * 11];
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n].properties.quadraticAttentuation;
			// Light spot direction (vec3)
			offset = offsets[7 + n * 11];
			for (int i = 0; i < 3; ++i)
			{
				*(reinterpret_cast<float*> (&buffer[0] + offset)) =
					lights[n].properties.spotDirection[i];
				offset += sizeof (GLfloat);
			}
			// Light spot cutoff (float)
			offset = offsets[8 + n * 11];
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n].properties.spotCosCutoff;
			// Light spot exponent (float)
			offset = offsets[9 + n * 11];
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n].properties.spotExponent;

			offset = offsets[10 + n * 11];
			*(reinterpret_cast<int*> (&buffer[0] + offset)) =
				lights[n].properties.type;
		}
		glBufferData(GL_UNIFORM_BUFFER, uboSize, &buffer[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, light_ubo);
		glUniformBlockBinding(program, uniformBlockIndex, 0);
		for (int i = 0, e = programs.size(); i < e; ++i)
		{

			program = programs[i].program->programID;
			uniformBlockIndex = glGetUniformBlockIndex(program, "Lights");
			glUniformBlockBinding(program, uniformBlockIndex, 0);
		}
	}

	transformNode* Scene::addRenderNodesFromAiNodes(aiNode* child, std::vector<CVS_Mesh*> meshes)
	{
		static renderNode* nodepointer = 0;
		renderables.push_back(renderNode());
		renderNode* ournode = &(renderables.back());
		printf("The capacity is %d\n", renderables.capacity());
		for (int i = 0, e = child->mNumChildren; i < e; ++i)
		{
			ournode->transform.addChild(addRenderNodesFromAiNodes(child->mChildren[i], meshes));
		}

		ournode->initFromRenderNode(child, meshes);
		programs[0].addNode(ournode);
		printf("node name is %s\n", ournode->name.c_str());
		if (nodepointer != &renderables[0])
			printf("secondary error found!\n");
		nodepointer = &renderables[0];
		if (&renderables[0] != programs[0].childNodes[0])
			printf("error found!\n");

		return &ournode->transform;
	}

	transformNode* Scene::addRenderNodesFromFbxNodes(FbxNode* child, std::vector<CVS_Mesh*> meshes)
	{
		static renderNode* pRenderNode = 0;

		renderables.push_back(renderNode());
		renderNode* pNode = &(renderables.back());

		printf("The capacity is %d\n", renderables.capacity());
		for (int i = 0, e = child->GetChildCount(); i < e; ++i)
		{
			pNode->transform.addChild(addRenderNodesFromFbxNodes(child->GetChild(i), meshes));
		}

		pNode->initFromRenderNode(child, meshes);
		programs[0].addNode(pNode);
		printf("node name is %s\n", pNode->name.c_str());
		if (pRenderNode != &renderables[0])
			printf("secondary error found!\n");
		pRenderNode = &renderables[0];
		if (&renderables[0] != programs[0].childNodes[0])
			printf("error found!\n");

		return &pNode->transform;
	}

	renderNode* Scene::getNode(std::string name)
	{
		for (int i = 0, e = renderables.size(); i < e; ++i)
		{
			if (renderables[i].name == name)
				return &renderables[i];
		}

		printf("Error finding %s\n", name.c_str());
		return NULL;
	}

	void Scene::initRenderNodesFromAiScene(aiNode* root, std::vector<CVS_Mesh*> meshes)
	{

		for (int i = 0, e = root->mNumChildren; i < e; ++i)
		{
			this->root.addChild(addRenderNodesFromAiNodes(root->mChildren[i], meshes));
		}
	}

	std::vector<CVS_Mesh*> Scene::initSceneFromAiScene(const aiScene* scene)
	{
		std::vector<CVS_Mesh*> meshes = parentSys->initMeshesFromAiScene(scene->mMeshes, scene->mNumMeshes);

		printf("renderable capacity %d\n", renderables.capacity());

		initRenderNodesFromAiScene(scene->mRootNode, meshes);

		printAiNode(scene->mRootNode, 0);
		return meshes;
	}

	std::vector<CVS_Mesh*> Scene::initSceneFromFbxScene(const FbxScene* _scene)
	{
		auto pRootNode = _scene->GetRootNode();
		std::vector<CVS_Mesh*> meshes = parentSys->initMeshesFromFbxNode(pRootNode);
		printf("renderable capacity %d\n", renderables.capacity());

		for (int i = 0, e = meshes.size(); i < e; ++i)
		{
			this->root.addChild(addRenderNodesFromFbxNodes(pRootNode->GetChild(i), meshes));
		}
		return meshes;
	}


	void Scene::Render()
	{
		Render(0);
	}

	void Scene::Render(int index)
	{
		if ((cameras.size() == 0))
		{
			printf("No Cameras in scene! add one before rendering\n");
			return;
		}
		root.calculateAllMatrices();

		generateShadow();

		for (int i = 0, e = programs.size(); i < e; ++i)
		{
			programs[i].Render(cameras[index], active_Lights_num, lights);
		}
	}

	void Scene::setShadowShader(std::string name)
	{
		shadowProgram = parentSys->getProgram(name);
	}

	Scene::~Scene()
	{}

	void Scene::getRenderProgram(std::string name)
	{
		this->programs.push_back(renderProgramID(parentSys->getProgram(name)));
	}

	void Scene::initializeShadowMap(int lightIndex)
	{
		glGenFramebuffers(1, &shadowMapFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);

		glGenTextures(1, &lights[lightIndex].shadowTexture);
		glBindTexture(GL_TEXTURE_2D, lights[lightIndex].shadowTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 2000, 1500, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights[lightIndex].shadowTexture, 0);
		glDrawBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("shadowInitialization error!!!!!\n");
			return;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Scene::generateShadow()
	{
		glUseProgram(shadowProgram->programID);

		GLuint depthMatrixID = glGetUniformLocation(shadowProgram->programID, "depthMVP");

		GLuint program = shadowProgram->programID;

		glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFrameBuffer);
		glViewport(0, 0, 2000, 1500);
		for (int i = 0, e = 10; i < e; ++i)
		{
			if (lights[i].shadow && lights[i].properties.type == 2)
			{
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights[i].shadowTexture, 0);

				glDrawBuffer(GL_NONE);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
				glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(lights[i].properties.position.x, lights[i].properties.position.y, lights[i].properties.position.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				lights[i].depthVP = depthProjectionMatrix * depthViewMatrix;
				for (int j = 0, je = renderables.size(); j < je; ++j)
				{
					if (renderables[j].name == "Plane001")
						continue;
					glm::mat4 depthModelMatrix = renderables[j].transform.transformMatrix;
					glm::mat4 depthMVP = lights[i].depthVP * depthModelMatrix;


					glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, glm::value_ptr(depthMVP));


					for (int k = 0, ke = renderables[j].meshes.size(); k < ke; ++k)
					{
						glBindVertexArray(renderables[j].meshes[k]->VAO);

						glDrawElements(GL_TRIANGLES, renderables[j].meshes[k]->indices.size(), GL_UNSIGNED_INT, 0);
					}
				}
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1200, 900);
	}

	void Scene::testShadowMap()
	{
		root.calculateAllMatrices();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(shadowProgram->programID);

		GLuint depthMatrixID = glGetUniformLocation(shadowProgram->programID, "depthMVP");

		if (depthMatrixID == -1)
		{
			printf("error");
		}

		glm::vec3 pointLightDir = glm::vec3(0.5f, 2, 2);

		GLuint program = shadowProgram->programID;

		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);

		glm::mat4 depthViewMatrix = glm::lookAt(pointLightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0f);
		depthModelMatrix = glm::scale(depthModelMatrix, glm::vec3(0.02, 0.02, 0.02));
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
		glUniformMatrix4fv(program, 1, GL_FALSE, glm::value_ptr(depthMVP));


		for (int j = 0, je = renderables.size(); j < je; ++j)
		{

			for (int k = 0, ke = renderables[j].meshes.size(); k < ke; ++k)
			{
				glBindVertexArray(renderables[j].meshes[k]->VAO);
				glDrawElements(GL_TRIANGLES, renderables[j].meshes[k]->indices.size(), GL_UNSIGNED_INT, 0);
			}
		}
	}

	void Scene::activateShadow(std::string name)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (name == lights[i].name)
			{
				lights[i].shadow = true;
				initializeShadowMap(i);
			}
		}
	}

	Light* Scene::activateLight(int index, std::string name)
	{
		lights[index].name = name;
		lights[index].active = true;
		this->active_Lights_num++;
		printf("Light activated!!\n");
		return &lights[index];
	}

	void Scene::deactivateLight(int index)
	{
		lights[index].name = std::string();
		lights[index].active = false;
	}

	Light* Scene::addSpotLight(std::string name, glm::vec3 position, glm::vec3 target, float cosCutoff)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (lights[i].active != true)
			{
				Light* light = activateLight(i, name);
				light->spotLight(position, target, cosCutoff);
				return light;
			}
		}

		printf("Out of lights!!\n");
		return NULL;
	}

	Light* Scene::addDirectionalLight(std::string name, glm::vec3 direction)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (lights[i].active != true)
			{
				Light* light = activateLight(i, name);
				light->directionalLight(direction);
				return light;
			}
		}

		printf("Out of lights!!\n");
		return NULL;
	}

}