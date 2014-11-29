#pragma once
class CGtestWindow
{
public:
	CGtestWindow(void);
	virtual ~CGtestWindow(void);

	static CGtestWindow* getInstance(LPARAM lParam);
	static BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);

};

