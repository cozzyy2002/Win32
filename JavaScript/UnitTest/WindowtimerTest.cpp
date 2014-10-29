#include "stdafx.h"
#include <gtest/gtest.h>

#include <WindowTimer.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Test.WindowTimerTest"));

class Timeout {
public:
	Timeout() : calledMethod(NULL), calledCount(0) {};

	void methodP0() {
		tickCount = ::GetTickCount();
		calledMethod = "methodP0";
		calledCount++;
	};
	int methodP1(int i) {
		calledMethod = "methodP1";
		calledCount++;
		return i;
	};

	DWORD tickCount;
	char* calledMethod;
	int calledCount;
};

class WindowTimerTest : public ::testing::Test {
public:
	Timeout testTimeout;
};

TEST_F(WindowTimerTest, method_with_no_param) {

	DWORD start = ::GetTickCount();
	HANDLE timerId = setTimeout(&testTimeout, &Timeout::methodP0, 1000);

	ASSERT_TRUE(joinTimeout(timerId)) << "join()";
	ASSERT_NEAR(1000, testTimeout.tickCount - start, 50) << "tick count";

	EXPECT_STREQ("methodP0", testTimeout.calledMethod) << "called method";
	EXPECT_EQ(1, testTimeout.calledCount) << "called count";
}
