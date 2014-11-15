#include "Canvas.h"
#include "CVS_GUI.h"





CVS_Selection::CVS_Selection(CVS_SceneView* view) :parent(view)
{
	this->mRenderBuffer = GLOBALSTATEMACHINE.m_RenderSub.createNewFramebuffer();
	this->mSelectionTexture = GLOBALSTATEMACHINE.m_RenderSub.createNewTexture(GL_TEXTURE_2D);

	int width = view->getWidth(), height = view->getHeight();

	mRenderBuffer->Bind(GL_FRAMEBUFFER);

	mSelectionTexture->loadData(GL_RGB, GL_RGB32F, 0, width ? width : 100, height ? height : 100, 0, NULL);
	mRenderBuffer->BindColorAttachment(mSelectionTexture, 0);

	mDepthTexture = GLOBALSTATEMACHINE.m_RenderSub.createNewTexture(GL_TEXTURE_2D);
	mDepthTexture->loadData(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, 0, width ? width : 100, height ? height : 100, 0, NULL);

	mRenderBuffer->BindDepthAttachment(mDepthTexture);

	mRenderBuffer->setDrawBuffer(GL_COLOR_ATTACHMENT0);
	mRenderBuffer->setReadBuffer(GL_NONE);

	if (!mRenderBuffer->GetBufferStatus())
	{
		MessageBox(NULL, "Error Generating buffer Status", "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
	mRenderBuffer->unBind();

	mRenderProgram = GLOBALSTATEMACHINE.m_RenderSub.getRenderProgram("Selection");

	MVPLoc = mRenderProgram->getUniformHash("MVP");
	DrawIndexLoc = mRenderProgram->getUniformHash("DrawIndex");
	ObjectIndexLoc = mRenderProgram->getUniformHash("ObjectIndex");
}

void CVS_Selection::UpdateSize()
{
	int width = parent->getWidth(), height = parent->getHeight();

	mRenderBuffer->Bind(GL_FRAMEBUFFER);

	mSelectionTexture->loadData(GL_RGB, GL_RGB32F, 0, width ? width : 100, height ? height : 100, 0, NULL);
	mRenderBuffer->BindColorAttachment(mSelectionTexture, 0);

	mDepthTexture = GLOBALSTATEMACHINE.m_RenderSub.createNewTexture(GL_TEXTURE_2D);
	mDepthTexture->loadData(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, 0, width ? width : 100, height ? height : 100, 0, NULL);

	mRenderBuffer->BindDepthAttachment(mDepthTexture);

	mRenderBuffer->setDrawBuffer(GL_COLOR_ATTACHMENT0);
	mRenderBuffer->setReadBuffer(GL_NONE);

	if (!mRenderBuffer->GetBufferStatus())
	{
		MessageBox(NULL, "Error Generating buffer Status", "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
	mRenderBuffer->unBind();
}

void CVS_Selection::Render(CVS_RenderScene* scene, CVS_View* view)
{
	mRenderBuffer->Bind(GL_DRAW_FRAMEBUFFER);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mRenderProgram->setAsCurrentProgram();

	for (int i = 0, e = scene->nodes.size(); i < e; ++i)
	{
		glUniform1i(this->ObjectIndexLoc, (GLint)scene->nodes[i]->msgData);
		glUniform1i(this->DrawIndexLoc, i);
		cmat4 mat = view->Pers * view->View * scene->nodes[i]->modelMatrix;

		glUniformMatrix4fv(this->MVPLoc, 1, GL_FALSE, glm::value_ptr(mat));

		scene->nodes[i]->mesh->Draw();
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClearColor(0.7, 0.7, 0.7, 1.0);
}

SelectData CVS_Selection::getPrimitiveID(int x, int y)
{
	SelectData data;

	RECT rect;

	GetClientRect(this->parent->hWnd, &rect);

	y = rect.bottom - rect.top - y - 1;

	mRenderBuffer->Bind(GL_READ_FRAMEBUFFER);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &data);
	printf("Data %p\n", (void*)((GLint)data.ObjectID));
	return data;
}

