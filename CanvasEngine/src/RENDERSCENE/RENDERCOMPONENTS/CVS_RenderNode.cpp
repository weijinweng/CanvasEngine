#include "Canvas.h"
#include "CVS_RenderComponents.h"


CVS_TextureReference::CVS_TextureReference(CVS_Texture* texture, int loc) :texture(texture), uniformLoc(loc)
{

}

CVS_RenderNode::CVS_RenderNode(CVS_RenderProgramInstance* parent) : mesh(NULL)
{
	parent->children.push_back(this);
	for (int i = 0, e = parent->program->uniforms.size(); i < e; ++i)
	{
		if (parent->program->uniforms[i].type == GL_SAMPLER_2D)
		{
			this->textures.push_back(CVS_TextureReference(GLOBALSTATEMACHINE.m_RenderSub.m_DefaultTexture, parent->program->uniforms[i].location));
		}
	}
	printf("Created rendernode\n");
}

void CVS_RenderNode::setMesh(CVS_Mesh* mesh)
{
	this->mesh = mesh;
	printf("Set mesh %d\n", mesh->indices.size());
}