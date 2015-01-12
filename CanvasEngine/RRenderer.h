#pragma once

#include "DeviceContextGL.h"
#include "TextureNode.h"
#include "ShaderNode.h"
#include "Framebuffer.h"
#include "SceneNode.h"
#include "RPass.h"
#include "RScene.h"
#include "RendererNode.h"
#include "MaterialNode.h"

namespace Canvas{

	class RRenderer:public RendererNode{
	public:
		RScene* m_scene;

		glm::mat4 P;
		glm::mat4 V;
		glm::mat4 VP;

		RMaterialShaderNode* m_def_shdr;
		RPassNode m_def_pass;
		DeviceContext* m_ctx;
		std::vector<RRendererNodeLayer*> m_layers;
		
		/*Creates a renderer. _ctx can be NULL
		This will cause all member nodes to be uninstantiated
		but operatable. Video memory is occupied once a context
		is bound.*/
		RRenderer(DeviceContext* _ctx = NULL);
		RPassNode* GetDefaultPass();
		RPassNode* CreateNewPass();

		bool BindContext(DeviceContext* _ctx);

		RendererNode* Instance(std::string _name, RRenderer* _nthing);

		RendererNode* CreateNode(std::string _node_type, std::string _node_name);

		RScene* CreateScene();

		bool SetScene(RScene*);
		bool BindScene(RScene*);

		uint32 AddNode(uint32 _layer, RendererNode* _node);

		RMaterialShaderNode* SetDefaultMaterialShader(std::string _shader_id);
		RMaterialShaderNode* SetDefaultMaterialShader(uint32 _hash);
		RMaterialShaderNode* SetDefaultMaterialShader(RMaterialShaderNode* _shader);
		
		FramebufferNode* CreateNewFramebuffer();
		Texture2DNode* CreateTexture();
		
		bool DataLink();
		bool LoadLink();
		bool Link();
		bool CleanupLink();

		void DrawFrame();
	};

}