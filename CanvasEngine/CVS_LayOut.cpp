#include "Canvas.h"

void CVS_Layout::onResize()
{

}

CVS_EditorLayout::CVS_EditorLayout(CVS_Gui* gui) : toolbarCell(this), rightbarCell(this), leftbarCell(this), bottomCell(this), renderCell(this)
{
	window = gui->window;
	CVS_ToolBar* ToolBar = new CVS_ToolBar(gui, 0, 0, 0, 0);
	CVS_Tab* Tab = new CVS_Tab(gui->window, 0, 0, 0, 0);
	CVS_Tab* Tabbot = new CVS_Tab(gui->window, 0, 0, 0, 0);
	view = new CVS_SceneView(gui, 0, 0, 0, 0);

	Tab->addTab("Scene", 0);
	Tab->addTab("Assets", 0);

	Tabbot->addTab("Debug", 0);

	ToolBar->AddButton(CVS_ARROW, 0, 0, 25, 25);
	ToolBar->AddButton(CVS_MOVE, 0, 0, 25, 25);
	ToolBar->AddButton(CVS_ROTATE, 0, 0, 25, 25);

	toolbarCell.SetObj(ToolBar);
	leftbarCell.SetObj(Tab);
	bottomCell.SetObj(Tabbot);
	renderCell.SetObj(view);


	toolbarCell.SetSize(0, 0, gui->window->getClientWidth(), 30);
	leftbarCell.SetSize(window->getClientWidth() - 300, 30, 300, window->getClientHeight() - 200);
	bottomCell.SetSize(0, window->getClientHeight() - 170, window->getClientWidth(), 170);
	renderCell.SetSize(0, 30, window->getClientWidth() - 300, window->getClientHeight() - 200);
}


void CVS_EditorLayout::onResize()
{
	toolbarCell.SetSize(0, 0, window->getClientWidth(), 30);
	leftbarCell.SetSize(window->getClientWidth() - 300, 30, 300, window->getClientHeight() - 200);
	bottomCell.SetSize(0, window->getClientHeight() - 170, window->getClientWidth(), 170);
	renderCell.SetSize(0, 30, window->getClientWidth() - 300, window->getClientHeight() - 200);
}

void CVS_EditorLayout::setScene(CVS_Scene* scene)
{
	this->view->Scene = scene->scene;
}
