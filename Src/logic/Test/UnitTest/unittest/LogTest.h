#ifndef __LogTest_h__
#define __LogTest_h__
#include "IUnitTest.h"
class LogTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);

protected:
private:
	void winConselColor(core::IKernel *kernel);
};
#endif
