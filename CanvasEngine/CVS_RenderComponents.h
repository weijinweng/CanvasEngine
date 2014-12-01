#ifndef CVS_RENDERSCENE_H
#define CVS_RENDERSCENE_H

#include "CVS_Precompiled.h"
#include "CVS_Transform.h"
#include "CVS_RenderProgram.h"
#include "CVS_Light.h"

struct CVS_VertexObject;
struct CVS_RenderNode;
struct CVS_RenderPackage;
struct CVS_RenderSystem;
struct CVS_View;

struct Plane{
	cvec3 normal, point;
	float d;

	Plane(cvec3 p1, cvec3 p2, cvec3 p3);
	Plane();
	void set3Vec(cvec3, cvec3, cvec3);
	void setCoefficients(float a, float b, float c, float d);
	float dist(cvec3 point);

};

struct Vertex{
	cvec3 position;
	cvec2 uv;
	cvec3 normal;
	cvec3 tangent;

	Vertex(cvec3 position, cvec2 uv, cvec3 normal);
};

struct BoundingSphere{
	cvec3 center;
	float radius;
};

struct CVS_Mesh{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	BoundingSphere bSphere;
	GLuint VAO;
	void initialize();
	void initializeFromAiMesh(const aiMesh* mesh);
	bool LoadFromFile(char* fpath);
	void Draw();
	void calculateTangents();
};

struct CVS_Frustum{
	enum FrustumEnum{
		TOP,
		LEFT,
		RIGHT,
		BOT,
		NPLANE,
		FPLANE
	};
	Plane pl[6];

	cvec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;
	CVS_Frustum(float FOV, float, float, float);
	bool Intersect(BoundingSphere sphere);
	void CalculatePlanes(cvec3 p, cvec3, cvec3);
	void SetPerspective(float FOV, float aspectRatio, float nearZ, float farZ);
};

struct CVS_View{
	cmat4 Pers;
	cmat4 View;
	CVS_Frustum* frustum;
};


struct CVS_Camera{
	CVS_Transform transform;

	cvec3 target;

	CVS_Frustum frustum;

	float aspectRatio;
	float FOV;
	float NearZ;
	float FarZ;

	cmat4 Persp;
	cmat4 View; 

	CVS_Camera(cvec3 pos = cvec3(4,3,4), cvec3 target = cvec3(0,0,0), 
	float FOV = 45.0f, float AspectRatio = 16.0f/9.0f, 
	float NearZ = 0.1f, float FarZ = 1000.0f);

	void UpdateView();
	void UpdatePerspective();

	void RotateAroundTarget(float,float,float);
	void pointRotate(float, float, float);
	void shiftLocalPos(float x, float y, float z);

	CVS_View getView();
};


struct CVS_RenderProgramInstance{
	CVS_RenderProgram* program;
	GLuint	viewLoc, mvpLoc, modelLoc;
	std::vector<CVS_RenderNode*> children;
	std::vector<CVS_RenderNode*> activeChildren;
	int numActive;
	CVS_RenderProgramInstance(CVS_RenderProgram* program);
	void addChild(CVS_RenderNode* node);
	void Render(CVS_View* cam);
	void CulledRender(CVS_View* cam);
	void BoundingSphereCull(CVS_View* view);
	void addActive(CVS_RenderNode* node);
};

struct CVS_TextureReference{
	CVS_Texture* texture;
	std::string name;
	int uniformLoc;
	CVS_TextureReference(CVS_Texture* texture, GLint uniformLoc, std::string uniformname);
};

struct CVS_RenderNode{
	void* msgData;
	int type;
	CVS_RenderProgramInstance* parent;
	CVS_Mesh* mesh;
	std::vector<CVS_TextureReference> textures;
	cmat4 modelMatrix;
	CVS_RenderNode(CVS_RenderProgramInstance* parent);
	int voidGetTextureHash(std::string texture);
	void BindTexture(CVS_Texture* texture, int location);
	bool SetTexture(std::string, CVS_Texture*);
	void Render(CVS_View* view);
	void setMesh(CVS_Mesh* mesh);
	bool CheckActiveBSphere(CVS_View* view);
};

struct CVS_RenderScene{
	std::vector<CVS_RenderNode*> nodes;
	std::vector<CVS_RenderProgramInstance*> programs;
	std::vector<CVS_Light*> lights;
	std::vector<CVS_Light*> dirLights;

	CVS_RenderScene();
	CVS_RenderNode* createNewNode();
	
	void addProgram(std::string name);
	void Draw(CVS_View* view);
	void OptimizedDraw(CVS_View* view);
	void AddLight(CVS_Light* light);
	void FrustumCull(CVS_View* view);
};

#endif