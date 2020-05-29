#include "LogTest.h"
#include "../UnitTest.h"
#include "../Game_tools.h"

UnitTestRegister<LogTest> test;
void LogTest::StartTest(core::IKernel *kernel)
{
	winConselColor(kernel);
	//TestAsyncQueue(kernel);
}

void LogTest::winConselColor(core::IKernel *kernel)
{
	for (int32_t i = 0; i < 100; i++)
	{
		char buff[255];
		SafeSprintf(buff, sizeof(buff), "this is test :%d", i);
		FILE_LOG(core::LOG_LEVEL_DEBUG, "File1", "%s", buff);
	}
	for (int32_t i = 0; i < 100; i++)
	{
		char buff[255];
		SafeSprintf(buff, sizeof(buff), "this is test :%d", i);
		FILE_LOG(core::LOG_LEVEL_DEBUG, "File2", "%s", buff);
	}
	IMPORTANT_LOG("Test", "test :%d", 5);

	for (s32 i = 0; i < 100000; i++)
	{
		auto startFun = [i](core::IKernel *kernel, s64 tick)
		{
			TRACE_LOG("timer:%d, start fun", i);
		};
		auto timeFun = [i](core::IKernel *kernel, s64 tick)
		{
			for (int32_t j = 0; j < 100; j++)
			{
				char buff[255];
				SafeSprintf(buff, sizeof(buff), "this is test log split :[%d] -[%d]", i, j);
				FILE_LOG(core::LOG_LEVEL_DEBUG, "File2", "%s", buff);
			}
		};

		auto onTerminateFun = [i](core::IKernel *kernel, s64 tick, bool isKill)
		{
			TRACE_LOG("timer:%d, onTerminate fun, Kill:%d", i, isKill);
		};
		SimpleTimer *timer = NEW SimpleTimer(startFun, timeFun, onTerminateFun);
		ADD_TIMER(timer, (i % 50) * 100, 10, 1000);
	}
}
