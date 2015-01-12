#include "RRenderer.h"

using namespace Canvas;

RMaterialNode::RMaterialNode() :RendererNode("Template", "MaterialNode")
{

}

RMaterialNode::RMaterialNode(std::string _name, RMaterialShaderNode* _prog) : RendererNode(_name, "MaterialNode"), m_shdr(_prog)
{
	SetRenderer(_prog->m_renderer);
	m_renderer->m_scene->AddRendererNode(this, CVS_RSCENE_MATERIALNODES);

}

bool RMaterialNode::DataLink()
{
	return true;
}

bool RMaterialNode::LoadLink()
{
	return true;
}

bool RMaterialNode::Link()
{
	return true;
}

bool RMaterialNode::CleanupLink()
{
	SyncMaterialProperties();
	return true;
}

void RMaterialNode::SyncMaterialProperties()
{
	for (int i = 0, e = m_shdr->m_mat_attrs.size(); i < e; ++i)
	{
		switch (m_shdr->m_mat_attrs[i].type)
		{
		case CVS_VEC4:
		{
						m_properties.AddValue(m_shdr->m_mat_attrs[i].loc, m_shdr->m_temp.FindVec4(m_shdr->m_mat_attrs[i].name), m_shdr->m_mat_attrs[i].name);
		}
			break;
		case CVS_VEC3:
		{
						m_properties.AddValue(m_shdr->m_mat_attrs[i].loc, m_shdr->m_temp.FindVec3(m_shdr->m_mat_attrs[i].name), m_shdr->m_mat_attrs[i].name);
		}
			break;
		case CVS_MAT4:
		{
						m_properties.AddValue(m_shdr->m_mat_attrs[i].loc, m_shdr->m_temp.FindMat4(m_shdr->m_mat_attrs[i].name), m_shdr->m_mat_attrs[i].name);
		}
			break;
		case CVS_FLOAT:
		{
						m_properties.AddValue(m_shdr->m_mat_attrs[i].loc, m_shdr->m_temp.FindFloat(m_shdr->m_mat_attrs[i].name), m_shdr->m_mat_attrs[i].name);
		}
			break;
		case CVS_INT:
		{
						m_properties.AddValue(m_shdr->m_mat_attrs[i].loc, m_shdr->m_temp.FindInt(m_shdr->m_mat_attrs[i].name), m_shdr->m_mat_attrs[i].name);
		}
			break;
		case CVS_TEXTURE_2D:
		{
						m_properties.AddValue(m_shdr->m_mat_attrs[i].loc, m_shdr->m_temp.FindVec4(m_shdr->m_mat_attrs[i].name), m_shdr->m_mat_attrs[i].name);
		}
			break;
		}
	}

	m_properties.SetNodeAttributes(this);
}

void RMaterialNode::Draw()
{
	m_properties.Bind();
	for (int i = 0, e = m_draw_nodes.size(); i < e; ++i)
	{
		glm::mat4 MVP = m_renderer->VP * m_draw_nodes[i].m_model;
		glUniformMatrix4fv(m_shdr->MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glDrawElements(GL_TRIANGLES, m_draw_nodes[i].m_count, GL_UNSIGNED_INT, NULL);
	}
}
