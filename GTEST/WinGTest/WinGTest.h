#pragma once

#include "resource.h"
#include "WinControl.h"

#include <map>

class WinGtest
{
public:
	WinGtest(void) {};
	virtual ~WinGtest(void) {};

	static WinGtest* getInstance(HWND hwnd);
	static BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	LRESULT onNotify(HWND hwnd, int /* not used */, NMHDR* nmhdr);
	void onPaint(HWND hwnd);
	void onDestroy(HWND hwnd);
	void onNCDestroy(HWND hwnd);

protected:
	bool createControls();

	typedef std::map<int, CWinControl*> controls_t;
	controls_t m_controls;
	CTreeViewControl* m_pTestList;
	CEditControl* m_pNote;

	HINSTANCE m_hInst;
	HWND m_hWnd;
};
