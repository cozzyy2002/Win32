#pragma once

#include "resource.h"

class WinGtest
{
public:
	WinGtest(void) {};
	virtual ~WinGtest(void) {};

	static WinGtest* getInstance(HWND hwnd);
	static BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void onPaint(HWND hwnd);
	void onDestroy(HWND hwnd);

protected:
	bool createControls();

	HINSTANCE m_hInst;
	HWND m_hWnd;
};
