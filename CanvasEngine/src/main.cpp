#include "CVS_Precompiled.h"
#include "Canvas.h"

int main(int argv, char* argc[])
{
	CVS_Initialize();
	Editor app;
	app.Initialize();
	app.Run();
	app.End();
	printf("Hi");
}