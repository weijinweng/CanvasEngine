#include "CVS_Transform.h"

CVS_Transform::CVS_Transform() :orientation(cvec3(0, 0, 0)), 
								translation(0.0f),
								scale(1.0f)
{

}

void CVS_Transform::calculateMatrix()
{
	this->transform = cmat4(1.0f);

	this->transform = glm::scale(this->transform, scale);

	this->transform = glm::mat4_cast(orientation) * this->transform;

	this->transform = glm::translate(this->transform, translation);
}

CVS_TransformNode::CVS_TransformNode():parent(NULL)
{

}

void CVS_TransformNode::calculateMatrix()
{
	transform.calculateMatrix();
	if (parent != NULL)
	{
		transform.transform = parent->transform.transform * transform.transform;
	}
}