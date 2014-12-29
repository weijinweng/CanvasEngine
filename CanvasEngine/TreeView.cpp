#include "TreeView.h"

using namespace Canvas;

const TreeNodeHandle Canvas::TREEVIEW_ROOT(0);
const TreeNodeHandle Canvas::TREEVIEW_NULL(0);

TreeNodeHandle::TreeNodeHandle()
{
	loc = 0;
}

TreeNodeHandle::TreeNodeHandle(int v)
{
	loc = v;
}

void TreeNodeHandle::Close()
{
	parent->treenodes[loc].Close();
}

void TreeNodeHandle::setParent(TreeView* parent)
{
	this->parent = parent;
}

TreeViewImageList::TreeViewImageList(std::string collapse, std::string expand) : m_collapsible(collapse), m_expandable(expand)
{}

int TreeViewImageList::addImage(std::string img)
{
	this->list.push_back(img);
	return list.size() - 1;
}

TreeNode::TreeNode() :m_open(false), m_selected(false)
{

}

I_Rect TreeNodeHandle::measureItem(int dx, int dy)
{
	return parent->treenodes[loc].measureItem(dx, dy);
}

I_Rect TreeNode::measureItem(int dx, int dy)
{
	rect.x = dx;
	rect.y = dy;
	rect.h = 17;

	int x = dx + 30;
	int y = dy + rect.h;

	if (m_open)
	{
		for (int i = 0, e = children.size(); i < e; ++i)
		{
			I_Rect irect = children[i].measureItem(x, y);
			x = irect.x;
			y = irect.y;
		}
	}

	I_Rect ret;
	ret.x = dx;
	ret.y = y;
	return ret; 
}

void TreeNode::Close()
{
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	for (int i = 0, e = children.size(); i < e; ++i)
	{
		children[i].Close();
	}
}

void TreeNodeHandle::addChild(TreeNodeHandle prev, TreeNodeHandle node)
{
	printf("%d\n", loc);
	parent->treenodes[loc].children.push_back(node);
}

TreeView::TreeView(int x, int y, int w, int h, uint32 style, Window* parent) :GUI_Entity(x, y, w, h, CVS_WS_CHILD | CVS_WS_CLIP_SIBLINGS | CVS_WS_VISIBLE, parent)
, m_update(false)
{
	this->treenodes.push_back(TreeNode());
}

void TreeView::Draw()
{
	if (!m_update)
	{
		measurePos();
		m_update = true;
	}
	DisplayContext context = getPaintContext();
	context.Init();
	context.SetFont(GUI_DefaultFont);
	context.SetFontColor(White);
	context.SetFillColor(GUI_Primary);
	I_Rect irect = context.getDisplayIRect();

	int width = getWidth();
	int dy = 0;

	context.DrawFillRect(irect.x, irect.y, irect.w, irect.h);

	for (int i = 1, e = treenodes.size(); i < e; ++i)
	{
		I_Rect noderect = treenodes[i].rect;
		if (!noderect.h)
			continue;
		noderect.w = width - noderect.x;

		if (treenodes[i].m_selected)
		{
			context.SetFillColor(CVS_Blue);
		}
		else {
			context.SetFillColor(GUI_Primary);
		}
	
		context.DrawFillRect(0, noderect.y, width, noderect.h);
		
		if (treenodes[i].children.size())
		{
			if (treenodes[i].m_open)
				context.TransparentBlit(m_Collapsible, noderect.x, noderect.y, 17, 17, Color_RGB(17, 168, 171));
			else {
				context.TransparentBlit(m_Expandable, noderect.x, noderect.y, 17, 17, Color_RGB(17, 168, 171));
			}
		}

		context.TransparentBlit(images[treenodes[i].image], noderect.x + 17, noderect.y, 17, 17, Color_RGB(17, 168, 171));
		context.DrawLeftOrientText(treenodes[i].text, noderect.x + 40, noderect.y, noderect.w, noderect.h);
	}
}

void TreeView::SetImageList(TreeViewImageList imagelist)
{
	m_Collapsible.Load(imagelist.m_collapsible);
	m_Expandable.Load(imagelist.m_expandable);


	for (int i = 0, e = imagelist.list.size(); i < e; ++i)
	{
		images.push_back(Bitmap(imagelist.list[i]));
	}
}

int TreeView::parseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
						   int mX = GET_X_LPARAM(lParam);
						   int mY = GET_Y_LPARAM(lParam);

						   for (int i = 1, e = treenodes.size(); i < e; ++i)
						   {
							   if (mY < treenodes[i].rect.y ||
								   mY > treenodes[i].rect.y + treenodes[i].rect.h)
							   {
							   }
							   else {

								   if (!treenodes[i].m_open)
								   {
									   treenodes[i].m_selected = true;
									   treenodes[i].m_open = treenodes[i].children.size();
									   for (int j = 0, jt = treenodes.size(); j < jt; ++j)
									   {
										   if (i != j)
										   treenodes[j].m_selected = false;
									   }
									   measurePos();
									   RedrawWindow(getHWND(), NULL, NULL, RDW_INVALIDATE);
								   }
								   else if(treenodes[i].children.size()){
									   treenodes[i].m_selected = true;
									   treenodes[i].m_open = false;
									   for (int j = 0, jt = treenodes.size(); j < jt; ++j)
									   {
										   if (i != j)
										   treenodes[j].m_selected = false;
									   }
									   treenodes[i].Close();
									   measurePos();
									   RedrawWindow(getHWND(), NULL, NULL, RDW_INVALIDATE);
								   }

							   }
						   }

	}
	case WM_PAINT:
	{
					 Draw();
	}
		return 0;
	}
	return -1;
}

TreeNodeHandle TreeView::Insert(TreeNodeHandle parent, TreeNodeHandle after, std::string text, void* data, uint32 msg, int image)
{
	if (!parent.loc)
	{
		parent.setParent(this);
	}
	if (!after.loc)
	{
		after.setParent(this);
	}

	TreeNode node;
	node.image = image;
	node.parent = parent;
	node.treeviewparent = this;
	node.data = data;
	node.msg = msg;
	node.text = text;

	this->treenodes.push_back(node);

	TreeNodeHandle handle;
	handle.loc = treenodes.size() - 1;
	handle.parent = this;
	
	printf("the tits %d %d\n", treenodes[0].image, image);

	parent.addChild(after, handle);
	m_update = false;
	return handle;
}

void TreeView::measurePos()
{
	int dx = 0;
	int dy = 0;
	for (int i = 0, e = treenodes[0].children.size(); i < e; ++i)
	{
		I_Rect rect = treenodes[0].children[i].measureItem(dx, dy);
		dx = rect.x;
		dy = rect.y;
	}
}