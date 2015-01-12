#include "RRenderer.h"

using namespace Canvas;

Texture2DNode::Texture2DNode() :RendererNode("Template", "Texture2DNode")
{

}

Texture2DNode::Texture2DNode(std::string _name, RRenderer* _renderer) : RendererNode(_name, "Texture2DNode")
{
	SetRenderer(_renderer);
}

void Texture2DNode::SetMagFilter(uint32 _filter)
{
	m_mag_flt = _filter;
}

void Texture2DNode::SetMinFilter(uint32 _filter)
{
	m_min_flt = _filter;
}

bool Texture2DNode::DataLink()
{
	if (!m_tex)
	{
		m_tex = m_renderer->m_ctx->AllocTexture();
		
		if (m_tex)
		{
			return true;
		}
		return false;
	}
}

bool Texture2DNode::LoadLink()
{
	if (m_tex)
	{
		if (m_filepath != "NULL")
		{
			//TODO:~ Load file
			printf("File load not yet implemented. Do it now!\n");
		}
		m_tex->BufferData(NULL, m_width, m_height, 0, m_tex_format, CVS_FLOAT);
		m_tex->SetMagFilter(m_mag_flt);
		m_tex->SetMinFilter(m_min_flt);
	}
}

bool Texture2DNode::Link()
{
	return true;
}

bool Texture2DNode::CleanupLink()
{
	return true;
}

void 
