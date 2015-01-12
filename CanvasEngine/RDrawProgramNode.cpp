#include "RRenderer.h"

using namespace Canvas;

RDrawProgramNode::RDrawProgramNode() : RProgramNode("Template", "DrawProgramNode")
{

}

RDrawProgramNode::RDrawProgramNode(std::string _name, RRenderer* _renderer) : RProgramNode(_name, "DrawProgramNode")
{
	SetRenderer(_renderer);
	_renderer->AddNode(CVS_RRENDERER_PROGRAMS, this);

	m_attributes.AddAttribute("VertexShader", CVS_SHADER, &v_shdr, CVS_RNODES_INPUT);
	m_attributes.AddAttribute("GeometryShader", CVS_SHADER, &g_shdr, CVS_RNODES_INPUT);
	m_attributes.AddAttribute("TessellationShader", CVS_SHADER, &t_shdr, CVS_RNODES_INPUT);
	m_attributes.AddAttribute("ComputeShader", CVS_SHADER, &c_shdr, CVS_RNODES_INPUT);
	m_attributes.AddAttribute("FragmentShader", CVS_SHADER, &f_shdr, CVS_RNODES_INPUT);

	DataLink();
}

bool RDrawProgramNode::DataLink()
{
	if (!m_prog)
	{
		if (m_renderer->m_ctx)
		{
			m_prog = m_renderer->m_ctx->AllocShaderProgram();
			
			if (!m_prog)
			{
				return false;
			}
		}
	}
	return true;
}

bool RDrawProgramNode::LoadLink()
{
	return m_prog;
}

bool RDrawProgramNode::Link()
{
	if (v_shdr)
	{
		m_prog->LinkShader(v_shdr);
	}
	if (f_shdr)
	{
		m_prog->LinkShader(f_shdr);
	}

	if (m_prog->GetProgramStatus() != CVS_OK)
	{
		m_prog->PrintProgramErrorMessage();
		return false;
	}

	return LoadProgramVariables();
}

bool RDrawProgramNode::LoadProgramVariables()
{
	GLuint program_id = m_prog->m_prog_id;

	glUseProgram(program_id);

	int count;

	glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &count);
	
	printf("Loaded number of uniforms %d\n", count);

	this->m_executables.resize(20);

	this->m_executables[0] = &RDrawProgramNode::Test;

	(this->*m_executables[0])();

	char name[200];
	for (int i = 0; i < count; ++i)
	{
		int arraySize = 0;
		GLenum type = GL_FALSE;
		GLsizei actualLength = 0;
		glGetActiveUniform(program_id, i, 200, &actualLength, &arraySize, &type, name);

		switch (type)
		{
			case GL_SAMPLER_2D:
			{
								  ShaderVariableContainerT2D var = { i, NULL, std::string(name) };
								  m_tex2d_variables.push_back(var);
			}
				break;
			case GL_INT:
			{
						   ShaderVariableContainerI var = { i, 0, std::string(name) };
						   m_int_variables.push_back(var);
			}
				break;
			case GL_FLOAT_VEC4:
			{
								  ShaderVariableContainerF4 var = { i, glm::vec4(0.0f), std::string(name) };
								  m_vec4_variables.push_back(var);
			}
				break;
			case GL_FLOAT_VEC3:
			{
								  ShaderVariableContainerF3 var = { i, glm::vec3(0.0f), std::string(name) };
								  m_vec3_variables.push_back(var);
			}
				break;
			case GL_FLOAT_MAT4:
			{
								  ShaderVariableContainerM4 var = { i, glm::mat4(1.0f), std::string(name) };
								  m_mat4_variables.push_back(var);
			}
				break;

		}
	}

	m_executables.clear();

	printf("Drawprogram executables size %d\n", m_executables.size());

	if (m_tex2d_variables.size())
	{
		this->m_executables.push_back(&RDrawProgramNode::Texture2DBind);
	}

	if (m_vec4_variables.size())
	{
		this->m_executables.push_back(&RDrawProgramNode::Vec4Bind);
	}

	if (m_vec3_variables.size())
	{
		this->m_executables.push_back(&RDrawProgramNode::Vec3Bind);
	}

	if (m_float_variables.size())
	{
		this->m_executables.push_back(&RDrawProgramNode::FloatBind);
	}

	if (m_int_variables.size())
	{
		this->m_executables.push_back(&RDrawProgramNode::IntBind);
	}

	if (m_mat4_variables.size())
	{
		this->m_executables.push_back(&RDrawProgramNode::Mat4Bind);
	}


	for (int i = 0; i < m_tex2d_variables.size(); ++i)
	{
		this->m_attributes.AddAttribute(m_tex2d_variables[i].name, CVS_TEXTURE_2D, &m_tex2d_variables[i].tex, CVS_RNODES_INPUT);
	}

	for (int i = 0; i < m_vec4_variables.size(); ++i)
	{
		this->m_attributes.AddAttribute(m_vec4_variables[i].name, CVS_VEC4, &m_vec4_variables[i].value, CVS_RNODES_INPUT);
	}

	for (int i = 0; i < m_vec3_variables.size(); ++i)
	{
		this->m_attributes.AddAttribute(m_vec3_variables[i].name, CVS_VEC3, &m_vec3_variables[i].value, CVS_RNODES_INPUT);
	}

	for (int i = 0; i < m_int_variables.size(); ++i)
	{
		this->m_attributes.AddAttribute(m_int_variables[i].name, CVS_INT, &m_int_variables[i].value, CVS_RNODES_INPUT);
	}

	for (int i = 0; i < m_float_variables.size(); ++i)
	{
		this->m_attributes.AddAttribute(m_float_variables[i].name, CVS_FLOAT, &m_float_variables[i].value, CVS_RNODES_INPUT);
	}

	for (int i = 0; i < m_mat4_variables.size(); ++i)
	{
		this->m_attributes.AddAttribute(m_mat4_variables[i].name, CVS_MAT4, &m_mat4_variables[i].value, CVS_RNODES_INPUT);
	}
	
	return true;
}

void RDrawProgramNode::Test()
{
	printf("Testing this %s\n", this->GetName().c_str());
}

bool RDrawProgramNode::CleanupLink()
{
	return LoadProgramVariables();
}

void RDrawProgramNode::Texture2DBind()
{
	for (int i = 0, e = m_tex2d_variables.size(); i < e; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_tex2d_variables[i].tex->m_tex_id);
		glUniform1i(m_tex2d_variables[i].loc, i);
	}
}

void RDrawProgramNode::IntBind()
{
	for (int i = 0, e = m_int_variables.size(); i < e; ++i)
	{
		glUniform1i(m_int_variables[i].loc, m_int_variables[i].value);
	}
}

void RDrawProgramNode::FloatBind()
{
	for (int i = 0, e = m_float_variables.size(); i < e; ++i)
	{
		glUniform1f(m_float_variables[i].loc, m_float_variables[i].value);
	}
}

void RDrawProgramNode::Mat4Bind()
{
	for (int i = 0, e = m_mat4_variables.size(); i < e; ++i)
	{
		glUniformMatrix4fv(m_mat4_variables[i].loc, 1, GL_FALSE, glm::value_ptr(m_mat4_variables[i].value));
	}
}

void RDrawProgramNode::Vec3Bind()
{
	for (int i = 0, e = m_vec3_variables.size(); i < e; ++i)
	{
		glUniform3fv(m_vec3_variables[i].loc, 1, glm::value_ptr(m_vec3_variables[i].value));
	}
}

void RDrawProgramNode::Vec4Bind()
{
	for (int i = 0, e = m_vec4_variables.size(); i < e; ++i)
	{
		glUniform1fv(m_vec4_variables[i].loc, 1, glm::value_ptr(m_vec4_variables[i].value));
	}
}

void RDrawProgramNode::Run()
{
	m_prog->Bind();
	for (int i = 0, e = m_executables.size(); i < e; ++i)
	{
		(this->*m_executables[i])();
	}
	glBindVertexArray(m_VAO);
	glDrawElements(m_draw_cmd, m_draw_count, GL_UNSIGNED_INT, NULL);
}