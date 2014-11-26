#pragma once

// Cannot use forward declaration due to resource pool vector
#include "CVS_Resource.h"

struct CVS_ResourceImporter
{
	bool supports(const char* _fileFormat);

	void AddFormatSupport(const char* _fileFormat);

	void SetResourcePool(std::vector<CVS_Resource*>* _resourcePool);

	virtual bool init();
	virtual bool import(const char* _filePath);
	virtual bool halt();

protected:
	std::vector<std::string> m_supportedTypes;
	std::vector<CVS_Resource*>* m_pResourcePool;
};
