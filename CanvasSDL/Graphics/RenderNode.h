#pragma once
#include "Canvas.h"
#include "CVS_Mesh.h"
#include "RenderNode.h"
#include "TransformNode.h"
#include "RenderSystem.h"

namespace Canvas
{
	//Renderable node within the program. Must contain a transform. 
	//If a mesh is not assigned, the node does not render.
	struct renderNode
	{
	public:
		std::string name;
		transformNode transform;
		renderProgram* programs;
		std::vector<CVS_Mesh*> meshes;
		std::vector<shaderTextureID> textureID;
		renderNode();
		bool setProgram(renderProgram* program);
		void Render();
		void setMesh(std::string meshName);
		void setTexture(std::string textureID, std::string texture);
		void initFromRenderNode(aiNode* child, std::vector<CVS_Mesh*> meshes);
		void initFromRenderNode(FbxNode* child, std::vector<CVS_Mesh*> meshes);
		renderNode(CVS_Mesh* mesh);
	};
}