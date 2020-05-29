#include "AsyncThread.h"
#include "Tools.h"
#include "Tools_time.h"
#include "TList.h"
#include "../Exception/ExceptionMgr.h"
#include <mutex>


AsyncThread::AsyncThread(s32 ququeId, s32 threadIdx) :_terminate(false) {

	tools::Zero(_readyExec.main);
	tools::Zero(_readyExec.swap);
	tools::Zero(_readyExec.work);
	tools::Zero(_complete.main);
	tools::Zero(_complete.swap);
	tools::Zero(_complete.work);
	tools::Zero(_status);

	_status.queueId = ququeId;
	_status.threadIdx = threadIdx;
}

void AsyncThread::Start() {
	_thread = std::thread(&AsyncThread::ThreadProc, this);
}

void AsyncThread::Terminate() {
	_terminate = true;
	_thread.join();
}

void AsyncThread::Add(AsyncBase * base) {
	_status.addCount++;
	tlib::linear::PushTail(_readyExec.main, base);
}

void AsyncThread::Loop(s64 overtime) {
	tlib::linear::MergeListByLock(_readyExec.swap, _readyExec.main, _readyExec.lock);
	tlib::linear::MergeListByLock(_complete.main, _complete.swap, _complete.lock);
	s64 tick = tools::GetTimeMillisecond();
	AsyncBase * base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_complete.main);
	while (base)
	{
		TRY_BEGIN
		_status.completeCount++;
		base->OnComplete();
		TRY_END
		TRY_BEGIN
		_status.releaseCount++;
		base->Release();
		TRY_END
		if (tools::GetTimeMillisecond() - tick >= overtime)
			break;
		base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_complete.main);
	}
}

void AsyncThread::ThreadProc() {
	while (!_terminate) {
		AsyncBase * base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_readyExec.work);
		do 
		{
			if (!base) {
				tlib::linear::MergeListByLock(_readyExec.work, _readyExec.swap, _readyExec.lock);
				base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_readyExec.work);
			}

			if (base) {
				TRY_BEGIN
					_status.execCount++;
					base->OnExecute(_status.queueId, _status.threadIdx);
				TRY_END
					tlib::linear::PushTail(_complete.work, base);
				base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_readyExec.work);
			}
		} while (base);

		if (_complete.swap.head == nullptr && _complete.work.head != nullptr)
			tlib::linear::MergeListByLock(_complete.swap, _complete.work, _complete.lock);
		else
			MSLEEP(10);
	}
}
