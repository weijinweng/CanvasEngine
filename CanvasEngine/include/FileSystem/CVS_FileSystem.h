
#include "CVS_SubSystem.h"

const int MAX_TIMEOUT = 60 * 1000; // 1 minute

struct CVS_File
{
	CVS_File();
	~CVS_File(void);

	CVS_File* OpenAsync(const char* _pFileName)
	{

	}

	CVS_File* Open(const char* _pFileName)
	{

	}

	bool IsOpen();
	bool AbortIO();
};

struct CVS_FileSystem : public CVS_SubSystem
{

};

