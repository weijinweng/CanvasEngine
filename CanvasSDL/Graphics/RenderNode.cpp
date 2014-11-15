#include "Canvas.h"
#include "RenderNode.h"
#include "RenderProgram.h"

void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to)
{
	to[0][0] = (GLfloat)from->a1; to[1][0] = (GLfloat)from->a2;
	to[2][0] = (GLfloat)from->a3; to[3][0] = (GLfloat)from->a4;
	to[0][1] = (GLfloat)from->b1; to[1][1] = (GLfloat)from->b2;
	to[2][1] = (GLfloat)from->b3; to[3][1] = (GLfloat)from->b4;
	to[0][2] = (GLfloat)from->c1; to[1][2] = (GLfloat)from->c2;
	to[2][2] = (GLfloat)from->c3; to[3][2] = (GLfloat)from->c4;
	to[0][3] = (GLfloat)from->d1; to[1][3] = (GLfloat)from->d2;
	to[2][3] = (GLfloat)from->d3; to[3][3] = (GLfloat)from->d4;
}

namespace Canvas
{
	renderNode::renderNode()
	{}

	void renderNode::initFromRenderNode(aiNode* child, std::vector<CVS_Mesh*> meshes)
	{
		name = std::string(child->mName.data);
		for (int i = 0, e = child->mNumMeshes; i < e; i++)
		{
			printf("Node %s added mesh %s\n", name.c_str(), meshes[child->mMeshes[i]]->name.c_str());
			this->meshes.push_back(meshes[child->mMeshes[i]]);
		}

		printf("added new render node:%s\n", name.c_str());
		glm::mat4 assMatrix;
		copyAiMatrixToGLM(&child->mTransformation, assMatrix);
		glm::vec4 transform(0, 0, 0, 1);
		transform = assMatrix*transform;
		//get translate
		this->transform.translation.x = transform.x;
		this->transform.translation.y = transform.y;
		this->transform.translation.z = transform.z;
		printf("translate x%f y%f z%f\n", transform.x, transform.y, transform.z);
		//get scale
		transform = glm::vec4(1, 0, 0, 1);

		float scale = 1;
		transform = assMatrix * transform;
		scale = glm::length(glm::vec3(transform.x, transform.y, transform.z) - this->transform.translation) / scale;
		this->transform.scale *= scale;
		printf("scale is %f\n", scale);

		glm::mat4 rotationMatrix = glm::translate(assMatrix, this->transform.translation * (-1.0f));

		rotationMatrix = glm::scale(rotationMatrix, glm::vec3(1 / scale));

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
		this->transform.orientation = rotation;





	}

	void renderNode::initFromRenderNode(FbxNode* child, std::vector<CVS_Mesh*> meshes)
	{
		name = std::string(child->mName.data);
		for (int i = 0, e = child->mNumMeshes; i < e; i++)
		{
			printf("Node %s added mesh %s\n", name.c_str(), meshes[child->mMeshes[i]]->name.c_str());
			this->meshes.push_back(meshes[child->mMeshes[i]]);
		}

		printf("added new render node:%s\n", name.c_str());
		glm::mat4 assMatrix;
		copyAiMatrixToGLM(&child->mTransformation, assMatrix);
		glm::vec4 transform(0, 0, 0, 1);
		transform = assMatrix*transform;
		//get translate
		this->transform.translation.x = transform.x;
		this->transform.translation.y = transform.y;
		this->transform.translation.z = transform.z;
		printf("translate x%f y%f z%f\n", transform.x, transform.y, transform.z);
		//get scale
		transform = glm::vec4(1, 0, 0, 1);

		float scale = 1;
		transform = assMatrix * transform;
		scale = glm::length(glm::vec3(transform.x, transform.y, transform.z) - this->transform.translation) / scale;
		this->transform.scale *= scale;
		printf("scale is %f\n", scale);

		glm::mat4 rotationMatrix = glm::translate(assMatrix, this->transform.translation * (-1.0f));

		rotationMatrix = glm::scale(rotationMatrix, glm::vec3(1 / scale));

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
		this->transform.orientation = rotation;





	}

	bool renderNode::setProgram(renderProgram* program)
	{
		textureID.clear();
		for (int i = 0; i < program->textureTemplate.size(); ++i)
		{
			textureID.push_back(program->textureTemplate[i]);
		}
		return true;
	}

	void renderNode::setTexture(std::string name, std::string texturename)
	{
		for (int i = 0, e = textureID.size(); i < e; ++i)
		{
			if (textureID[i].name == name)
			{
				textureID[i].setTexture(texturename);
				return;
			}
		}

		printf("Cannot find correct texture %s\n", texturename.c_str());
	}

	void renderNode::Render()
	{}
}