#pragma once

#include "CanvasCore.h"
#include "TextureGL.h"

namespace Canvas{

	class Renderbuffer{
	public:
		void SetAttachmentType(uint32);
		void SetPixelFormat(uint32);
		void SeStorage(int w, int h);
	};

	class Framebuffer{
	public:

		void Bind();
		void DrawBind();
		void ReadBind();

		void BindRenderTarget(Texture* _render_target);
		void BindRenderTarget(Texture**, int _size);
		void BindAttachment(Renderbuffer*);

		void UpdateBuffers();
		void Free();
	};

}