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

CTreeViewItem::CTreeViewItem(LPCTSTR title, CTreeViewItem* pParent/* = NULL*/)
	: m_title(title), m_pParent(pParent)
{
	m_fullName = (pParent ? pParent->m_fullName : tstring()) + _T(".") + title;
}

CTreeViewItem* CTreeViewItem::addChild(LPCTSTR title)
{
	CTreeViewItem* pChild = new CTreeViewItem(title, this);
	m_Childs.push_back(CTreeViewItem(title, this));
	return &(*m_Childs.rbegin());
}

CWinControl* CTreeViewControl::create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR rootItemTitle, const SIZE& size, const POINT& pos)
{
	CTreeViewControl* pThis = new CTreeViewControl(rootItemTitle);
	static const DWORD style = WS_BORDER;
	pThis->CWinControl::create(hInst, hParentWnd, id, WC_TREEVIEW, style, NULL, size, pos);
	return pThis;
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
