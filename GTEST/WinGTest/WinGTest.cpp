// WinGTest.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "WinGTest.h"
#include "WinControl.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINGTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINGTEST));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINGTEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINGTEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, new WinGtest());

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WinGtest* pThis = WinGtest::getInstance(hwnd);
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CREATE, WinGtest::onCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, pThis->onCommand);
		HANDLE_MSG(hwnd, WM_PAINT, pThis->onPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, pThis->onDestroy);
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

WinGtest* WinGtest::getInstance(HWND hwnd)
{
	return (WinGtest*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

BOOL WinGtest::onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	WinGtest* pThis = (WinGtest*)lpCreateStruct->lpCreateParams;
	::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

	pThis->m_hInst = lpCreateStruct->hInstance;
	pThis->m_hWnd = hwnd;

	return pThis->createControls() ? TRUE : FALSE;
}

typedef enum _CONTROL_ID {
	CONTROL_ID_ = 200,
	CONTROL_ID_TEST_LIST,
	CONTROL_ID_NOTE,
	CONTROL_ID_OK,
	CONTROL_ID_NG,
	CONTROL_ID_ABORT,
} CONTROL_ID;

typedef struct _ControlData {
	CWinControl::factory_t factory;
	CONTROL_ID id;
	LPCTSTR title;
	SIZE size;
	POINT pos;
	CWinControl** ppControl;
} ControlData;

static const int marginX = 2;
static const int marginY = 2;
static const DWORD controlStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD;

static const DWORD testListStyle = controlStyle | WS_BORDER | LVS_REPORT;
static const SIZE testListSize = { 600, 160 };

static const DWORD noteStyle = controlStyle | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL;
static const SIZE noteSize = { testListSize.cx, 80 };

static const DWORD buttonStyle = controlStyle;
static const SIZE buttonSize = { 80, 20 };

static CTreeViewControl* pTestList = NULL;
static CEditControl* pNote;

static const ControlData controlDatas[] = {
	{ CTreeViewControl::create, (CONTROL_ID)0, _T("All"), testListSize, { 0, 0 }, (CWinControl**)&pTestList },
	{ CEditControl::create, (CONTROL_ID)0, _T("enter note in this edit box"), noteSize, { 0, testListSize.cy }, (CWinControl**)&pNote },
	{ CButtonControl::create, CONTROL_ID_OK, _T("OK"), buttonSize, { 360, 240 } },
	{ CButtonControl::create, CONTROL_ID_NG, _T("NG"), buttonSize, { 440, 240 } },
	{ CButtonControl::create, CONTROL_ID_ABORT, _T("Abort"), buttonSize, { 520, 240 } },
};

bool WinGtest::createControls()
{
	for(int i = 0; i < ARRAYSIZE(controlDatas); i++) {
		const ControlData& cd = controlDatas[i];
		SIZE size = { cd.size.cx - marginX, cd.size.cy - marginY };
		POINT pos = { cd.pos.x + (marginX/2), cd.pos.y + (marginY/2) };
		CWinControl* pControl = cd.factory(m_hInst, m_hWnd, cd.id, cd.title, size, pos);
		if(cd.ppControl) *cd.ppControl = pControl;
	}

	return true;
}

void WinGtest::onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// �I�����ꂽ���j���[�̉��:
	switch (id)
	{
	case CONTROL_ID_TEST_LIST:
	case CONTROL_ID_NOTE:
	case CONTROL_ID_OK:
	case CONTROL_ID_NG:
	case CONTROL_ID_ABORT:
		break;
	case IDM_ABOUT:
		DialogBox(m_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
	}
}

void WinGtest::onPaint(HWND hwnd)
{
	PAINTSTRUCT ps;

	HDC hdc = BeginPaint(hwnd, &ps);
	// TODO: �`��R�[�h�������ɒǉ����Ă�������...
	EndPaint(hwnd, &ps);
}

void WinGtest::onDestroy(HWND hwnd)
{
	PostQuitMessage(0);

	delete this;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
