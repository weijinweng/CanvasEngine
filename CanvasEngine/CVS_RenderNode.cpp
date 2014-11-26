#include "Canvas.h"

CVS_TextureReference::CVS_TextureReference(CVS_Texture* texture, int loc, std::string uniformname) :texture(texture), uniformLoc(loc), name(uniformname)
{

}

CVS_RenderNode::CVS_RenderNode(CVS_RenderProgramInstance* parent) : mesh(NULL)
{
	parent->children.push_back(this);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05));
	for (int i = 0, e = parent->program->uniforms.size(); i < e; ++i)
	{
		if (parent->program->uniforms[i].type == GL_SAMPLER_2D)
		{
			this->textures.push_back(CVS_TextureReference(GLOBALSTATEMACHINE.m_RenderSub.m_DefaultTexture, parent->program->uniforms[i].location, parent->program->uniforms[i].name));
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

void CVS_RenderNode::setMesh(CVS_Mesh* mesh)
{
	this->mesh = mesh;

}
