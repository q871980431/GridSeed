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
	//printf("���Ǻ�ɫ\n");
	////system("color 14");
	//SetConsoleTextAttribute(handle, FOREGROUND_RED);
	//printf("���Ǻ�ɫ2\n");
	//SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED);
	//printf("���Ǻ�ɫ222222\n");
	//SetConsoleTextAttribute(handle, FOREGROUND_RED | BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_RED);
	//printf("���Ǻ�ɫ3\n");
	//SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
	//printf("���ǻ�ɫ\n");
	//SetConsoleTextAttribute(handle, 0x0f);
	//printf("����ȫ������\n");
	//SetConsoleTextAttribute(handle, 0x07);
	//printf("����ȫ������\n");
}
