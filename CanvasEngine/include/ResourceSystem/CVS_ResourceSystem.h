#pragma once
#include "CVS_SubSystem.h"
#include "CVS_Resource.h"

#include "CVS_Mesh.h"
#include "CVS_Skeleton.h"
#include "CVS_Animation.h"

class CVS_ResourceImporter;
struct CVS_ResourceSystem : CVS_SubSystem
{

public:
	virtual bool Initialize();

	virtual bool End();

	bool import(const char* _filePath);
	CVS_Resource* get(const char* _resourceName, CVS_Resource::eType _type);

	// Data types
	std::vector<CVS_ResourceImporter*> m_importers;
	std::vector<CVS_Resource*> m_resources;

	// Helper functions
	//FbxScene* _initFBXScene(char* filepath);
	//CVS_Scene* loadFileToScene(char* filename, CVS_Scene* _pScene);

};