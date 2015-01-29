
class IWin32 {
public:
	virtual BOOL CloseHandle(_In_ HANDLE hObject) = 0;
};

class MockWin32 : public IWin32 {
public:
	static MockWin32* instance;
	MockWin32() { instance = this; };

	MOCK_METHOD1(CloseHandle, BOOL(_In_ HANDLE hObject));
};

// about SEXYHOOK2, see https://prezi.com/aigm45d07lrp/sexyhook2/

#define INSTALL_CloseHandle_HOOK \
	SEXYHOOK_BEGIN(BOOL, SEXYHOOK_STDCALL, &CloseHandle, (HANDLE hObject)) \
	{ \
		return MockWin32::instance->CloseHandle(hObject); \
	} SEXYHOOK_END()
