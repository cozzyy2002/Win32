
#include "stdafx.h"
#include <gtest/gtest.h>

class MyTest1 : public testing::Test {
};

TEST_F(MyTest1, test1) {}
TEST_F(MyTest1, test2) {}
TEST_F(MyTest1, test3) {}
TEST_F(MyTest1, test4) {}

class MyTest2 : public testing::Test {
};

TEST_F(MyTest2, test1) {}
TEST_F(MyTest2, test2) {}
TEST_F(MyTest2, test3) {}
