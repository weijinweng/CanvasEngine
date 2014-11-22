#ifndef CVS_SUBSYSTEM
#define CVS_SUBSYSTEM

struct CVS_SubSystem{
public:
	virtual bool Initialize(){ return true;}
	virtual bool Update() {return true;}
	virtual bool End() {return true;}
};


#define RENDER_SELECTION WM_USER + 1
#define SCENE_LOAD WM_USER + 2

//WILL COME UP WITH BETTER NAME. But this is the core CVS communications.
struct CVS_Messagable{
	virtual LONG_PTR Message(UINT msg, UINT sParam, LONG_PTR lParam){ return 0; }
};


struct CVS_Timer
{
	int lastTime;
	int deltaTime;
	CVS_Timer();
	void beginTime();
	void endTime();
	int setFrame(UINT precision = 0);
};

#endif