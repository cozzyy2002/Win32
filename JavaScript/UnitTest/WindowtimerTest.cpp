#include "stdafx.h"
#include <gtest/gtest.h>

#include <WindowTimer.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Test.WindowTimerTest"));

// parameter that will be passed to timeout method
static struct P1 {
	int x;
	char s[10];
} p1 = { 12, "a string" };

class Timeout {
public:
	Timeout() : calledMethod(NULL), calledCount(0) {};

	void methodP0() {
		saveCalledContext("methodP0");
	};
	void methodP1(const P1& i) {
		p1 = i;
		saveCalledContext("methodP1");
	};

	void saveCalledContext(char* calledMethod) {
		this->tickCount = ::GetTickCount();
		this->calledMethod = calledMethod;
		this->calledCount++;
	};

	DWORD tickCount;
	char* calledMethod;
	int calledCount;
	P1 p1;
};

class WindowTimerTest : public ::testing::Test {
public:
	Timeout testTimeout;
};

TEST_F(WindowTimerTest, set_join_P0) {

	DWORD start = ::GetTickCount();
	HANDLE timerId = setTimeout(&testTimeout, &Timeout::methodP0, 1000);

	ASSERT_TRUE(joinTimeout(timerId)) << "join()";
	ASSERT_NEAR(1000, testTimeout.tickCount - start, 50) << "tick count";

	EXPECT_STREQ("methodP0", testTimeout.calledMethod) << "called method";
	EXPECT_EQ(1, testTimeout.calledCount) << "called count";
}

TEST_F(WindowTimerTest, set_join_limit_P0) {

	DWORD start = ::GetTickCount();
	HANDLE timerId = setTimeout(&testTimeout, &Timeout::methodP0, 1000);

	ASSERT_FALSE(joinTimeout(timerId, 800)) << "join()";

	clearTimeout(timerId);
}

TEST_F(WindowTimerTest, set_clear_P0) {

	DWORD start = ::GetTickCount();
	HANDLE timerId = setTimeout(&testTimeout, &Timeout::methodP0, 1000);

	ASSERT_TRUE(clearTimeout(timerId)) << "clear";
	ASSERT_TRUE(joinTimeout(timerId)) << "join()";

	EXPECT_EQ(0, testTimeout.calledCount) << "called count";
}

TEST_F(WindowTimerTest, set_join_P1) {

	DWORD start = ::GetTickCount();
	HANDLE timerId = setTimeout(&testTimeout, &Timeout::methodP1, p1, 1000);

	ASSERT_TRUE(joinTimeout(timerId)) << "join()";
	ASSERT_NEAR(1000, testTimeout.tickCount - start, 50) << "tick count";

	EXPECT_EQ(p1.x, testTimeout.p1.x) << "int parameter to timeout";
	EXPECT_STREQ(p1.s, testTimeout.p1.s) << "string parameter to timeout";
	EXPECT_STREQ("methodP1", testTimeout.calledMethod) << "called method";
	EXPECT_EQ(1, testTimeout.calledCount) << "called count";
}
