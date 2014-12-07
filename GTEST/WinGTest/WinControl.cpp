#include "stdafx.h"
#include "WinControl.h"

CWinControl* CWinControl::create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR className, DWORD style, LPCTSTR title, const SIZE& size, const POINT& pos)
{
	m_hInst = hInst;
	m_hParentWnd = hParentWnd;
	m_hWnd = ::CreateWindow(className, title, style | commonStyle,
						pos.x, pos.y, size.cx, size.cy,
						hParentWnd, (HMENU)id, hInst, NULL);
	DWORD error = ::GetLastError();
	_ASSERT(m_hWnd);

	return this;
}

/**
 * constructor for tree item except for root item
 */
CTreeViewItem::CTreeViewItem(LPCTSTR title, CTreeViewItem* pParent/* = NULL*/)
	: m_title(title), m_pParent(pParent)
{
	if(!pParent) {
		m_fullName = title;
		m_hTreeViewWnd = NULL;		// We will set in create() method for root item.
	} else {
		m_fullName = pParent->m_fullName + _T(".") + title;
		m_hTreeViewWnd = pParent->m_hTreeViewWnd;
	}
}

void CTreeViewItem::createRoot(LPCTSTR title, HWND hTreeViewWnd)
{
	m_hTreeViewWnd = hTreeViewWnd;
	create(title);
}

/**
 * create tree view item control and add to the tree view
 *
 * @param title
 * @param hParentItem set NULL to create root item
 */
void CTreeViewItem::create(LPCTSTR title, HTREEITEM hParentItem/* = NULL*/)
{
	HTREEITEM hInsertAfter = !hParentItem ? TVI_ROOT : TVI_LAST;
	TVINSERTSTRUCT ins = { hParentItem, hInsertAfter, { TVIF_TEXT | TVIF_PARAM } };
	ins.itemex.pszText = (LPTSTR)m_title.c_str();
	ins.itemex.lParam = (LPARAM)this;

	m_hTreeItem = TreeView_InsertItem(m_hTreeViewWnd, &ins);
	_ASSERT(m_hTreeItem);
}

/**
 * Add tree view item as a child of this.
 *
 *  - Create CTreeViewItem object.
 *  - Create tree view item and add it to owner tree view control.
 *  - Associate the tree view item with the object.
 */
CTreeViewItem* CTreeViewItem::addChild(LPCTSTR title)
{
	m_Childs.push_back(CTreeViewItem(title, this));
	CTreeViewItem& child = *m_Childs.rbegin();

	child.create(title, m_hTreeItem);

	return &child;
}

CWinControl* CTreeViewControl::create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR rootItemTitle, const SIZE& size, const POINT& pos)
{
	CTreeViewControl* pThis = new CTreeViewControl(rootItemTitle);
	static const DWORD style = WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	pThis->CWinControl::create(hInst, hParentWnd, id, WC_TREEVIEW, style, NULL, size, pos);

	pThis->m_rootItem.createRoot(rootItemTitle, pThis->m_hWnd);

	CTreeViewItem* p = pThis->m_rootItem.addChild(_T("X"));
	p = p->addChild(_T("X1"));
	p = p->addChild(_T("X2"));
	pThis->m_rootItem.addChild(_T("Y"));
	return pThis;
}

LRESULT CTreeViewControl::onNotify(NMHDR* nmhdr)
{
	switch(nmhdr->code) {
	case TVN_SELCHANGED:
		{
			TVITEM& item = ((LPNMTREEVIEW)nmhdr)->itemNew;
			CTreeViewItem* pItem = (CTreeViewItem*)item.lParam;
			pItem->getFullName();
		}
		break;
	}
	return 0L;
}

CWinControl* CEditControl::create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos)
{
	CEditControl* pThis = new CEditControl();
	static const DWORD style = WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL;
	pThis->CWinControl::create(hInst, hParentWnd, id, WC_EDIT, style, title, size, pos);
	return pThis;
}

CWinControl* CButtonControl::create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos)
{
	CButtonControl* pThis = new CButtonControl();
	pThis->CWinControl::create(hInst, hParentWnd, id, WC_BUTTON, 0, title, size, pos);
	return pThis;
}
