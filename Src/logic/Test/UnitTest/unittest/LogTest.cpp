#include "LogTest.h"
#include "../UnitTest.h"

UnitTestRegister<LogTest> test;
void LogTest::StartTest(core::IKernel *kernel)
{
	winConselColor(kernel);
	//TestAsyncQueue(kernel);
}

void LogTest::winConselColor(core::IKernel *kernel)
{
	//HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	////system("color 1A");
	//SetConsoleTextAttribute(handle, 0x54);
	//printf("这是红色\n");
	////system("color 14");
	//SetConsoleTextAttribute(handle, FOREGROUND_RED);
	//printf("这是红色2\n");
	//SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED);
	//printf("这是红色222222\n");
	//SetConsoleTextAttribute(handle, FOREGROUND_RED | BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_RED);
	//printf("这是红色3\n");
	//SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
	//printf("这是灰色\n");
	//SetConsoleTextAttribute(handle, 0x0f);
	//printf("这是全部高亮\n");
	//SetConsoleTextAttribute(handle, 0x07);
	//printf("这是全部正常\n");
}
