#ifndef CVS_SUBSYSTEM
#define CVS_SUBSYSTEM

struct CVS_SubSystem{
public:
	virtual bool Initialize();
	virtual bool End();
};

#endif