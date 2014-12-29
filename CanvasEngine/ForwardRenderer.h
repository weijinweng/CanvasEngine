#pragma once

#include "Renderer.h"

namespace Canvas{

	class ForwardRenderer :public Renderer{
	public:
		void BindScene(RenderScene*);
		void DrawFrame(const View&);
		
		glm::mat4 GetViewMatrix();
		glm::mat4 GetPerspectiveMatrix();
		glm::mat4 GetVPMatrix();

		MaterialNode* CreateDefaultMaterial();

		void RemoveMaterial(MaterialNode*);
		void AddMaterial(MaterialNode*);
		
		void Message(uint32, uint32, long);
	};

}