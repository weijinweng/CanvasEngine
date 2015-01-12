#include "RRenderer.h"

using namespace Canvas;

void MaterialProperties::SetNodeAttributes(RMaterialNode* _node)
{
	if (m_vec3_variables.size())
	{
		m_executables.push_back(&MaterialProperties::Vec3Bind);
	}
	for (int i = 0, e = m_vec3_variables.size(); i < e; ++i)
	{
		_node->m_attributes.AddAttribute(m_vec3_variables[i].name, CVS_VEC3, &m_vec3_variables[i].value, CVS_RNODES_INPUT);
	}

	if (m_vec4_variables.size())
	{
		m_executables.push_back(&MaterialProperties::Vec4Bind);
	}
	for (int i = 0, e = m_vec4_variables.size(); i < e; ++i)
	{
		_node->m_attributes.AddAttribute(m_vec4_variables[i].name, CVS_VEC4, &m_vec4_variables[i].value, CVS_RNODES_INPUT);
	}

	if (m_mat4_variables.size())
	{
		m_executables.push_back(&MaterialProperties::Mat4Bind);
	}
	for (int i = 0, e = m_mat4_variables.size(); i < e; ++i)
	{
		_node->m_attributes.AddAttribute(m_mat4_variables[i].name, CVS_MAT4, &m_mat4_variables[i].value, CVS_RNODES_INPUT);
	}

	if (m_float_variables.size())
	{
		m_executables.push_back(&MaterialProperties::FloatBind);
	}
	for (int i = 0, e = m_float_variables.size(); i < e; ++i)
	{
		_node->m_attributes.AddAttribute(m_float_variables[i].name, CVS_FLOAT, &m_float_variables[i].value, CVS_RNODES_INPUT);
	}

	if (m_int_variables.size())
	{
		m_executables.push_back(&MaterialProperties::IntBind);
	}
	for (int i = 0, e = m_int_variables.size(); i < e; ++i)
	{
		_node->m_attributes.AddAttribute(m_int_variables[i].name, CVS_INT, &m_int_variables[i].value, CVS_RNODES_INPUT);
	}

	if (m_tex2d_variables.size())
	{
		m_executables.push_back(&MaterialProperties::Texture2DBind);
	}
	for (int i = 0, e = m_tex2d_variables.size(); i < e; ++i)
	{
		_node->m_attributes.AddAttribute(m_tex2d_variables[i].name, CVS_TEXTURE_2D, &m_tex2d_variables[i].tex, CVS_RNODES_INPUT);
	}
}


void MaterialProperties::AddValue(int loc, glm::vec4 value, std::string name)
{
	ShaderVariableContainerF4 var = { loc, value, name };

	m_vec4_variables.push_back(var);
}

void MaterialProperties::AddValue(int loc, glm::vec3 value, std::string name)
{
	ShaderVariableContainerF3 var = { loc, value, name };

	m_vec3_variables.push_back(var);
}

void MaterialProperties::AddValue(int loc, Texture* value, std::string name)
{
	ShaderVariableContainerT2D var = { loc, value, name };
}

void MaterialProperties::AddValue(int loc, float value, std::string name)
{
	ShaderVariableContainerF var = { loc, value, name };

	m_float_variables.push_back(var);
}

void MaterialProperties::AddValue(int loc, int value, std::string name)
{
	ShaderVariableContainerI var = { loc, value, name };

	m_int_variables.push_back(var);
}

void MaterialProperties::AddValue(int loc, glm::mat4 value, std::string name)
{
	ShaderVariableContainerM4 var = { loc, value, name };

	m_mat4_variables.push_back(var);
}

