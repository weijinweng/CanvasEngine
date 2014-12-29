#include "LayOut.h"

using namespace Canvas;

LayoutRelation::LayoutRelation(GUI_Entity* obj1, GUI_Entity* obj2, uint32 type, int value) :obj1(obj1), obj2(obj2), type(type), value(value)
{
}

void LayoutRelation::UpdateValue()
{
	I_Rect child_rect = obj1->getIRectRelative();
	I_Rect parent_rect = obj2 ? obj2->getIRectRelative() : obj1->getRootWindow()->getClientIRect();
	if (!obj2)
	{
		parent_rect.x = 0;
		parent_rect.y = 0;
	}


	if (type & CVS_RIGHT_BOUND)
	{
		child_rect.x = parent_rect.x + parent_rect.w;
	}
	if (type & CVS_TOP_BOUND)
	{
		child_rect.y = parent_rect.y + parent_rect.h;
	}
	if (type & CVS_LEFT_BOUND)
	{
		child_rect.w = parent_rect.x - child_rect.x;
	}
	if (type & CVS_BOT_BOUND)
	{
		child_rect.h = parent_rect.y - child_rect.y;
	}
	if (type & CVS_FILL_PARENT)
	{
		child_rect = parent_rect;
	}
	if (type & CVS_LEFT_LEFT)
	{
		child_rect.x = parent_rect.x + parent_rect.w - child_rect.w;
	}
	if (type & CVS_RIGHT_RIGHT)
	{
		child_rect.x = parent_rect.x;
	}
	if (type & CVS_TOP_TOP)
	{
		child_rect.y = parent_rect.y;
	}
	if (type & CVS_BOT_BOT)
	{
		child_rect.y = parent_rect.h + parent_rect.y - child_rect.h;
	}
	if (type & CVS_P_LEFT_OFFSET)
	{
		child_rect.x = value;
	}
	if (type & CVS_P_TOP_OFFSET)
	{
		child_rect.y = value;
	}
	if (type & CVS_P_RIGHT_OFFSET)
	{
		child_rect.w = parent_rect.w - value - child_rect.x;
	}
	if (type & CVS_P_BOT_OFFSET)
	{
		child_rect.h = parent_rect.h - value - child_rect.y;
	}

	obj1->setWindowSize(child_rect.x, child_rect.y, child_rect.w, child_rect.h);
}

Layout::Layout(Window* parent) :GUI_Entity()
{
	cloneWindow(parent);
	parent->addChild(this);
}

int Layout::parseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case CVS_SIZE:
		Update();
		return 0;
	}
	return -1;
}

void Layout::Update()
{
	for (int i = 0, e = m_relations.size(); i < e; ++i)
	{
		m_relations[i].UpdateValue();
	}
}

void Layout::addConnection(LayoutRelation relation)
{
	this->m_relations.push_back(relation);
	relation.UpdateValue();
}

