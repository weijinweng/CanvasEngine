#ifndef CVS_SUBSYSTEM
#define CVS_SUBSYSTEM

struct CVS_SubSystem{
public:
	virtual bool Initialize(){ return true;}
	virtual bool Update() {return true;}
	virtual bool End() {return true;}
};

#endif