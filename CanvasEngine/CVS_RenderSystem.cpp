#include "Canvas.h"

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
	glFramebufferTexture2D(flag, GL_COLOR_ATTACHMENT0 + location, texture->target, texture->texture, 0);
}

void CVS_FrameBuffer::BindDepthAttachment(CVS_Texture* texture)
{
	texture->Bind();
	glFramebufferTexture2D(flag, GL_DEPTH_ATTACHMENT, texture->target, texture->texture, 0);
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
	glUniformMatrix4fv(GLOBALSTATEMACHINE.m_RenderSub.m_GridMVP,1, GL_FALSE, glm::value_ptr(Mat));
	glBindVertexArray(GLOBALSTATEMACHINE.m_RenderSub.gridVAO);
	glDrawArrays(GL_LINES, 0, GLOBALSTATEMACHINE.m_RenderSub.m_GridNum);
	glBindVertexArray(0);
	
	if(scene != NULL)
	{
		scene->Draw(&View);
	}
	if(SwapBuffers(m_ParentHDC) != TRUE)
	{
		printf("Error swap buffers\n");
	}
}

CVS_RenderSystem::CVS_RenderSystem():m_glContext(NULL)
{
	this->pipeline = new CVS_RenderPipeline();
}

bool CVS_RenderSystem::Initialize()
{
	return true;
}

CVS_RenderProgram* CVS_RenderSystem::getRenderProgram(std::string name)
{
	if(programs.find(name) == programs.end())
	{
		return NULL;
	}
	return programs[name];
}

CVS_RenderProgram* CVS_RenderSystem::createNewShader(std::string name, char* file_vert_path, char* file_frag_path)
{
	if(programs.find(name) == programs.end())
	{
		CVS_RenderProgram* newProgram  = new CVS_RenderProgram();

		newProgram->loadFile(file_vert_path, file_frag_path);
		programs[name] = newProgram;
		return newProgram;
	}
	printf("Program already exists\n");
	return programs[name];
}

CVS_Renderer* CVS_RenderSystem::createNewRenderer(HDC glHdc)
{
	if(m_glContext == NULL)
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

	   if(!(pixelformat = ChoosePixelFormat(glHdc, &pfd)))
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

	   if(!(temp = wglCreateContext(glHdc)))
	   {
		   printf("Error getting glcontext\n");
		   return NULL;
	   }

	   if(!(wglMakeCurrent(glHdc, temp)))
	   {
		   printf("Error making gl current\n");
			return NULL;
	   }
	   printf("Yay it worked\n");

	   glewExperimental = GL_TRUE;

	   GLenum glewError = glewInit();
	   if(glewError != GLEW_OK)
	   {
		   printf("Error initializing GLEW %s\n", glewGetErrorString(glewError));
	   }

		int attribs[] =
		{
			 WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			 WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			 WGL_CONTEXT_FLAGS_ARB,
			 WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			 0
		};
		
	   m_glContext = wglCreateContextAttribsARB(glHdc, NULL, attribs);

	   wglMakeCurrent(NULL,NULL);
	   wglDeleteContext(temp);
	   wglMakeCurrent(glHdc, m_glContext);

		std::vector<cvec3> vertices;

		glGenVertexArrays(1, &gridVAO);
		glBindVertexArray(gridVAO);

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		float X = -1.0f, Y = 0.0f, Z= 1.0f;

		for(int i = 0; i < 11; ++i)
		{
			vertices.push_back(cvec3(X, Y, Z));
			vertices.push_back(cvec3(X, Y,-1.0f*Z));
			X += 0.2f;
		}

		X = 1.0f;
		Z = -1.0f;
	
		for(int i = 0; i < 11; ++i)
		{
			vertices.push_back(cvec3(X, Y, Z));
			vertices.push_back(cvec3(-1.0f * X, Y, Z));
			Z += 0.2f;
		}



		glBufferData(GL_ARRAY_BUFFER, sizeof(cvec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	

		glBindVertexArray(0);

		glDeleteBuffers(1, &vertexBuffer);

		m_GridDraw = createNewShader("DefaultGrid", "./Shaders/grid.vert", "./Shaders/grid.frag");

		m_GridMVP = glGetUniformLocation(m_GridDraw->programID, "MVP");

		if(m_GridMVP == -1)
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
	}



	CVS_Renderer* newRenderer = new CVS_Renderer(glHdc);
	this->renderers.push_back(newRenderer);
	return newRenderer;
}

std::vector<CVS_Mesh*> CVS_RenderSystem::addMeshesFromaiScene(const aiScene* scene)
{
	std::vector<CVS_Mesh*> newmeshes;
	for(int i = 0, e = scene->mNumMeshes; i < e; ++i)
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

bool CVS_RenderSystem::End()
{
	return true;
}