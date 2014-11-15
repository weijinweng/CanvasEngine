#ifndef CVS_TRANSFORM_H
#define CVS_TRANSFORM_H

#include CVS_PRECOMPILED_H

struct CVS_Transform{
public:
	cmat4 transform;
	cvec3 translation;
	cvec3 scale;
	cquat orientation;
	CVS_Transform();
	void calculateMatrix();
};

struct CVS_TransformNode{
private:

public:
	CVS_Transform transform;
	CVS_TransformNode* parent;
	std::vector< CVS_TransformNode* >children;
	CVS_TransformNode();
	//Tree traversal
	void calculateAllMatrices();
	//Gets matrix from parent
	void calculateMatrix();
};

#endif