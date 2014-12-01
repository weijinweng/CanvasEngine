#include "Canvas.h"
#include "CVS_Resource.h"
#include "CVS_ResourceSystem.h"
#include "CVS_ResourceImporter.h"

#include "CVS_FbxImporter.h"

// Change to virtual path instead of name and type
CVS_Resource* CVS_ResourceSystem::get(const char* _fileName, CVS_Resource::EType _type)
{
	for (auto i : m_resources)
	{
		if (i->m_name == _fileName && i->m_type == _type)
		{
			return i;
		}
	}
	printf("Resource not found %s\n", _fileName);
	return nullptr;
}

bool CVS_ResourceSystem::import(const char* _filePath)
{
	// Get file extension, also transform to lower case
	std::string filePath = _filePath;
	std::string fileFormat = filePath.substr(filePath.find_last_of('.'), filePath.size());
	std::transform(fileFormat.begin(), fileFormat.end(), fileFormat.begin(), ::tolower);

	// Find a suitable loader
	for (auto i : m_importers)
	{
		if (i->supports(fileFormat.c_str()))
		{
			return i->import(_filePath);
		}
	}

	return false;
}

bool CVS_ResourceSystem::End()
{
	for (auto i : m_importers)
	{
		i->halt();
	}
	return true;
}

bool CVS_ResourceSystem::Initialize()
{
	m_importers.push_back(new CVS_FbxImporter);

	for (auto i : m_importers)
	{
		i->init();
		i->SetResourcePool(&m_resources);
	}
	return true;
}
