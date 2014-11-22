#include "Canvas.h"

bool CVS_DeferredPipeline::GBuffer::SetUp()
{
	glGenFramebuffers(1, &m_FrameBuffer);
	glGenTextures(ALL_TEX, m_Buffers);
	glGenTextures(1, &m_DepthTexture);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBuffer);


	for (int i = 0, e = ALL_TEX; i < e; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_Buffers[i]);

		glTexImage2D(GL_TEXTURE_2D, 
			0, GL_RGB32F, 800, 450, 
			0, GL_RGB, GL_FLOAT, NULL);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 
			m_Buffers[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 800, 450, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
	
	GLenum dBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(ALL_TEX, dBuffers);

	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		MessageBox(NULL, "Error Framebuffer incomplete", "Error", MB_OK);
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	return true;
}

void CVS_DeferredPipeline::GBuffer::WBind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBuffer);
}

void CVS_DeferredPipeline::GBuffer::RBind()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBuffer);
}

void CVS_DeferredPipeline::GBuffer::RBindPosBuffer()
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + POS_TEX);
}

void CVS_DeferredPipeline::SetUp()
{
	printf("Setup\n");
	GLOBALSTATEMACHINE.m_RenderSub.createNewShader("Default", "./Shaders/Deferred.vert", "./Shaders/Deferred.frag");
	this->buffer.SetUp();
}

void CVS_DeferredPipeline::Render(CVS_RenderScene* Scene, CVS_View view)
{
	//Geometry pass
	buffer.WBind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Scene->Draw(&view);

	//Light pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
}

void CVS_DeferredPipeline::SetSize(int w, int h)
{

}