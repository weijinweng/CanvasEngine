#include "Canvas.h"
#include "CVS_Mesh.h"

CVS_FrameBuffer::CVS_FrameBuffer()
{
	glGenFramebuffers(1, &buffer);
}

void CVS_FrameBuffer::Bind(GLenum target)
{
	glBindFramebuffer(target, buffer);
	this->flag = target;
}

void CVS_FrameBuffer::unBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CVS_FrameBuffer::BindColorAttachment(CVS_Texture* texture, UINT location)
{
	texture->Bind();
	glFramebufferTexture2D(flag, GL_COLOR_ATTACHMENT0 + location, texture->target, texture->textureID, 0);
}

void CVS_FrameBuffer::BindDepthAttachment(CVS_Texture* texture)
{
	texture->Bind();
	glFramebufferTexture2D(flag, GL_DEPTH_ATTACHMENT, texture->target, texture->textureID, 0);
}

void CVS_FrameBuffer::setDrawBuffer(GLenum flag)
{
	glDrawBuffer(flag);
}

void CVS_FrameBuffer::setReadBuffer(GLenum flag)
{
	glReadBuffer(flag);
}

bool CVS_FrameBuffer::GetBufferStatus()
{
	GLenum status = glCheckFramebufferStatus(flag);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	return true;
}

void BindColorAttacment(CVS_Texture* texture, UINT location)
{

}
void CVS_RenderPipeline::SetUp()
{

}

void CVS_RenderPipeline::Render(CVS_RenderScene* scene, CVS_View view)
{
	scene->Draw(&view);
}


CVS_Renderer::CVS_Renderer(HDC glHdc)
{
	wglMakeCurrent(glHdc, GLOBALSTATEMACHINE.m_RenderSub.m_glContext);
	m_ParentHDC = glHdc;
}

void CVS_Renderer::Render(CVS_RenderScene* scene, CVS_View View)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLOBALSTATEMACHINE.m_RenderSub.m_GridDraw->setAsCurrentProgram();
	glm::mat4 Mat = View.Pers * View.View * glm::mat4(1.0f);
	glUniformMatrix4fv(GLOBALSTATEMACHINE.m_RenderSub.m_GridMVP, 1, GL_FALSE, glm::value_ptr(Mat));
	glBindVertexArray(GLOBALSTATEMACHINE.m_RenderSub.gridVAO);
	glDrawArrays(GL_LINES, 0, GLOBALSTATEMACHINE.m_RenderSub.m_GridNum);
	glBindVertexArray(0);

	if (scene != NULL)
	{
		scene->Draw(&View);
	}

	if (SwapBuffers(m_ParentHDC) != TRUE)
	{
		printf("Error swap buffers\n");

	}


}

CVS_RenderSystem::CVS_RenderSystem() :m_glContext(NULL)
{
	this->pipeline = new CVS_RenderPipeline();
}

bool CVS_RenderSystem::Initialize()
{
	return true;
}

CVS_RenderProgram* CVS_RenderSystem::getRenderProgram(std::string name)
{
	if (programs.find(name) == programs.end())
	{
		return NULL;
	}
	return programs[name];
}

CVS_RenderProgram* CVS_RenderSystem::createNewShader(std::string name, char* file_vert_path, char* file_frag_path)
{
	if (programs.find(name) == programs.end())
	{
		CVS_RenderProgram* newProgram = new CVS_RenderProgram();

		newProgram->loadFile(file_vert_path, file_frag_path);
		programs[name] = newProgram;
		return newProgram;
	}
	printf("Program already exists\n");
	return programs[name];
}

CVS_Renderer* CVS_RenderSystem::createNewRenderer(HDC glHdc)
{
	if (m_glContext == NULL)
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
			32,                        //Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                        //Number of bits for the depthbuffer
			8,                        //Number of bits for the stencilbuffer
			0,                        //Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		GLuint pixelformat;

		if (!(pixelformat = ChoosePixelFormat(glHdc, &pfd)))
		{
			printf("Error choosed pixel format\n");
			return NULL;
		}

		if (SetPixelFormat(glHdc, pixelformat, &pfd) != TRUE)
		{
			printf("Error choosing pixel format\n");
			return NULL;
		}

		HGLRC temp;

		if (!(temp = wglCreateContext(glHdc)))
		{
			printf("Error getting glcontext\n");
			return NULL;
		}

		if (!(wglMakeCurrent(glHdc, temp)))
		{
			printf("Error making gl current\n");
			return NULL;
		}
		printf("Yay it worked\n");

		glewExperimental = GL_TRUE;

		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW %s\n", glewGetErrorString(glewError));
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_FLAGS_ARB,
			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		m_glContext = wglCreateContextAttribsARB(glHdc, NULL, attribs);

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(temp);
		wglMakeCurrent(glHdc, m_glContext);

		std::vector<cvec3> vertices;

		glGenVertexArrays(1, &gridVAO);
		glBindVertexArray(gridVAO);

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		float X = -1.0f, Y = 0.0f, Z = 1.0f;

		for (int i = 0; i < 11; ++i)
		{
			vertices.push_back(cvec3(X, Y, Z));
			vertices.push_back(cvec3(X, Y, -1.0f*Z));
			X += 0.2f;
		}

		X = 1.0f;
		Z = -1.0f;

		for (int i = 0; i < 11; ++i)
		{
			vertices.push_back(cvec3(X, Y, Z));
			vertices.push_back(cvec3(-1.0f * X, Y, Z));
			Z += 0.2f;
		}



		glBufferData(GL_ARRAY_BUFFER, sizeof(cvec3)* vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		glBindVertexArray(0);

		glDeleteBuffers(1, &vertexBuffer);

		m_GridDraw = createNewShader("DefaultGrid", "./Shaders/grid.vert", "./Shaders/grid.frag");

		m_GridMVP = glGetUniformLocation(m_GridDraw->programID, "MVP");

		if (m_GridMVP == -1)
		{
			printf("Error Uniform Invalid\n");
		}

		m_GridNum = vertices.size();

		wglSwapIntervalEXT(0);

		glEnable(GL_CULL_FACE);


		glEnable(GL_DEPTH_TEST);

		glClearColor(0.7, 0.7, 0.7, 1.0);



		this->pipeline = new CVS_RenderPipeline();
		this->pipeline->SetUp();
		createNewShader("Selection", "./Shaders/selection.vert", "./Shaders/selection.frag");
		this->m_DefaultTexture = createNewTexture("./Textures/Default.png");
	}



	CVS_Renderer* newRenderer = new CVS_Renderer(glHdc);
	this->renderers.push_back(newRenderer);
	return newRenderer;
}

void LoadNodeKeyframeAnimation(FbxScene* _scene, FbxNode* fbxNode)
{
	// 	bool isAnimated = false;
	// 
	// 	// Iterate all animations (for example, walking, running, falling and etc.)
	// 	int numAnimations = _scene->GetSrcObjectCount(FbxAnimStack::ClassId);
	// 	for (int animationIndex = 0; animationIndex < numAnimations; animationIndex++)
	// 	{
	// 		FbxAnimStack *animStack = (FbxAnimStack*)_scene->GetSrcObject(FbxAnimStack::ClassId, animationIndex);
	// 		FbxAnimEvaluator *animEvaluator = _scene->GetAnimationEvaluator();
	// 		animStack->GetName(); // Get the name of the animation if needed
	// 
	// 		// Iterate all the transformation layers of the animation. You can have several layers, for example one for translation, one for rotation, one for scaling and each can have keys at different frame numbers.
	// 		int numLayers = animStack->GetMemberCount();
	// 		for (int layerIndex = 0; layerIndex < numLayers; layerIndex++)
	// 		{
	// 			FbxAnimLayer *animLayer = (FbxAnimLayer*)animStack->GetMember(layerIndex);
	// 			animLayer->GetName(); // Get the layer's name if needed
	// 
	// 			FbxAnimCurve *translationCurve = fbxNode->LclTranslation.GetCurve(animLayer);
	// 			FbxAnimCurve *rotationCurve = fbxNode->LclRotation.GetCurve(animLayer);
	// 			FbxAnimCurve *scalingCurve = fbxNode->LclScaling.GetCurve(animLayer);
	// 
	// 			if (scalingCurve != 0)
	// 			{
	// 				int numKeys = scalingCurve->KeyGetCount();
	// 				for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
	// 				{
	// 					FbxTime frameTime = scalingCurve->KeyGetTime(keyIndex);
	// 					FbxDouble3 scalingVector = fbxNode->EvaluateLocalScaling(frameTime);
	// 					float x = (float)scalingVector[0];
	// 					float y = (float)scalingVector[1];
	// 					float z = (float)scalingVector[2];
	// 
	// 					float frameSeconds = (float)frameTime.GetSecondDouble(); // If needed, get the time of the scaling keyframe, in seconds
	// 				}
	// 			}
	// 			else
	// 			{
	// 				// If this animation layer has no scaling curve, then use the default one, if needed
	// 				FbxDouble3 scalingVector = fbxNode->LclScaling.Get();
	// 				float x = (float)scalingVector[0];
	// 				float y = (float)scalingVector[1];
	// 				float z = (float)scalingVector[2];
	// 			}
	// 
	// 			// Analogically, process rotationa and translation 
	// 		}
	// 	}
}

static double FrameRateToDouble(FbxTime::EMode fp, double customFPSVal = -1.0)
{
	switch (fp)
	{
	case FbxTime::EMode::eDefaultMode:
		return 1.0;

	case FbxTime::EMode::eFrames120:
		return 120.0;

	case FbxTime::EMode::eFrames100:
		return 100.0;

	case FbxTime::EMode::eFrames60:
		return 60.0;

	case FbxTime::EMode::eFrames50:
		return 50.0;

	case FbxTime::EMode::eFrames48:
		return 48.0;

	case FbxTime::EMode::eFrames30:
	case FbxTime::EMode::eFrames30Drop:
		return 30.0;

	case FbxTime::EMode::eNTSCDropFrame:
	case FbxTime::EMode::eNTSCFullFrame:
		return 29.9700262;

	case FbxTime::EMode::ePAL:
		return 25.0;

	case FbxTime::EMode::eFrames24:
		return 24.0;

	case FbxTime::EMode::eFrames1000:
		return 1000.0;

	case FbxTime::EMode::eFilmFullFrame:
		return 23.976;

	case FbxTime::EMode::eCustom:
		return customFPSVal;

	case FbxTime::EMode::eModesCount:
		break;
	}

	assert(0);
	return -1.0f; // this is to silence compiler warnings
}

static int InterpolationFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
	return 0;
}

static int ConstantmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
	return 0;
}

static int TangentmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
	if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
	if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
	return 0;
}

static int TangentweightFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
	return 0;
}

static int TangentVelocityFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
	return 0;
}

void DisplayCurveKeys(FbxAnimCurve* pCurve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = pCurve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);

		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}
/*
Returns meshes only, but processes animation, textures, and etc.
*/
//Standard curve node names
#define FBXSDK_CURVENODE_TRANSLATION	'T'
#define FBXSDK_CURVENODE_ROTATION		'R'
#define FBXSDK_CURVENODE_SCALING		'S'
#define FBXSDK_CURVENODE_COMPONENT_X	'X'
#define FBXSDK_CURVENODE_COMPONENT_Y	'Y'
#define FBXSDK_CURVENODE_COMPONENT_Z	'Z'
#define FBXSDK_CURVENODE_COLOR			'C'
#define FBXSDK_CURVENODE_COLOR_RED		FBXSDK_CURVENODE_COMPONENT_X
#define FBXSDK_CURVENODE_COLOR_GREEN	FBXSDK_CURVENODE_COMPONENT_Y
#define FBXSDK_CURVENODE_COLOR_BLUE		FBXSDK_CURVENODE_COMPONENT_Z
/*
std::vector<CVS_Mesh*> CVS_RenderSystem::populateRenderSceneFromFbxScene(FbxScene* _scene)
{
	FbxGlobalSettings& gSettings = _scene->GetGlobalSettings();

	std::vector<CVS_Mesh*> meshes;

	std::map<FbxNode*, CVS_Bone*> boneMap;
	std::map<FbxNode*, CVS_Mesh*> meshMap;

	auto pRootNode = _scene->GetRootNode();
	// skeleton pass
	_RecursionSwitcher(pRootNode, EFbxParseMode::Skeleton, boneMap, meshMap);
	// mesh pass
	meshes = _RecursionSwitcher(pRootNode, EFbxParseMode::Mesh, boneMap, meshMap);
	// Animation pass
	//GETTING ANIMAION DATA
	char pTimeString[256];

	auto timeMode = gSettings.GetTimeMode();
	printf("Time Mode : %s\n", FbxGetTimeModeName(timeMode));
	double fps = FrameRateToDouble(timeMode);
	FbxTimeSpan pTimeSpan;
	gSettings.GetTimelineDefaultTimeSpan(pTimeSpan);
	FbxTime     lStart, lEnd;
	lStart = pTimeSpan.GetStart();
	lEnd = pTimeSpan.GetStop();
	printf("Timeline default timespan: \n");
	printf("     Start: %s\n", lStart.GetTimeString(pTimeString, FbxUShort(256)));
	printf("     Stop : %s\n", lEnd.GetTimeString(pTimeString, FbxUShort(256)));

	// Get animation information
	// Now only supports one take
	for (int i = 0; i < _scene->GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
		auto lAnimStack = _scene->GetSrcObject<FbxAnimStack>(i);

		auto stackName = lAnimStack->GetName();
		printf("Animation Stack %d Name: %s\n", i, stackName);
		auto start = lAnimStack->GetLocalTimeSpan().GetStart();
		auto stop = lAnimStack->GetLocalTimeSpan().GetStop();
		auto animationLength = stop.GetFrameCount(timeMode) - start.GetFrameCount(timeMode);// TODO: do we really need +1?;
		printf("     Start: %d\n", start.GetFrameCount(timeMode));
		printf("     Stop : %d\n", stop.GetFrameCount(timeMode));
		// For Each layer
		int numLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < numLayers; ++j)
		{
			FbxAnimLayer* pAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

			auto layerName = pAnimLayer->GetName();
			printf("Animation Stack Name: %s\n", layerName);

			// Start with root node
			FbxNode* pNodeItr = _scene->GetRootNode();
			// Breadth first inline recursion
			std::queue<FbxNode*> nodes;
			while (pNodeItr != NULL)
			{
				FbxAnimCurve* lAnimCurve = pNodeItr->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				// For each node, extract all channels
				static const char channels[] = { FBXSDK_CURVENODE_TRANSLATION, FBXSDK_CURVENODE_ROTATION, FBXSDK_CURVENODE_SCALING };
				static const char components[] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };
				for (int i = 0; i < 3; i++)
				{
					// Extract Translation Curve
					auto lAnimCurve = pNodeItr->LclTranslation.GetCurve(pAnimLayer, components[i]);
					if (lAnimCurve)
					{
						printf("        %s%s\n", channels[0], components[i]);
						//ProcessCurve(lAnimCurve);
					}

					// Extract Rotation Curve
					lAnimCurve = pNodeItr->LclRotation.GetCurve(pAnimLayer, components[i]);
					if (lAnimCurve)
					{
						printf("        %s%s\n", channels[1], components[i]);
						//ProcessCurve(lAnimCurve);
					}

					// Extract Scaling Curve
					lAnimCurve = pNodeItr->LclScaling.GetCurve(pAnimLayer, components[i]);
					if (lAnimCurve)
					{
						printf("        %s%s\n", channels[2], components[i]);
						//ProcessCurve(lAnimCurve);
					}
				}

				for (int i = 0; i < pNodeItr->GetChildCount(false); ++i)
				{
					nodes.push(pNodeItr->GetChild(i));
				}

				if (nodes.size() > 0)
				{
					pNodeItr = nodes.front();
					nodes.pop();
				}
				else
				{
					pNodeItr = NULL;
				}
			}
		}
	}

	// Resolve bone links and pass skeleton to mesh
	for (auto i : meshes)
	{
		i->initialize();
	}

	return meshes;
}
*/
std::vector<CVS_Mesh*> CVS_RenderSystem::addMeshesFromaiScene(const aiScene* scene)
{
	std::vector<CVS_Mesh*> newmeshes;
	for (int i = 0, e = scene->mNumMeshes; i < e; ++i)
	{
		CVS_Mesh* newmesh = new CVS_Mesh();
		newmesh->initializeFromAiMesh(scene->mMeshes[i]);
		newmeshes.push_back(newmesh);
		this->meshes.push_back(newmesh);
	}
	return newmeshes;
}

CVS_RenderScene* CVS_RenderSystem::createNewScene()
{
	CVS_RenderScene* newScene = new CVS_RenderScene();

	scenes.push_back(newScene);
	return newScene;
}

CVS_FrameBuffer* CVS_RenderSystem::createNewFramebuffer()
{
	CVS_FrameBuffer* newBuffer = new CVS_FrameBuffer();
	this->framebuffers.push_back(newBuffer);
	return newBuffer;
}

CVS_Texture* CVS_RenderSystem::createNewTexture(UINT target)
{
	CVS_Texture* newTex = new CVS_Texture(target);
	textures.push_back(newTex);
	return newTex;
}

CVS_Texture* CVS_RenderSystem::createNewTexture(char* filepath)
{
	CVS_Texture* newTex = new CVS_Texture(GL_TEXTURE_2D);

	textures.push_back(newTex);
	newTex->loadFile(filepath);
	return newTex;
}

bool CVS_RenderSystem::End()
{
	return true;
}