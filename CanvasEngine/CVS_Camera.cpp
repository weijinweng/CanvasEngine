#include "CVS_RenderComponents.h"
#include "Canvas.h"

#define DEGTORAD 3.14159265358979323846/180.0

Plane::Plane() :d(0.f)
{

}

float Plane::dist(cvec3 point)
{
	return (d + glm::dot(normal, point));
}

void Plane::set3Vec(cvec3 v1, cvec3 v2, cvec3 v3)
{
	cvec3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = glm::cross(aux2, aux1);

	normal = glm::normalize(normal);
	point = v2;
	d = -(glm::dot(normal, point));
}

CVS_Frustum::CVS_Frustum(float FOV, float aspectRatio, float NearZ, float FarZ) 
{
	this->ratio = aspectRatio;
	this->angle = FOV;
	this->nearD = NearZ;
	this->farD = FarZ;

	tang = (float)std::tan(angle * glm::radians(FOV) * 0.5);
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD  * tang;
	fw = fh * ratio;
}

void CVS_Frustum::SetPerspective(float FOV, float aspectRatio, float NearZ, float FarZ)
{
	this->ratio = aspectRatio;
	this->angle = FOV ;
	this->nearD = NearZ;
	this->farD = FarZ;

	tang = (float)std::tan(angle* glm::radians(FOV) * 0.5);
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD  * tang;
	fw = fh * ratio;
}

void CVS_Frustum::CalculatePlanes(cvec3 p, cvec3 l, cvec3 u)
{
	cvec3 dir, nc, fc, X, Y, Z;

	Z = p - l;
	Z = glm::normalize(Z);

	X = glm::cross(u, Z);
	X = glm::normalize(X);

	Y = glm::cross(Z, X);

	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	pl[TOP].set3Vec(ntr, ntl, ftl);
	pl[BOT].set3Vec(nbl, nbr, fbr);
	pl[LEFT].set3Vec(ntl, nbl, fbl);
	pl[RIGHT].set3Vec(nbr, ntr, fbr);
	pl[NPLANE].set3Vec(ntl, ntr, nbr);
	pl[FPLANE].set3Vec(ftr, ftl, fbl);
}

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

CVS_Camera::CVS_Camera(cvec3 pos, cvec3 target, float FOV, float AspectRatio, float NearZ, float FarZ) :frustum(FOV, AspectRatio, NearZ, FarZ)
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

	UpdateView();
}



void CVS_Camera::UpdateView()
{
	glm::mat3 rot = glm::mat3_cast(transform.orientation);

	glm::vec3 targetDir = rot * glm::vec3( 0, 0, -1);

	glm::vec3 rightVec = rot * glm::vec3(1, 0, 0);

	glm::vec3 Up = glm::cross(rightVec, targetDir);

	View  = glm::lookAt(transform.translation, transform.translation + targetDir, Up);

	frustum.CalculatePlanes(transform.translation, transform.translation + targetDir, Up);
}

void CVS_Camera::UpdatePerspective()
{
	Persp = glm::perspective(FOV, aspectRatio, NearZ, FarZ);
	frustum.SetPerspective(FOV, aspectRatio, NearZ, FarZ);
	UpdateView();
}

void CVS_Camera::pointRotate(float x, float y, float z)
{

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
	view.frustum = &frustum;
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



