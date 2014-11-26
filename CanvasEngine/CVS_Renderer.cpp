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

void CVS_RenderPipeline::SetSize(int w, int h)
{

}

CVS_Renderer::CVS_Renderer(HDC glHdc)
{
	wglMakeCurrent(glHdc, GLOBALSTATEMACHINE.m_RenderSub.m_glContext);
	m_ParentHDC = glHdc;
	renderSystem = &GLOBALSTATEMACHINE.m_RenderSub;
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
		renderSystem->pipeline->Render(scene, View);
	}

	SwapBuffers(m_ParentHDC);
}
