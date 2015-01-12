#include "RRenderer.h"

using namespace Canvas;

RMaterialShaderNode::RMaterialShaderNode(std::string _name) :RProgramNode(_name, "MaterialShaderNode")
{

}

RMaterialShaderNode::RMaterialShaderNode(std::string _name, RRenderer* _renderer) : RProgramNode(_name, "MaterialShaderNode"), m_prog(NULL)
{
	SetRenderer(_renderer);
	_renderer->m_scene->AddRendererNode(this, CVS_RSCENE_MATERIALSHADERNODES);

	v_shdr = m_attributes.AddAttribute("VertexShader", CVS_SHADER, NULL, CVS_RNODES_INPUT);
	f_shdr = m_attributes.AddAttribute("FragmentShader", CVS_SHADER, NULL, CVS_RNODES_INPUT);

	DataLink();
}

bool RMaterialShaderNode::DataLink()
{
	if (!m_prog)
	{
		if (m_renderer->m_ctx)
		{
			m_prog = m_renderer->m_ctx->AllocShaderProgram();
			if (m_prog)
				return true;
		}
		return false;
	}
	else {
		return true;
	}
}

bool RMaterialShaderNode::LoadLink()
{
	return true;
}

bool RMaterialShaderNode::Link()
{
	bool update = false;

	if (v_shdr->GetDataPtr())
	{
		if (((RShaderNode*)v_shdr->GetDataPtr())->Updated())
		{
			m_prog->LinkShader(((RShaderNode*)v_shdr)->m_shdr);
			update = true;
		}
	}

	if (f_shdr->GetDataPtr())
	{
		if (((RShaderNode*)f_shdr->GetDataPtr())->Updated())
		{
			m_prog->LinkShader(((RShaderNode*)f_shdr)->m_shdr);
			update = true;
		}
	}

	if (m_prog->GetProgramStatus() != CVS_OK)
	{
		m_prog->PrintProgramErrorMessage();
		return false;
	}
	
	if (update)
	{
		LoadMaterialAttributes();

		for (int i = 0, e = m_nodes.size(); i < e; ++i)
		{
			m_nodes[i]->SyncMaterialProperties();
		}
	}

	return true;
}

bool RMaterialShaderNode::CleanupLink()
{
	return true;
}

RMaterialNode* RMaterialShaderNode::CreateMaterialNode(std::string _name)
{
	RMaterialNode* ret = new RMaterialNode(_name, this);
	if (m_mat_attrs.size())
	{
		ret->SyncMaterialProperties();
	}
	return ret;
}

void RMaterialShaderNode::LoadMaterialAttributes()
{
	m_mat_attrs.clear();
	m_prog->Bind();

	GLuint program_id = m_prog->m_prog_id;

	int count;

	glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &count);
	
	char name[200];

	for (int i = 0; i < count; ++i)
	{
		int arraySize = 0;
		GLenum type = GL_FALSE;
		GLsizei actualLength = 0;

		glGetActiveUniform(program_id, i, 200, &actualLength, &arraySize, &type, name);

		MaterialAttributes attr = {i, GLToCVS(type), std::string(name) };

		this->m_mat_attrs.push_back(attr);
	}
}

void RMaterialShaderNode::Run()
{
	for (int i = 0, e = m_nodes.size(); i < e; ++i)
	{
		m_nodes[i]->Draw();
	}

}












