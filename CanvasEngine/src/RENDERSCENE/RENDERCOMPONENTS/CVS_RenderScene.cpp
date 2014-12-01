#include "CVS_RenderComponents.h"
#include "Canvas.h"

#include "CVS_Mesh.h"
#include "CVS_Skeleton.h"

CVS_RenderProgramInstance::CVS_RenderProgramInstance(CVS_RenderProgram* program)
{
	this->program = program;
	this->viewLoc = glGetUniformLocation(program->programID, "V");
	if (viewLoc < 0)
	{
		printf("error uni\n");
	}

	this->mvpLoc = glGetUniformLocation(program->programID, "MVP");
	if (mvpLoc < 0)
	{
		printf("Error uni\n");
	}

	this->modelLoc = glGetUniformLocation(program->programID, "M");
	if (modelLoc < 0)
	{
		printf("Error uni\n");
	}

	char uniformName[12];
	for (int i = 0; i < MAX_BONES; i++)
	{
		sprintf_s(uniformName, sizeof(uniformName), "gBones[%d]", i);
		m_boneLoc[i] = glGetUniformLocation(program->programID, uniformName);
		if (m_boneLoc[i] < 0)
		{
			printf("Error uni\n");
			assert(0);
		}
	}
}

void CVS_RenderProgramInstance::Render(CVS_View* view)
{

	program->setAsCurrentProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view->View));

	int lightPos = glGetUniformLocation(program->programID, "LightPosition_worldspace");
	glUniform3f(lightPos, 3, 3, 3);


	for(int i = 0, e = this->children.size(); i < e; ++i)
	{
		cmat4 Model = this->children[i]->modelMatrix * this->children[i]->mesh->m_offset;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));
		
		cmat4 MVP = view->Pers * view->View * Model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		
		auto& globalPose = this->children[i]->m_pSkeletonPose->m_aGlobalPose;
		for (int j = 0, e = globalPose.size(); j < e; ++j)
		{
			glUniformMatrix4fv(m_boneLoc[j], 1, GL_FALSE, glm::value_ptr(globalPose[j]));
		}

		for (int i = 0, e = children[i]->textures.size(); i < e; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(children[i]->textures[i].texture->target, children[i]->textures[i].texture->textureID);
			glUniform1i(children[i]->textures[i].uniformLoc, i);
		}

		if (children[i]->mesh != NULL)
		{
			children[i]->mesh->Draw(CVS_Renderable::EDrawMode::Default);
			children[i]->mesh->Draw(CVS_Renderable::EDrawMode::Skeleton);
		}
	}
}

CVS_RenderScene::CVS_RenderScene()
{
	this->programs.push_back(new CVS_RenderProgramInstance(GLOBALSTATEMACHINE.m_RenderSub.createNewShader("Default3D", "./Shaders/3D.vert", "./Shaders/3D.frag")));
}

CVS_RenderNode* CVS_RenderScene::createNewNode()
{
	CVS_RenderNode* newNode = new CVS_RenderNode(programs[0]);
	nodes.push_back(newNode);
	return newNode;
}

void CVS_RenderScene::Draw(CVS_View* view)
{
	for(int i = 0, e = programs.size(); i < e; ++i)
	{
		programs[i]->Render(view);
	}
}

