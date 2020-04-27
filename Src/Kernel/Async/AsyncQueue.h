#ifndef __AsyncQueue_H__
#define __AsyncQueue_H__
#include "IKernel.h"
#include <vector>
#include <string>

using namespace core;

class AsyncThread;
class AsyncQueue : public IAsyncQueue, public IModuleProfile {
public:
	AsyncQueue(s32 queueId, const char *trace) :_trace(trace), _queueId(queueId), _moduleName("AsyncQueue") {
		_moduleName.push_back('_');
		_moduleName.append(std::to_string(_queueId));
		_moduleName.push_back('_');
		_moduleName.append(trace);
	};

    bool Ready();
    bool Initialize(s32 threadCount);
    bool Destroy();
	void Loop(s64 overTime);

	virtual void StartAsync(const s64 threadId, IAsyncHandler * handler, const char * file, const s32 line);
	virtual void StopAsync(IAsyncHandler * handler);
	virtual s32  GetQueueId() { return _queueId; };
	virtual void  GetQueueInfo(s32 &threadNum, std::set<s32> &threadIds);

public:
	virtual const char * Name();
	virtual std::string ProfileInfo();

private:
	std::vector<AsyncThread*> _threads;
	std::string _trace;
	s32	_queueId;
	std::string _moduleName;
};

#endif //__ASYNCMGR_H__

