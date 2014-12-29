#pragma once

#include "CanvasCore.h"

namespace Canvas{

	class RenderScene{
	public:
		std::vector<Material> m_mats;
		std::vector<MaterialNode> m_mat_nodes;
		std::vector<RenderNode*> m_nodes;
		RenderNode* CreateNode(const glm::mat4& _model, MaterialNode* _material);
		MaterialNode* CreateMaterialNode(std::string _type_name);
		MaterialNetwork* FetchShaderNetwork();

	};

}