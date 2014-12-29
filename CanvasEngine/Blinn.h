#pragma once

#include "SceneNode.h"
#include "Material.h"

namespace Canvas{
	struct BlinnCache{
		
	};

	class BlinnMaterialTemplate:public MaterialTemplate{
	public:
		BlinnMaterialTemplate();
		void SetDrawProgram(DrawProgram*);
		MaterialNode* GenNewNode(std::string = "Blinn");
		void DeleteNode(MaterialNode*);
	};

	struct BlinnMaterialNode:public MaterialNode{
		BlinnMaterialNode(DrawProgram*);
		uint32 CacheProgram(DrawProgram*);
		void Bind(uint32 = 0);

		void Set(uint32, float);
		void Set(uint32, int);
		void Set(uint32, glm::vec3);
		void Set(uint32, glm::mat4);
		void Set(uint32, glm::vec4);
		void Set(uint32, bool);

		void SetActivity(bool);
		void SetInputTexture(int loc, Texture*);
		void AddNode(RenderNode*);
		virtual std::string FetchID();
		std::string GetName();
	};
}