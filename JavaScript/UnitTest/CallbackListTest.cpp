#include "stdafx.h"
#include <gtest/gtest.h>

#include <helpers/CallbackList.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Test.CallbackListTest"));

class Listener {
public:
	Listener() : calledMethod(NULL), calledCount(0) {};

	void methodP0() {
		calledMethod = "methodP0";
		calledCount++;
	};
	int methodP1(int i) {
		calledMethod = "methodP1";
		calledCount++;
		return i;
	};

	char* calledMethod;
	int calledCount;
};

class CallbackListTest : public ::testing::Test {
public:
	Listener testListener1;
	Listener testListener2;
};

TEST_F(CallbackListTest, method_with_no_param) {

	P0CallbackList<Listener> listeners;

	ASSERT_FALSE(listeners) << "before add listener";
	ASSERT_TRUE(listeners.addListener(&testListener1, &Listener::methodP0)) << "add listener 1";
	ASSERT_TRUE(listeners.addListener(&testListener2, &Listener::methodP0)) << "add listener 2";
	ASSERT_TRUE(listeners) << "after add listener 1 and 2";

	ASSERT_NO_THROW(listeners.forEach()) << "calling the method";
	EXPECT_STREQ("methodP0", testListener1.calledMethod) << "called method";
	EXPECT_EQ(1, testListener1.calledCount) << "listener 1 has been called once";
	EXPECT_EQ(1, testListener2.calledCount) << "listener 2 has been called once";

	ASSERT_TRUE(listeners.removeListener(&testListener2)) << "remove listener 2";
	ASSERT_FALSE(listeners.removeListener(&testListener2)) << "remove listener 2 again";
	ASSERT_TRUE(listeners) << "after remove lsitener 2";
	ASSERT_NO_THROW(listeners.forEach()) << "1 listener";
	EXPECT_EQ(2, testListener1.calledCount) << "listener 1 has been called twice";
	EXPECT_EQ(1, testListener2.calledCount) << "listener 2 has been called once";

	ASSERT_TRUE(listeners.removeListener(&testListener1)) << "remove listener 1";
	ASSERT_FALSE(listeners) << "after remove all lsiteners";
	ASSERT_NO_THROW(listeners.forEach()) << "no listener";
	EXPECT_EQ(2, testListener1.calledCount) << "listener 1 has been called twice";
	EXPECT_EQ(1, testListener2.calledCount) << "listener 2 has been called once";
}
