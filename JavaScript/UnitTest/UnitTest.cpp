// UnitTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <gtest/gtest.h>

#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Dispatcher"));

int _tmain(int argc, _TCHAR* argv[])
{
	log4cplus::initialize();
	log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

	LOG4CPLUS_INFO(logger, "start test");
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

