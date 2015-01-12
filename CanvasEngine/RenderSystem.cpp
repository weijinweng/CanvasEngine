#include "RenderSystem.h"

using namespace Canvas;

RenderSystemGL::RenderSystemGL() :SubSystem("RenderSystemGL")
{

}

RRenderer* RenderSystemGL::CreateNewRenderer(std::string _name)
{
	RRenderer* ret = new RRenderer();
	m_register[_name] = ret;
	return ret;
}

void Canvas::RegisterMaterialShader(std::string _name, std::string _vert_path, std::string _frag_path)
{
	RMaterialShaderNode* shdr = new RMaterialShaderNode(_name);
	shdr->LoadVertAndFrag(_vert_path, _frag_path);
	RenderSystemGL::m_sys_intance->m_mat_register.RegisterNode(shdr);
}

bool RenderSystemGL::init()
{
	m_sys_intance = this;
	return true;
}

void RenderSystemGL::update()
{

}

bool RenderSystemGL::end()
{
	return true;
}

void Canvas::InitCommonShaders()
{
	RegisterMaterialShader("Deferred_GBuffer_Pass", "./Shaders/Deferred_GBuffer.vert", "./Shaders/Deferred_GBuffer.frag");
}

void Canvas::InitCommonRenderSystems()
{
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new RShaderNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new RShaderProgramNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new RIteratorProgramNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new Texture2DNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new FramebufferNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new RenderbufferNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new RDrawProgramNode());
	RenderSystemGL::m_sys_intance->m_node_register.RegisterNode("General", new RPassNode());

	InitCommonShaders();

	InitCommonRenderer();

}

void Canvas::InitCommonRenderer()
{

	RRenderer* m_renderer = RenderSystemGL::m_sys_intance->CreateNewRenderer("Deferred");
	
	RPassNode* gbuffer_Pass = m_renderer->GetDefaultPass();

	RMaterialShaderNode* m_gbuffer_shader = m_renderer->SetDefaultMaterialShader("Deferred_GBuffer_Pass");
	gbuffer_Pass->AttachProgram(m_gbuffer_shader);

	FramebufferNode* gbuffer = m_renderer->CreateNewFramebuffer();

	Texture2DNode* gbuffer_tex[4];

	for (int i = 0; i < 4; ++i)
	{
		char buffer[10];

		gbuffer_tex[i] = m_renderer->CreateTexture();
		gbuffer_tex[i]->CreateConnection(m_renderer, "RenderWidth", "Width");
		gbuffer_tex[i]->CreateConnection(m_renderer, "RenderHeight", "Height");
		gbuffer_tex[i]->SetMagFilter(CVS_FILTER_CLOSEST);
		gbuffer_tex[i]->SetMinFilter(CVS_FILTER_CLOSEST);

		sprintf(buffer, "RT%d", i);

		gbuffer->CreateConnection(gbuffer_tex[i], "Texture", buffer);
	}

	RIteratorProgramNode* m_iterator_node = (RIteratorProgramNode*)m_renderer->CreateNode("General", "IteratorNode");
	RDrawProgramNode* m_light_shader = (RDrawProgramNode*)m_renderer->CreateNode("General", "DrawProgramNode");

	m_light_shader->LoadShaderFile(CVS_VERT_SHADER, "./Shaders/Deferred_PointLight.vert");
	m_light_shader->LoadShaderFile(CVS_FRAG_SHADER, "./Shaders/Deferred_PointLight.frag");
	m_light_shader->CreateConnection(m_renderer, "VP", "VP");
	m_light_shader->CreateConnection(m_renderer, "Sphere_VAO", "VAO");
	m_light_shader->SetDrawType(CVS_TRIANGLES);

	m_iterator_node->SetProgram(m_light_shader);
	m_iterator_node->SetSceneLayer("Point_Lights");

	m_iterator_node->CreateNodeProgramConnection("pos", "Light.pos");
	m_iterator_node->CreateNodeProgramConnection("diffuse", "Light.diffuse");
	m_iterator_node->CreateNodeProgramConnection("specular", "Light.specular");
	m_iterator_node->CreateNodeProgramConnection("ambient", "Light.ambient");
	m_iterator_node->CreateNodeProgramConnection("linearAtten", "Light.linearAtten");
	m_iterator_node->CreateNodeProgramConnection("constAtten", "Light.constAtten");
	m_iterator_node->CreateNodeProgramConnection("quadAtten", "Light.quadAtten");
	m_iterator_node->CreateNodeProgramConnection("rad", "rad");

	RPassNode* light_pass = m_renderer->CreateNewPass();

	light_pass->AttachProgram(m_iterator_node);

	gbuffer_Pass->CreateConnection(light_pass, "Pass", "Next");

}