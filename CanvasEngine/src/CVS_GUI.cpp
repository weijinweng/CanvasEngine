#include "Canvas.h"
#include "CVS_GUI.h"
#include "CVS_2DTools.h"
#include "CVS_WindowSystem.h"

extern CVS_StateMachine GLOBALSTATEMACHINE;

CVS_GUI_OBJ::CVS_GUI_OBJ():texture(NULL), bit(false), bitColor(BLACK)
{
}

void CVS_GUI_OBJ::onResize()
{
}

void CVS_GUI_OBJ::addBitmap(char* imgPath)
{
	bit = true;
	texture = GLOBALSTATEMACHINE.m_RenderSub.generateNewTexture();
	texture->loadFile(imgPath);
}

CVS_ColorRGBA::CVS_ColorRGBA()
{
}

CVS_ColorRGBA::CVS_ColorRGBA(float r, float g, float b, float a):r(r), g(g), b(b), a(a)
{
}

void CVS_ColorRGBA::set(float r, float g, float b, float a)
{
	if(r >= 0)
		this->r = r;
	if(g >= 0)
		this->g = g;
	if(b >= 0)
		this->b = b;
	if(a >= 0)
		this->a = a;
}

bool getMouseOver(CVS_IRECT rect, int x, int y)
{
	if(rect.x > x
		||rect.x + rect.w < x
		||rect.y > y
		||rect.y + rect.h < y)
		return false;
	return true;
}

CVS_TextNode::CVS_TextNode(CVSFontHandle handle):font(GLOBALSTATEMACHINE.m_RenderSub.fonts[handle].face)
{
	texture = GLOBALSTATEMACHINE.m_RenderSub.generateNewTexture();
	size.x = 0;
	size.y = 0;
}

bool CVS_TextNode::addChar(char letter)
{
	if(letter != ' ')
	{
		updated = false;
		text += letter;		
		return true;
	}
	return false;
}

void CVS_TextNode::renderText(int x, int y, CVS_2DTools* tools)
{
	if(!updated)
	{
		SDL_Color color = {0,0,0,1};
		SDL_Surface* fonts = TTF_RenderText_Blended(font, text.c_str(), color);

		texture->loadSDL_Surface(fonts);
	}
	CVS_IRECT vec = {x ,y, size.x, size.y};

	CVS_FRECT rect = tools->ConvertToScreenCoords(vec);

	tools->drawTextureRect(texture, rect.x, rect.y, rect.w, rect.h);

}

CVS_Button::CVS_Button(int x, int y, int w, int h):callBack(NULL),bundle(NULL),mouseDown(false)
{
	posType = CVS_POS_RELATIVE;
	offHoverColor.set(1.0f, 1.0f,1.0f,1.0f);
	mouseUpColor.set(0.7f,0.7f, 0.7f,1.0f);
	onHoverColor.set(0.7,0.7,0.7,1.0f);
	mouseDownColor.set(0.3,0.3,0.3,1.0f);

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	font = 0;
}

void CVS_Button::setText(std::string text)
{
	
	this->text.clear();
	for(const char* p = text.c_str(); *p;)
	{
		CVS_TextNode node(this->font);
		while(node.addChar(*p))
		{
			this->text.push_back(node);
			p++;
		}
	}
}

bool CVS_Button::getMouseDown(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		mouseDown = true;
		color = mouseDownColor;
		bitColor.set(1.0,1.0,1.0,1.0);
		return true;
	}
	return false;
}

bool CVS_Button::getMouseUp(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		if(mouseDown && callBack != NULL)
		{
			(*callBack)(bundle);
		}
		color = mouseUpColor;
		bitColor = mouseDownColor;
	}
	mouseDown = false;

	return mouseDown;
}

void CVS_Button::onHover(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		if(!mouseDown)
		{
			color = onHoverColor;
			bitColor = mouseDownColor;
		}
	} else offHover();
}

void CVS_Button::offHover()
{
	if(!mouseDown)
	{
		color = offHoverColor;
		bitColor = BLACK;
	}
}

void CVS_Button::setOnClickFunction( void function(void* bundle), void* data)
{
	callBack = function;
	this->bundle = data;
}

void CVS_Button::Render(CVS_2DTools* tools)
{

	CVS_FRECT rect = {0,0,0,0};
	rect = tools->ConvertToScreenCoords(this->rect);

	tools->drawRect(color, rect.x, rect.y, rect.w, rect.h);
	if(texture != NULL)
	{
		if(bit)
		{
			tools->drawBit(texture, bitColor, rect.x, rect.y, rect.w, rect.h);
		}
		else tools->drawTextureRect(texture, rect.x, rect.y, rect.w, rect.h);
	}
}


handleBar::handleBar(CVS_Gui_Cell* cell):parent(cell),initialized(false),mouseDown(false),active(true),min(0),max(0)
{
}

void handleBar::initialize(bool horizontal, int loc)
{
	if(initialized)
		return;
	if(horizontal)
	{
		printf("Added HandleBar\n");
		this->v1 = &(parent->cell1->rect.h);
		this->v2 = &(parent->cell2->rect.y);
		rect.w = parent->rect.w;
		rect.x = parent->rect.x;
		rect.h = 5;
		rect.y = parent->rect.y + loc;
	} else {
		printf("Added HandleBar\n");
		this->v1 = &(parent->cell1->rect.w);
		this->v2 = &(parent->cell2->rect.x);
		rect.h = parent->rect.h;
		rect.y = parent->rect.y;
		rect.w = 5;
		rect.x = parent->rect.x + loc;
	}
	initialized = true;
	this->horizontal = horizontal;
	sync();
}

bool handleBar::getMouseDown(int x, int y)
{
	if(!this->active)
		return false;
	if(getMouseOver(rect, x, y)  && initialized)
	{
		color.set(0.3,0.3,0.3,1.0);
		mouseDown = true;
		mouseX = x - rect.x;
		mouseY = y - rect.y;
		return true;
	}
	return false;
}

bool handleBar::getMouseUp(int x, int y)
{
	if(!active)
		return false;
	if(getMouseOver(rect, x, y) && initialized)
	{
		color.set(0.0, 0.0, 0.0, 1.0);
	}
	color.set(0.0, 0.0, 0.0, 1.0);
	mouseDown = false;
	mouseX = 0;
	mouseY = 0;
	return true;
}

float fClamp(float value, float min, float max)
{
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}


bool handleBar::onHover(int x, int y)
{
	if(!active)
		return false;
	if(getMouseOver(rect,x,y) && initialized)
	{
		if(mouseDown)
		{
			if(horizontal)
			{
				rect.y = y- mouseY;
				rect.y = fClamp(rect.y, parent->rect.y + min, parent->rect.y + parent->rect.h - rect.h - max);
			} else {
				rect.x =x - mouseX;
				rect.x = fClamp(rect.x, parent->rect.x + min, parent->rect.x + parent->rect.w - rect.w - max);
			}
			sync();
			} else {
				color.set(0.7,0.7,0.7,1.0);
		}
		return true;
	}
	offHover();
	return false;
}

void handleBar::offHover()
{
	if(!active)
		return;
	if(mouseDown)
	{
		int x,y;
		SDL_GetMouseState(&x,&y);
			if(horizontal)
			{
				rect.y = y- mouseY;
				rect.y = fClamp(rect.y, parent->rect.y + min, parent->rect.y + parent->rect.h - rect.h - max);
			} else {
				rect.x =x - mouseX;
				rect.x = fClamp(rect.x, parent->rect.x + min, parent->rect.x + parent->rect.w - rect.w - max);
			}
		sync();
	} else {
		color.set(0.0,0.0,0.0,1.0f);
	}
}

void handleBar::sync()
{
	if(!initialized)
		return;
	if(horizontal){
		*v1 = rect.y;
		*v2 = rect.y;
		rect.x = this->parent->rect.x;
		rect.w = this->parent->rect.w;
	} else {
		*v1 = rect.x;
		*v2 = rect.x;
		rect.y = this->parent->rect.y;
		rect.h = this->parent->rect.h;
	}
	this->parent->UpdatePos();
}	

void handleBar::Render(CVS_2DTools* tools)
{
	if(!active)
		return;
	CVS_FRECT rectangle = tools->ConvertToScreenCoords(rect);
	tools->drawRect(color, rectangle.x, rectangle.y, rectangle.w, rectangle.h);
}

CVS_Gui_Cell::CVS_Gui_Cell(CVS_Gui_Cell* cell):bar(this),cell2(NULL),cell1(NULL),parent(cell),gui(NULL){
	this->rect = cell->rect;
	debugColor = WHITE;
}

CVS_Gui_Cell::CVS_Gui_Cell(CVS_Gui* gui):bar(this), cell1(NULL), cell2(NULL),gui(gui),parent(NULL){
	debugColor = WHITE;
}

void CVS_Gui_Cell::Divide(bool horizontal, int loc)
{
	cell1 = new CVS_Gui_Cell(this);
	cell2 = new CVS_Gui_Cell(this);
	bar.initialize(horizontal, loc);
}

bool CVS_Gui_Cell::getMouseDown(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		if(bar.getMouseDown(x,y))
			return true;
		for( int i = 0, e = children.size(); i < e; ++i)
		{
			if(children[i]->getMouseDown(x, y))
				return true;
		}
		if(cell1)
			cell1->getMouseDown(x,y);
		if(cell2)
			cell2->getMouseDown(x,y);
		return true;
	}
	return false;
}

bool CVS_Gui_Cell::getMouseUp(int x, int y)
{
	bool over = false;
	if(getMouseOver(rect, x, y))
	{
		over = true;
	}

	bar.getMouseUp(x,y);
	for( int i = 0, e = children.size(); i < e; ++i)
	{
		if(children[i]->getMouseUp(x,y))
			return true;
	}
	if(cell1)
		cell1->getMouseUp(x,y);
	if(cell2)
		cell2->getMouseUp(x,y);

	return over;
}

void CVS_Gui_Cell::onHover(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		if(bar.onHover(x,y))
			return;
		for( int i = 0, e = children.size(); i < e; ++i)
		{
			children[i]->onHover(x,y);
		}
		if(cell1)
		{
			cell1->onHover(x,y);
		
			cell2->onHover(x,y);
		}
		return;
	}
	offHover();
}

void CVS_Gui_Cell::offHover()
{
	bar.offHover();
	for( int i = 0, e = children.size(); i < e; ++i)
	{
		children[i]->offHover();
	}
}

void CVS_Gui_Cell::Render(CVS_2DTools* tools)
{
	CVS_FRECT rectangle = tools->ConvertToScreenCoords(rect);

	tools->drawRect(debugColor, rectangle.x, rectangle.y, rectangle.w, rectangle.h);


	for(int i = 0; i < children.size(); ++i)
	{
		children[i]->Render(tools);
	}
	if(cell1 != NULL)
		cell1->Render(tools);
	if(cell2 != NULL)
		cell2->Render(tools);
	bar.Render(tools);
}

void CVS_Gui_Cell::UpdatePos()
{
	cell1->onResize();
	cell2->onResize();
}

void CVS_Gui_Cell::onResize()
{
	int offX = rect.x, offY = rect.y;
	for(int i = 0, e = children.size(); i < e; ++i)
	{
		if(children[i]->posType == CVS_POS_RELATIVE)
		{


			if( i > 0 && offX + children[i]->rect.w > rect.x + rect.w)
			{
				offY += children[i-1]->rect.h;
				offX = rect.x;
			}

			children[i]->rect.x = offX;
			children[i]->rect.y = offY;
			offX = children[i]->rect.w + children[i]->rect.x;
		}
	}
	if(!bar.initialized)
		return;
	if(bar.horizontal)
	{
		cell1->rect.x = rect.x;
		cell2->rect.x = rect.x;
		cell1->rect.w = rect.w;
		cell2->rect.x = rect.w;
		cell2->rect.h = rect.h + rect.y - cell2->rect.y;

		bar.rect.x = rect.x;
		bar.rect.w = rect.w;
	} else {
		cell1->rect.y = rect.y;
		cell2->rect.y = rect.y;
		cell1->rect.h = rect.h;
		cell2->rect.h = rect.h;
		cell2->rect.w = rect.x + rect.w - cell2->rect.x;

		bar.rect.y = rect.y;
		bar.rect.h = rect.h;
	}
	cell1->onResize();
	cell2->onResize();
}

void CVS_Gui_Cell::setHandleBarMax(int max)
{
	bar.max = max;
}

void CVS_Gui_Cell::setHandleBarMin(int min)
{
	bar.min = min;
}

CVSButtonHandle CVS_Gui_Cell::addButton(int x, int y, int w, int h,  void function(void* bundle), void* data)
{
	int offX = x + rect.x, offY = y + rect.y;
	if(children.size() > 0)
	{
		offX = children.back()->rect.x + children.back()->rect.w;
		offY = children.back()->rect.y;
		if(offX + w > rect.w + rect.x)
		{
			offY += children.back()->rect.h;
			offX = children.back()->rect.x;
		}
	}
	CVS_Button* button = new CVS_Button(offX, offY , w, h);
	button->setOnClickFunction(function, data);
	children.push_back(button);
	return children.size()-1;
}

CVS_Gui::CVS_Gui(CVS_Window* window):window(window),mainCell(this)
{
	mainCell.rect.h = window->height;
	mainCell.rect.w = window->width;
	mainCell.rect.x = 0;
	mainCell.rect.y = 0;
}

CVS_Gui_SceneRenderer::CVS_Gui_SceneRenderer(int x, int y, int w, int h, CVS_RenderScene* scene, CVS_Window* parent)
{
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	this->scene = scene;
	this->parent = parent;
}

void CVS_Gui_SceneRenderer::Render(CVS_2DTools* tools)
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH);
	
	CVS_FRECT rectangle = tools->ConvertToScreenCoords(rect);

	tools->drawRect(BLACK, rectangle.x, rectangle.y, rectangle.w, rectangle.h);
	if(scene != NULL)
	{

	}
	glDisable(GL_DEPTH);
	glEnable(GL_BLEND);
	glViewport(0,0, parent->width, parent->height);
}

bool CVS_Gui_SceneRenderer::getMouseDown(int x, int y)
{
	return true;
}

bool CVS_Gui_SceneRenderer::getMouseUp(int x, int y)
{
	return true;
}

void CVS_Gui_SceneRenderer::onHover(int x, int y)
{
	return;
}

void CVS_Gui_SceneRenderer::offHover()
{
}



CVS_Button* CVS_Gui::addButton(int x, int y, int w, int h, void (function)(void* bundle), void* bundle)
{
	this->buttons.push_back(new CVS_Button(x, y, w, h));
	((CVS_Button*)buttons.back())->setOnClickFunction(function, bundle);
	return (CVS_Button*)buttons.back();
}


void CVS_Gui::Update()
{
	mainCell.Render(window->renderer->tools);
	for(int i = 0; i < buttons.size(); ++i)
	{
		buttons[i]->Render(window->renderer->tools);
	}
}

void CVS_Gui::ParseInputs(SDL_Event e)
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	if(e.type == SDL_MOUSEBUTTONDOWN)
	{

		for(int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->getMouseDown(x,y);
		}
		mainCell.getMouseDown(x,y);
	}
	else if(e.type == SDL_MOUSEBUTTONUP)
	{
		for(int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->getMouseUp(x,y);
		}
		mainCell.getMouseUp(x,y);
	}
	else if(e.type == SDL_MOUSEMOTION)
	{
		for(int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->onHover(x,y);
		}
		mainCell.onHover(x, y);
	}

}