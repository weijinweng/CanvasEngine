#pragma once

#include "RendererNode.h"

namespace Canvas{

	class RPassNode:public RRenderNode{
		std::vector<RProgramNode*> m_processes;
	public:
		RPassNode();
		RPassNode(RRenderer* _renderer, std::string _node_name);

		void Run();

		void AttachProgram(RProgramNode* _program);

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