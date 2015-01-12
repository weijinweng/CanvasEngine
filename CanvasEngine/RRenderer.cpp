#include "RenderSystem.h"

using namespace Canvas;

RRenderer::RRenderer(DeviceContext* _ctx) :m_ctx(_ctx), m_def_pass(this, "Pass1"), RendererNode("Renderer", "Renderer")
{
	m_layers.resize(CVS_RRENDERER_TOTAL);

	std::string names[] = { std::string("All"), std::string("Passes"), std::string("Framebuffers"), std::string("Renderbuffers"), std::string("RScenes"), std::string("Textures"), std::string("Shaders") };

	for (int i = 0, e = CVS_RRENDERER_TOTAL; i < e; ++i)
	{
		m_layers[i] = new RRendererNodeLayer(names[i]);
	}
}

RPassNode* RRenderer::GetDefaultPass()
{
	return &m_def_pass;
}

RPassNode* RRenderer::CreateNewPass()
{
	char buffer[20];
	sprintf(buffer, "Pass%d\n", m_layers[CVS_RRENDERER_PASSES]->Size());
	return new RPassNode(this, std::string(buffer));
}

RendererNode* RRenderer::Instance(std::string _name, RRenderer* _nthing)
{
	printf("ERROR: Renderers can not be instanced!\n");
	return NULL;
}

RendererNode* RRenderer::CreateNode(std::string _node_type, std::string _node_name)
{
	return RenderSystemGL::InstantiateNode(_node_type, _node_name, this);
}

RMaterialShaderNode* RRenderer::SetDefaultMaterialShader(std::string _shader_id)
{
	m_def_shdr = (RMaterialShaderNode*)RenderSystemGL::FetchMaterialShader(_shader_id);
	return m_def_shdr;
}

RMaterialShaderNode* RRenderer::SetDefaultMaterialShader(uint32 _hash)
{
	m_def_shdr = (RMaterialShaderNode*)RenderSystemGL::FetchMaterialShader(_hash);
	return m_def_shdr;
}

RMaterialShaderNode* RRenderer::SetDefaultMaterialShader(RMaterialShaderNode* _shader)
{
	m_def_shdr = _shader;
	return _shader;
}

bool RRenderer::BindScene(RScene* _scene)
{
	RMaterialShaderNode* shdr = _scene->HasMaterialShader(m_def_shdr);
	if (shdr)
	{
		m_def_shdr = shdr;
		return true;
	}
	else  {
		m_def_shdr = _scene->AddMaterialShader(m_def_shdr);
		
		if (m_def_shdr)
		{
			return true;
		}
		return false;
	}
}

void RRenderer::DrawFrame()
{
	m_scene->UpdateRendererMatrices();
	m_def_pass.Run();
}