#pragma once
#include "Canvas.h"

namespace Canvas
{
	//Nodes of a transform Tree, the node memory are stored inside renderNodes
	struct transformNode
	{
		void calculateMatrix();
	public:
		//Attributes of transforms
		glm::vec3 translation;
		glm::vec3 scale;
		glm::fquat orientation;
		//Resultant matrix
		glm::mat4 transformMatrix;
		//Derived from assimp
		glm::mat4 assimpMatrix;
		//Node relationships
		transformNode* parent;
		std::vector<transformNode*> children;
		transformNode();
		//Tree modifiers
		void setParent(transformNode* parent);
		void addChild(transformNode* child);
		void removeChild(transformNode* child);
		void print(int level);
		void print();
		//Calculate down a transformTree
		void calculateAllMatrices();
		~transformNode();
	};
}