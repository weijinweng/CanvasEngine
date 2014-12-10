#include "CanvasCore.h"
#include "Window.h"
using namespace Canvas;

StateMachine CVS_Server;

void RegisterSubSystem(SubSystem* sys)
{
	CVS_Server.addSubSystem(sys->name, sys);
}

bool Initialize(uint32 flag)
{
	RegisterSubSystem(new WindowSystem());

	CVS_Server.Init(flag);
}

