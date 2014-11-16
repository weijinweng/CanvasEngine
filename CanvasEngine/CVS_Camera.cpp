#include "CVS_RenderComponents.h"
#include "Canvas.h"



glm::quat CreateQuatFromTwoVectors(cvec3 U, cvec3 V)
{
	cvec3 w = glm::cross(U,V);
	glm::quat q = glm::quat(glm::dot(U,V), w.x, w.y, w.z);
	q.w += sqrt(q.x*q.x + q.w*q.w + q.y*q.y + q.z*q.z);
	return glm::normalize(q);
}

glm::quat lookToTarget(cvec3 pos, cvec3 target)
{
	cvec3 forVec = glm::normalize(target - pos);

	float dotp = glm::dot(cvec3(0, 0, -1), forVec);

	float rotAng = acos(dotp);

	glm::vec3 rotAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), forVec));

	return glm::quat(rotAxis * rotAng);
}

CVS_Camera::CVS_Camera(cvec3 pos,cvec3 target, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	this->transform.translation = pos;

	cvec3 forVec = glm::normalize(target - pos);

	float dotp = glm::dot(cvec3(0, 0, -1), forVec);

	float rotAng = acos(dotp);
	
	glm::vec3 rotAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), forVec));


	this->transform.orientation =glm::normalize(glm::quat(rotAng * rotAxis));

	this->transform.scale = glm::vec3(1.0f);


	this->FOV = FOV;
	this->aspectRatio = AspectRatio;
	this->NearZ = NearZ;
	this->FarZ = FarZ;

	this->target = target;

	RotateAroundTarget(0, 0, 0);
}



void CVS_Camera::UpdateView()
{
	glm::mat3 rot = glm::mat3_cast(transform.orientation);

	glm::vec3 targetDir = rot * glm::vec3( 0, 0, -1);

	glm::vec3 rightVec = rot * glm::vec3(1, 0, 0);

	glm::vec3 Up = glm::cross(rightVec, targetDir);

	View  = glm::lookAt(transform.translation, transform.translation + targetDir, Up);

}

void CVS_Camera::UpdatePerspective()
{
	Persp = glm::perspective(FOV, aspectRatio, NearZ, FarZ);
}

void CVS_Camera::pointRotate(float x, float y, float z)
{
	cvec3 pos = transform.translation;

	cquat rotQuat = cquat(glm::vec3(x, y, z));

	glm::mat3 rotMat = glm::mat3_cast(rotQuat);

	pos -= target;

	pos = rotMat * pos;

	pos += target;

	transform.translation = pos;

	View = glm::lookAt(pos, target, cvec3(0, 1, 0));
}


//Precision error right now, DONOT USE ONLY FOR TESTING
void CVS_Camera::RotateAroundTarget(float x, float y, float z)
{

	cquat pitchQuat = glm::angleAxis(x, cvec3(1, 0, 0));
	cquat yawQuat = glm::angleAxis(y, cvec3(0, 1, 0));

	transform.orientation = glm::normalize(transform.orientation * pitchQuat);
	transform.orientation = glm::normalize(yawQuat * transform.orientation);
	UpdateView();
}

CVS_View CVS_Camera::getView()
{
	CVS_View view;
	view.Pers = Persp;
	view.View = View;
	return view;
}

void CVS_Camera::shiftLocalPos(float x, float y, float z)
{
	glm::mat3 rotMat = glm::mat3_cast(transform.orientation);

	cvec3 V = rotMat * cvec3(0, 0, -1);
	cvec3 U = cvec3(0, 1, 0);
	cvec3 R = glm::cross(V, U);
	U = glm::cross(R, V);

	transform.translation += V * z;
	transform.translation += R * x;
	transform.translation += U * y;
	UpdateView();
}



