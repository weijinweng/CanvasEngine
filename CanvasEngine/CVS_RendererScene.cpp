#include "Canvas.h"


CVS_RenderProgramInstance::CVS_RenderProgramInstance(CVS_RenderProgram* program)
{
	this->program = program;
	this->viewLoc = glGetUniformLocation(program->programID, "V");
	if (viewLoc < 0)
	{
		printf("error uni\n");
	}
	mvpLoc = glGetUniformLocation(program->programID, "MVP");
	if (mvpLoc < 0)
	{
		printf("Error uni\n");
	}
	this->modelLoc = glGetUniformLocation(program->programID, "M");
}

void CVS_RenderProgramInstance::Render(CVS_View* view)
{

	program->setAsCurrentProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view->View));

	for (int i = 0, e = this->children.size(); i < e; ++i)
	{
		cmat4 Model = this->children[i]->modelMatrix;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));

		cmat4 MVP = view->Pers * view->View * Model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		for (int j = 0, e = children[j]->textures.size(); j < e; ++j)
		{
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(children[i]->textures[j].texture->target, children[i]->textures[j].texture->textureID);
			glUniform1i(children[i]->textures[j].uniformLoc, j);
		}

		if (children[i]->mesh != NULL)
			children[i]->mesh->Draw();
	}
}

void CVS_RenderProgramInstance::BoundingSphereCull(CVS_View* view)
{
	numActive = 0;
	for (int i = 0, e = children.size(); i < e; ++i)
	{
		if (children[i]->CheckActiveBSphere(view))
		{
			this->activeChildren[numActive] = children[i];
			numActive++;
		}
	}
}

void CVS_RenderProgramInstance::CulledRender(CVS_View* view)
{
	program->setAsCurrentProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view->View));

	for (int i = 0, e = this->children.size(); i < e; ++i)
	{
		if (this->children[i]->CheckActiveBSphere(view))
		{
			cmat4 Model = this->children[i]->modelMatrix;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));

			cmat4 MVP = view->Pers * view->View * Model;
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

			for (int j = 0, e = children[j]->textures.size(); j < e; ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j);
				glBindTexture(children[i]->textures[j].texture->target, children[i]->textures[j].texture->textureID);
				glUniform1i(children[i]->textures[j].uniformLoc, j);
			}

			if (children[i]->mesh != NULL)
				children[i]->mesh->Draw();
		}
	}
}

void CVS_RenderProgramInstance::addChild(CVS_RenderNode* node)
{
	this->children.push_back(node);
	this->activeChildren.resize(this->children.size());
}


CVS_RenderScene::CVS_RenderScene()
{
	this->programs.push_back(new CVS_RenderProgramInstance(GLOBALSTATEMACHINE.m_RenderSub.createNewShader("Default", "./Shaders/3D.vert", "./Shaders/3D.frag")));
}

CVS_RenderNode* CVS_RenderScene::createNewNode()
{
	CVS_RenderNode* newNode = new CVS_RenderNode(programs[0]);
	nodes.push_back(newNode);
	return newNode;
}

void CVS_RenderScene::Draw(CVS_View* view)
{
	for (int i = 0, e = programs.size(); i < e; ++i)
	{
		programs[i]->Render(view);
	}
}

void CVS_RenderScene::FrustumCull(CVS_View* view)
{
	for (int i = 0, e = programs.size(); i < e; ++i)
	{
		programs[i]->BoundingSphereCull(view);
	}
}

void CVS_RenderScene::OptimizedDraw(CVS_View* view)
{
	for (int i = 0, e = programs.size(); i < e; ++i)
	{
		programs[i]->CulledRender(view);
	}
}