#pragma once
#include "Externals.h"

//Texture compression format definitions
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII


//Enumerators for various CVS flags
enum CANVASENUM{
	CVS_NULL = 0,
	CVS_WDW_FULLSCREEN = 1,
	CVS_WDW_WINDOW = 1 << 1,//2
	CVS_TXT_RGB = 1<< 2,//4
	CVS_TXT_RGBA = 1 << 3,//8
	CVS_LGT_POINT = 3,
	CVS_LGT_SPOT = 5,
	CVS_LGT_DIRECTIONAL = 6
};

//Default timer, highest accuracy expressed in milliseconds
class Timer{
	int lastFrameTime;
	int timeMS;
	double timeSec;
	Timer():timeMS(0),timeSec(0.0) {}
	//Sets the time in the timer from the last time this function was called
	void setFrame(){
		timeMS = lastFrameTime - timeMS;
		timeSec = (double)timeMS/1000;
	}
};

//Part of major Canvas Sytems require a end and initialization function
class Subsystem{
public:
	Subsystem(){}
	virtual bool initialize(){return false;}
	virtual bool end(){return false;}
	~Subsystem(){}
};