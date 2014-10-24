#include "Canvas.h"
#include "CVS_GUI.h"
#include "CVS_2DTools.h"
#include "CVS_WindowSystem.h"

extern CVS_StateMachine GLOBALSTATEMACHINE;

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
	}
	mouseDown = false;

	return mouseDown;
}

void CVS_Button::onHover(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		if(!mouseDown)
			color = onHoverColor;
	} else offHover();
}

void CVS_Button::offHover()
{
	if(!mouseDown)
		color = offHoverColor;
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
}


handleBar::handleBar(CVS_Gui_Cell* cell):parent(cell),initialized(false),mouseDown(false)
{
}

void handleBar::initialize(bool horizontal, int loc)
{
	if(initialized)
		return;
	if(horizontal)
	{
		this->v1 = &(parent->cell1->rect.h);
		this->v2 = &(parent->cell2->rect.y);
		rect.w = parent->rect.w;
		rect.x = parent->rect.x;
		rect.h = 10;
		rect.y = loc;
	} else {
		printf("Added HandleBar\n");
		this->v1 = &(parent->cell1->rect.w);
		this->v2 = &(parent->cell2->rect.x);
		rect.h = parent->rect.h;
		rect.y = parent->rect.y;
		rect.w = 10;
		rect.x = loc;
	}
	initialized = true;
	this->horizontal = horizontal;
	sync();
}

bool handleBar::getMouseDown(int x, int y)
{
	if(getMouseOver(rect, x, y)  && initialized)
	{
		color.set(0.3,0.3,0.3,1.0);
		mouseDown = true;
		mouseX = x;
		mouseY = y;
		return true;
	}
	return false;
}

bool handleBar::getMouseUp(int x, int y)
{
	printf("WTF wokr\n");
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

void handleBar::onHover(int x, int y)
{

	if(getMouseOver(rect,x,y) && initialized)
	{
		if(mouseDown)
		{
			printf("Not supposed to happen\n");
			if(horizontal)
			{
				rect.y += y - mouseY;	
				mouseY = y;
			} else {
				rect.x += x - mouseX;
				mouseX = x;
			}
			sync();
		} else {
			color.set(0.7,0.7,0.7,1.0);
		}
		return;
	}
	offHover();
}

void handleBar::offHover()
{
	if(mouseDown)
	{
		printf("Not supposed to happen\n");
		int x,y;
		SDL_GetMouseState(&x,&y);
		if(horizontal)
		{
			rect.y += y - mouseY;	
			mouseY = y;
		} else {
			rect.x += x - mouseX;
			mouseX = x;
		}
		sync();
	} else {
		color.set(0.0,0.0,0.0,1.0f);
	}
}

void handleBar::sync()
{
	if(horizontal){
		*v1 = rect.y;
		*v2 = rect.y;
	} else {
		*v1 = rect.x;
		*v2 = rect.x;
	}
	this->parent->UpdatePos();
}

void handleBar::Render(CVS_2DTools* tools)
{
	CVS_FRECT rectangle = tools->ConvertToScreenCoords(rect);
	tools->drawRect(color, rectangle.x, rectangle.y, rectangle.w, rectangle.h);
}

CVS_Gui_Cell::CVS_Gui_Cell(CVS_Gui_Cell* cell):bar(this),cell2(NULL),cell1(NULL),parent(cell),gui(NULL){
	this->rect = cell->rect;
}

CVS_Gui_Cell::CVS_Gui_Cell(CVS_Gui* gui):bar(this), cell1(NULL), cell2(NULL),gui(gui),parent(NULL){
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
		return true;
	}
	return false;
}

bool CVS_Gui_Cell::getMouseUp(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		bar.getMouseUp(x,y);
		for( int i = 0, e = children.size(); i < e; ++i)
		{
			if(children[i]->getMouseUp(x,y))
				return true;
		}
		return true;
	}
	return false;
}

void CVS_Gui_Cell::onHover(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		bar.onHover(x,y);
		for( int i = 0, e = children.size(); i < e; ++i)
		{
			children[i]->onHover(x,y);
		}

	}
	offHover();
}

void CVS_Gui_Cell::offHover()
{
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
	int offX = 0, offY = 0;
	for(int i = 0, e = children.size(); i < e; ++i)
	{
		if(children[i]->posType == CVS_POS_RELATIVE)
		{
			children[i]->rect.x = offX;
			children[i]->rect.y = offY;

			if( i > 0 && children[i]->rect.x + children[i]->rect.w > rect.x + rect.w)
			{
				children[i]->rect.y = children[i-1]->rect.h;
				children[i]->rect.x = rect.x;
			}

			offX = children[i]->rect.w + children[i]->rect.x;
			offY = children[i]->rect.h + children[i]->rect.y;
		}
	}
}

CVS_Gui::CVS_Gui(CVS_Window* window):window(window),mainCell(this)
{
	mainCell.rect.h = window->height;
	mainCell.rect.w = window->width;
	mainCell.rect.x = 0;
	mainCell.rect.y = 0;
}

CVSButtonHandle CVS_Gui::addButton(int x, int y, int w, int h, void (function)(void* bundle), void* bundle)
{
	this->buttons.push_back(new CVS_Button(x, y, w, h));
	((CVS_Button*)buttons.back())->setOnClickFunction(function, bundle);
	return buttons.size()-1;
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