#ifndef __Logger_h__
#define __Logger_h__
#include "MultiSys.h"
#include "Singleton.h"
#include "ILogger.h"
#include "LogFile.h"
#include "CircularQueue.h"
#include <thread>
#include <mutex>
#include <list>
#include <map>


class Logger   : public ILogger, public Singleton<Logger>
{
	enum
	{
		LOG_NODE_COUNT = 1024,
		TIME_OUT_FOR_CUT_FILE = 7200 * 1000,
		FLUSH_ASYNC_BATCH = 512,
		COMPUT_TIME_BATCH_COUNT = 200,
		DELAY_FLUSH_COUNT = 2,
		SLEEP_TIME = 100,
    };

	struct LogConfig 
	{
		std::string path;
		s32 splitSize;
		s32 splitTime;
	};

	typedef std::list<LogNode*> LogList;
	struct LogListThreadData
	{
		LogList threadA;
		LogList threadB;
		LogList	swap;
		std::mutex	mutex;
	};

	struct LogChannelInfo 
	{
		LogList threadA;
		LogList threadB;
		LogList	swap;
		std::mutex	mutex;
		std::map<std::string, LogFile *> logMap;
		LogFile *mainLog;
		std::string suffixName;
		s64 flushTime{0};
	};


public:
    virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();

    virtual void SyncLog(const char *contents);
    virtual void AsyncLog(const char *fileName, const char *content);
	virtual void ThreadLog(const char *fileName, const char *content);
	virtual void Process(s32 tick);

public:
	inline s32	LogLevel() { return _logLevel; };

private:
    void ThreadRun();

private:
	bool LoadConfig();
	void InitLogChannel(LogChannelInfo &channelInfo, const std::string &suffixName, s64 nowTime);
	const char *GetLogTimeString();
	bool CreateLogFile(LogFile &logFile);
	bool WriteLogNode(LogFile &logFile, const LogNode *logNode);
	void OnCheckChannel(s64 nowTime, LogChannelInfo &channelInfo);
	bool DealLogChannelInfo(s64 nowTime, LogChannelInfo &channelInfo);
	void DealChannleLog(s64 nowTime, LogChannelInfo &channelInfo, const LogNode *logNode);

private:

    tlib::TString<MAX_PATH>				_logPath;
	tlib::TString<MAX_PATH>				_logPrefix;
	LogConfig							_logConfig;
	LogFile								_syncFile{"", "sync"};

    std::thread                         _thread;
    bool                                _terminate;
	LogChannelInfo						_asyncChannel;
	LogChannelInfo						_threadChannel;
	std::mutex							_threadLogMutex;
	s32									_logLevel;
};

#define LOGGER  (Logger::GetInstancePtr())

#endif