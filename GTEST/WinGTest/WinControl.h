#pragma once

#include <CommCtrl.h>
#include <vector>

class CWinControl
{
public:
	typedef CWinControl* (*factory_t)(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos);

protected:
	static const DWORD commonStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
	CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR className, DWORD style, LPCTSTR title, const SIZE& size, const POINT& pos);

	CWinControl() {};
	virtual ~CWinControl() {};

	HINSTANCE m_hInst;
	HWND m_hParentWnd;
	HWND m_hWnd;
};

class CTreeViewItem {
public:
	CTreeViewItem(LPCTSTR title, CTreeViewItem* pParent = NULL);
	~CTreeViewItem() {};

	const tstring& getTitle() const { return m_title; };
	const tstring& getFullName() const { return m_fullName; };

	CTreeViewItem* addChild(LPCTSTR title);

protected:
	tstring m_title;
	tstring m_fullName;
	CTreeViewItem* m_pParent;
	std::vector<CTreeViewItem> m_Childs;
};

class CTreeViewControl : public CWinControl {
public:
	static CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR rootItemTitle, const SIZE& size, const POINT& pos);
	virtual ~CTreeViewControl() {};

protected:
	CTreeViewControl(LPCTSTR rootItemTitle) : CWinControl(), m_rootItem(rootItemTitle) {};

	CTreeViewItem m_rootItem;
};

class CEditControl : public CWinControl {
public:
	static CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos);
	virtual ~CEditControl() {};

protected:
	CEditControl() : CWinControl() {};
};

class CButtonControl : public CWinControl {
public:
	static CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos);
	virtual ~CButtonControl() {};

protected:
	CButtonControl() : CWinControl() {};
	virtual DWORD style() const { return 0; };
};
