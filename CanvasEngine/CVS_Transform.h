#ifndef CVS_TRANSFORM_H
#define CVS_TRANSFORM_H

#include "CVS_Precompiled.h"

struct CVS_Transform{
public:
	cmat4 transform;
	cvec3 translation;
	cvec3 scale;
	cquat orientation;
	void calculateMatrix();
};

struct CVS_TransformNode{
private:
	void calculateMatrix();
public:
	CVS_Transform transform;
	CVS_TransformNode* parent;
	std::vector< CVS_TransformNode* >children;
	void calculateAllMatrices();
};

#endif