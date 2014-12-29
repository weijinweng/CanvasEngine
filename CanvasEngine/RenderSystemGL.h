#pragma once

#include "CanvasCore.h"

namespace Canvas{

	class RenderSystemGL:public SubSystem{
	
	public:
		RenderSystemGL();
		bool init();
		void update();
		bool end();
		static RenderSystemGL* m_system_intance;
	};


}