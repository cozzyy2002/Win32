
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

#define INSTALL_CloseHandle_HOOK \
	SEXYHOOK_BEGIN(BOOL, SEXYHOOK_STDCALL, &CloseHandle, (HANDLE a1)) \
	{ \
		return MockWin32::instance->CloseHandle(a1); \
	} SEXYHOOK_END()
