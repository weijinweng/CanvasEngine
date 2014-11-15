#pragma once
#include "Canvas.h"
#include "Vertex.h"

namespace Canvas
{
	//Storage of vertices datas
	//Me5mory located in Parent RenderSys
	class CVS_Mesh
	{
	public:
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		GLuint VAO;
		bool loadMeshFromFile(char* filePath);
		bool initMeshFromAiMesh(aiMesh* mesh);
		static CVS_Mesh* initFromFbxNode(FbxNode* inNode);
		CVS_Mesh();
		CVS_Mesh(std::string name);
		void initialize();
		CVS_Mesh(char* path, std::string name);
		~CVS_Mesh();

		// For every material, record the offsets in every VBO and triangle counts
		struct CVSSubMesh
		{
			CVSSubMesh() : m_indexOffset(0), m_triangleCount(0) {}

			int m_indexOffset;
			int m_triangleCount;
		};

		std::vector<CVSSubMesh*> m_subMeshes;
	};
}