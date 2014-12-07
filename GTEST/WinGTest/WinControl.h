#pragma once

#include <CommCtrl.h>
#include <vector>

class CWinControl
{
public:
	typedef CWinControl* (*factory_t)(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos);

	virtual ~CWinControl() {};

	inline bool operator==(HWND hWnd) const { return hWnd == m_hWnd; };
	virtual LRESULT onNotify(NMHDR* nmhdr) { return 0L; };

protected:
	static const DWORD commonStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
	CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR className, DWORD style, LPCTSTR title, const SIZE& size, const POINT& pos);

	CWinControl() {};

	HINSTANCE m_hInst;
	HWND m_hParentWnd;
	HWND m_hWnd;
};

class CTreeViewControl;

class CTreeViewItem {
	friend class CTreeViewControl;

public:
	CTreeViewItem(LPCTSTR title, CTreeViewItem* pParent = NULL);
	~CTreeViewItem() {};

	const tstring& getTitle() const { return m_title; };
	const tstring& getFullName() const { return m_fullName; };

	CTreeViewItem* addChild(LPCTSTR title);

protected:
	void createRoot(LPCTSTR title, HWND hTreeViewWnd);
	void create(LPCTSTR title, HTREEITEM hParentItem = NULL);

	HWND m_hTreeViewWnd;
	HTREEITEM m_hTreeItem;
	tstring m_title;
	tstring m_fullName;
	CTreeViewItem* m_pParent;
	std::vector<CTreeViewItem> m_Childs;
};

class CTreeViewControl : public CWinControl {
public:
	static CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR rootItemTitle, const SIZE& size, const POINT& pos);

	virtual LRESULT onNotify(NMHDR* nmhdr);

protected:
	CTreeViewControl(LPCTSTR rootItemTitle) : CWinControl(), m_rootItem(rootItemTitle) {};

	CTreeViewItem m_rootItem;
};

class CEditControl : public CWinControl {
public:
	static CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos);

protected:
	CEditControl() : CWinControl() {};
};

class CButtonControl : public CWinControl {
public:
	static CWinControl* create(HINSTANCE hInst, HWND hParentWnd, int id, LPCTSTR title, const SIZE& size, const POINT& pos);

protected:
	CButtonControl() : CWinControl() {};
	virtual DWORD style() const { return 0; };
};
