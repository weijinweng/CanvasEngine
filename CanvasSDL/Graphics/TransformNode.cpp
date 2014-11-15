#include "TransformNode.h"

namespace Canvas
{
	transformNode::transformNode() :translation(0, 0, 0), orientation(glm::vec3(0, 0, 0)), scale(1, 1, 1)
	{
		parent = NULL;
	}

	void transformNode::setParent(transformNode* parent)
	{
		if (this->parent != NULL)
		{
			this->parent->removeChild(this);
		}
		parent->addChild(this);
	}

	void transformNode::removeChild(transformNode* child)
	{
		for (std::vector<transformNode*>::iterator it = children.begin(), et = children.end(); it != et; it++)
		{
			if ((*it) == child)
				children.erase(it);
		}
	}

	void transformNode::addChild(transformNode* child)
	{
		children.push_back(child);
		child->parent = this;
	}

	void transformNode::calculateMatrix()
	{
		transformMatrix = glm::mat4(1.0);

		transformMatrix = glm::scale(transformMatrix, scale);
		transformMatrix = transformMatrix*glm::mat4_cast(orientation);
		transformMatrix = glm::translate(transformMatrix, translation);


		if (parent != NULL)
		{
			transformMatrix = parent->transformMatrix * transformMatrix;
		}
	}

	void transformNode::calculateAllMatrices()
	{
		calculateMatrix();
		for (int i = 0; i < children.size(); i++)
		{

			children[i]->calculateAllMatrices();
		}
	}

	void transformNode::print(int level)
	{
		printf("subnode\n");
		printf("translate x%d y%d z%d\n", translation.x, translation.y, translation.z);
		for (int i = 0, e = children.size(); i < e; ++i)
		{
			for (int j = 0, je = level; j < je; j++)
			{
				printf("-");
			}
			children[i]->print(level + 1);
		}

	}

	void transformNode::print()
	{
		printf("node\n");
		printf("translate x%d y%d z%d\n", translation.x, translation.y, translation.z);
		for (int i = 0, e = children.size(); i < e; ++i)
		{
			printf("-");
			children[i]->print(2);
		}
	}

	transformNode::~transformNode()
	{}
}