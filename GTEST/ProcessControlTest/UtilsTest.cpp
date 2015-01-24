#include "stdafx.h"
#include "Utils.h"
#include "MockWin32.h"

using namespace ::testing;

namespace UtilsTest {

	class Testee : public AutoFileHandle {
	public:
		Testee(HANDLE hHandle = INVALID_HANDLE_VALUE) : AutoFileHandle(hHandle) {};
	};

	class AutoFileHandleTest : public Test {
	public:
		MockWin32 mockWin32;
	};

	TEST_F(AutoFileHandleTest, empty)
	{
		INSTALL_CloseHandle_HOOK;

		static const HANDLE h = (HANDLE)10;
		EXPECT_CALL(mockWin32, CloseHandle(h)).Times(0);
		{
			Testee testee;
			ASSERT_FALSE(testee.isValid());
			ASSERT_EQ(INVALID_HANDLE_VALUE, testee);
		}
	}

	TEST_F(AutoFileHandleTest, normal)
	{
		INSTALL_CloseHandle_HOOK;
		static const HANDLE h = (HANDLE)10;
		EXPECT_CALL(mockWin32, CloseHandle(h)).WillOnce(Return(TRUE));

		{
			Testee testee;
			ASSERT_FALSE(testee.isValid());
			ASSERT_EQ(INVALID_HANDLE_VALUE, testee);
			ASSERT_EQ(h, testee = h);
			ASSERT_EQ(h, testee);
			ASSERT_TRUE(testee.isValid());
		}
	}

} // namespace UtilsTest
