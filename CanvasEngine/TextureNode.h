#pragma once

#include "RendererNode.h"
#include "TextureGL.h"

namespace Canvas{
	
	class Texture2DNode :public RendererNode{
		Texture* m_tex;
		std::string m_filepath;

		uint32 m_tex_format;

		uint32 m_width;
		uint32 m_height;
		
		uint32 m_min_flt;
		uint32 m_mag_flt;
	public:
		Texture2DNode();
		Texture2DNode(std::string _name, RRenderer* _renderer);

		void SetMagFilter(uint32);
		void SetMinFilter(uint32);

		void BufferData(void* _data, uint32 _width, uint32 _height, uint32 _input_type);

		bool DataLink();
		bool LoadLink();
		bool Link();
		bool CleanupLink();

		bool Initiate();
		RendererNode* Instance(std::string _name, RRenderer* _scene);
		void Unbind();
		bool Rebind();
		bool CreateConnection(RendererNode* _in_node, std::string _in_attrib, std::string _reciever_node);

	};
}