#include "CanvasCore.h"
#include "Window.h"
using namespace Canvas;

bool Canvas::PullEvent(Event& e)
{
	return CVS_Server.getNextEvent(e);
}


bool Canvas::Initialize(uint32 flag, HINSTANCE instance, HINSTANCE prevInstance, int nCmdShow)
{
	RegisterSubSystem(new WindowSystem());

	return CVS_Server.Init(flag, instance, prevInstance);
}



void Canvas::Run()
{

	CVS_Server.Run();
}

bool Canvas::End()
{
	return CVS_Server.End();
}

