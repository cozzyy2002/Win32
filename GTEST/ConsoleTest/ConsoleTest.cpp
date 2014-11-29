// ConsoleTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string.h>

using namespace testing;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	UnitTest* unitTest = UnitTest::GetInstance();
	for(int i = 0; i < unitTest->total_test_case_count(); i++) {
		const TestCase* testCase = unitTest->GetTestCase(i);
		cout << testCase->name() << endl;
		for(int j = 0; j < testCase->total_test_count(); j++) {
			const TestInfo* testInfo = testCase->GetTestInfo(j);
			cout << "  " << testInfo->name() << endl;
		}
	}

	InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	return 0;
}
