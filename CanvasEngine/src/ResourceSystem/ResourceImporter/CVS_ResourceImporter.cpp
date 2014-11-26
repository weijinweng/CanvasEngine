#include "CVS_ResourceImporter.h"
#include "CVS_ResourceSystem.h"

bool CVS_ResourceImporter::halt()
{
	return true;
}

bool CVS_ResourceImporter::import(const char* _filePath)
{
	return true;
}

bool CVS_ResourceImporter::init()
{
	return true;
}

void CVS_ResourceImporter::SetResourcePool(std::vector<CVS_Resource*>* _resourcePool)
{
	m_pResourcePool = _resourcePool;
}

void CVS_ResourceImporter::AddFormatSupport(const char* _fileFormat)
{
	// transform to lower case
	std::string fileFormat = _fileFormat;
	std::transform(fileFormat.begin(), fileFormat.end(), fileFormat.begin(), ::tolower);
	m_supportedTypes.push_back(fileFormat);
}

bool CVS_ResourceImporter::supports(const char* _fileFormat)
{
	for (auto i : m_supportedTypes)
	{
		if (i == _fileFormat)
		{
			return true;
		}
	}
	return false;
}

