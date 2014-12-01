#include "Canvas.h"

CVS_TextureReference::CVS_TextureReference(CVS_Texture* texture, int loc, std::string uniformname) :texture(texture), uniformLoc(loc), name(uniformname)
{

}

CVS_RenderNode::CVS_RenderNode(CVS_RenderProgramInstance* parent) : mesh(NULL)
{
	parent->addChild(this);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05));
	for (int i = 0, e = parent->program->uniforms.size(); i < e; ++i)
	{
		if (parent->program->uniforms[i].type == GL_SAMPLER_2D)
		{
			if (parent->program->uniforms[i].name == "specMap")
			{
				this->textures.push_back(CVS_TextureReference(GLOBALSTATEMACHINE.m_RenderSub.m_DefaultSpecTexture, parent->program->uniforms[i].location, parent->program->uniforms[i].name));
			}
			else if (parent->program->uniforms[i].name == "alphaMask")
			{
				this->textures.push_back(CVS_TextureReference(GLOBALSTATEMACHINE.m_RenderSub.m_DefaultMaskTexture, parent->program->uniforms[i].location, parent->program->uniforms[i].name));
			}
			else {
				this->textures.push_back(CVS_TextureReference(GLOBALSTATEMACHINE.m_RenderSub.m_DefaultTexture, parent->program->uniforms[i].location, parent->program->uniforms[i].name));

			}
		}
	}
}

bool CVS_RenderNode::SetTexture(std::string texturename, CVS_Texture* texture)
{
	for (int i = 0, e = textures.size(); i < e; ++i)
	{
		if (texturename == textures[i].name)
		{
			textures[i].texture = texture;
			return true;
		}
	}
	printf("RENDERNODEERROR can not find texture reference\n");
	return false;
}

bool CVS_RenderNode::CheckActiveBSphere(CVS_View* view)
{
	float radius = this->mesh->bSphere.radius * 0.05f;
	cvec3 center = this->mesh->bSphere.center * 0.05f;
	bool result = true;
	for (int i = 0; i < 6; ++i)
	{
		float dist = view->frustum->pl[i].dist(center);

		if (dist < (-1.0f * radius))
		{
			return false;
		}
	}
	return true;
}

void CVS_RenderNode::setMesh(CVS_Mesh* mesh)
{
	this->mesh = mesh;
}
