#include "Logger.h"
#include "Tools.h"
#include "Tools_time.h"
#include "../Kernel.h"
#include "Tools_time.h"
#include "XmlReader.h"

s32 CHANNEL_CHECK_FLUSH_TIME = 1000;
s32 FILE_SPLIT_SIZE = 1 * 1024 * 1024 * 1024;
s32 LOOP_WRIT_LOG_COUNT = 2 * 1024;

template<  > Logger * Singleton<Logger>::_instance = nullptr;
bool Logger::Ready()
{
    _terminate = false;

    return true;
}

bool Logger::Initialize()
{
	if (!LoadConfig())
		return false;
	_logPath << tools::GetAppPath() << DIR_DELIMITER << _logConfig.path.c_str();
	tools::Mkdir(_logPath.GetString());
	s64 now = tools::GetTimeMillisecond();
	const char *procName = Kernel::GetInstance().GetCmdArg("name");
	const char *procId = Kernel::GetInstance().GetCmdArg("id");
	_logPrefix << procName << LOG_CONNECT_SIGN << procId;
	CreateLogFile(_syncFile);
	InitLogChannel(_asyncChannel, "async", now);
	InitLogChannel(_threadChannel, "thread", now);
    _thread = std::thread(&Logger::ThreadRun, this);

    return true;
}

bool Logger::Destroy()
{
    if ( _thread.joinable())
    {
        _terminate = true;
        _thread.join();
    }
    _syncFile.Close();

    return true;
}

void Logger::SyncLog(const char *contents)
{
    _syncFile.Write(tools::GetCurrentTimeString());
    _syncFile.Write(" | ");
    _syncFile.Write(contents);
    _syncFile.Write("\n");
    _syncFile.Flush();
}

void Logger::AsyncLog(const char *fileName, const char *content)
{
    LogNode *logNode = (LogNode*)NEW LogNode();
    logNode->_time.Assign(tools::GetCurrentTimeString());
	if (fileName != nullptr)
		logNode->_name = fileName;
    if ( nullptr != content )
        logNode->_contents.Assign(content);
    else
        logNode->_contents.Assign("NULL");
	_asyncChannel.threadA.push_back(logNode);
}

void Logger::ThreadLog(const char *fileName, const char *content)
{
	LogNode *logNode = (LogNode*)NEW LogNode();
	logNode->_time.Assign(tools::GetCurrentTimeString());
	if (fileName != nullptr)
		logNode->_name = fileName;
	if (nullptr != content)
		logNode->_contents.Assign(content);
	else
		logNode->_contents.Assign("NULL");
	{
		std::lock_guard<std::mutex> guard(_threadLogMutex);
		_threadChannel.threadA.push_back(logNode);
	}
}

void Logger::Process(s32 tick)
{
	s64 start = tools::GetTimeMillisecond();
	{
		std::lock_guard<std::mutex> guard(_asyncChannel.mutex);
		if (_asyncChannel.swap.empty())
			_asyncChannel.threadA.swap(_asyncChannel.swap);
	}
	{
		std::lock_guard<std::mutex> guard(_threadChannel.mutex);
		if (_threadChannel.swap.empty())
		{
			std::lock_guard<std::mutex> guard1(_threadLogMutex);
			if (!_threadChannel.threadA.empty())
				_threadChannel.threadA.swap(_threadChannel.swap);
		}
	}

}

bool Logger::LoadConfig()
{
	const char *configPath = Kernel::GetInstance().GetCoreFile();
	XmlReader reader;
	if (!reader.LoadFile(configPath))
	{
		ASSERT(false, "don't load config, file = %s", configPath);
		return false;
	}
	IXmlObject *logConfig = reader.Root()->GetFirstChrild("log");
	if (logConfig != nullptr)
	{
		_logConfig.path = logConfig->GetAttribute_Str("path");
		_logConfig.splitSize = logConfig->GetAttribute_S32("split_size") * 1024 * 1024;
		_logConfig.splitTime = logConfig->GetAttribute_S32("split_time") * 3600 * 1000;
	}
	if (_logConfig.path.empty())
		_logConfig.path.append("./log");
	if (_logConfig.splitSize == 0)
		_logConfig.splitSize = FILE_SPLIT_SIZE;
	if (_logConfig.splitTime == 0)
		_logConfig.splitTime = TIME_OUT_FOR_CUT_FILE;
}

void Logger::InitLogChannel(LogChannelInfo &channelInfo, const std::string &suffixName, s64 nowTime)
{
	channelInfo.mainLog = NEW LogFile("", suffixName);
	channelInfo.mainLog->SetAutoFlushNum(FLUSH_ASYNC_BATCH);
	channelInfo.suffixName = suffixName;
	channelInfo.flushTime = nowTime;
}

const char * Logger::GetLogTimeString()
{
	return tools::GetCurrentTimeString("%4d_%02d_%02d_%02d_%02d_%02d");
	//return tools::GetCurrentTimeString("%4d_%02d_%02d_%02d");
}

void Logger::ThreadRun()
{
    LogNode *logNode = nullptr;
    s32 loopCount = 0;
	s64 now = tools::GetTimeMillisecond();
    while (true)
    {
		if (loopCount > 50)
		{
			now = tools::GetTimeMillisecond();
			OnCheckChannel(now, _asyncChannel);
			OnCheckChannel(now, _threadChannel);

			loopCount = 0;
		}

		bool dealAsync = DealLogChannelInfo(now, _asyncChannel);
		bool dealThread = DealLogChannelInfo(now, _threadChannel);
		if (!dealAsync && !dealThread)
			Sleep(20);

		loopCount++;
		if (_terminate)
			return;
    }
}

bool Logger::CreateLogFile(LogFile &logFile)
{
	if (logFile.IsOpen())
	{
		ASSERT(false, "log file is open");
		return false;
	}

	tlib::TString<MAX_PATH> logName(_logPrefix);
	if (!logFile.GetName().empty())
	{
		logName << LOG_CONNECT_SIGN << logFile.GetName().c_str();
	}
	logName << LOG_CONNECT_SIGN << GetLogTimeString() << LOG_CONNECT_SIGN << logFile.GetFileSuffixName().c_str() << LOG_FILE_ATT;
	if (!logFile.Open(_logPath.GetString(), logName.GetString()))
	{
		ASSERT(false, "Open log file: %s,error", logName.GetString());
		return false;
	}

	return true;
}

bool Logger::WriteLogNode(LogFile &logFile, const LogNode *logNode)
{
	if (!logFile.IsOpen())
	{
		if (!CreateLogFile(logFile))
			return false;
	}

	if (logFile.GetFileSize() > _logConfig.splitSize)
	{
		logFile.Close();
		if (!CreateLogFile(logFile))
			return false;
	}

	logFile.Write(*logNode);
	return true;
}

void Logger::OnCheckChannel(s64 nowTime, LogChannelInfo &channelInfo)
{
	if (nowTime - channelInfo.flushTime > CHANNEL_CHECK_FLUSH_TIME)
	{
		if (nowTime - channelInfo.mainLog->CreateTick() > _logConfig.splitTime)
		{
			channelInfo.mainLog->Close();
			CreateLogFile(*channelInfo.mainLog);
		}
		else {
			channelInfo.mainLog->Flush();
		}

		for (auto iter = channelInfo.logMap.begin(); iter != channelInfo.logMap.end();)
		{
			if (nowTime - iter->second->GetWriteTick() > _logConfig.splitTime)
			{
				iter->second->Close();
				DEL iter->second;
				iter = channelInfo.logMap.erase(iter);
			}
			else {
				iter->second->Flush();
				iter++;
			}
		}
		channelInfo.flushTime = nowTime;
	}
}

bool Logger::DealLogChannelInfo(s64 nowTime, LogChannelInfo &channelInfo)
{
	if (channelInfo.threadB.empty())
	{
		std::lock_guard<std::mutex> guard(channelInfo.mutex);
		if (!channelInfo.swap.empty())
			channelInfo.threadB.swap(channelInfo.swap);
	}
	LogNode *logNode = nullptr;
	s32 count = 0;
	while (!channelInfo.threadB.empty())
	{
		logNode = channelInfo.threadB.front();
		DealChannleLog(nowTime, channelInfo, logNode);
		DEL logNode;
		channelInfo.threadB.pop_front();
		count++;
		if (count == LOOP_WRIT_LOG_COUNT)
			break;
	}

	return count != 0;
}

void Logger::DealChannleLog(s64 nowTime, LogChannelInfo &channelInfo, const LogNode *logNode)
{
	auto &logMap = channelInfo.logMap;
	if (logNode->_name.Length() != 0)
	{
		auto iter = logMap.find(logNode->_name.GetString());
		if (iter == logMap.end())
		{
			LogFile *logFile = NEW LogFile(logNode->_name.GetString(), channelInfo.suffixName);
			auto retIter = channelInfo.logMap.emplace(logNode->_name.GetString(), logFile);
			if (retIter.second)
				iter = retIter.first;
		}
		WriteLogNode(*iter->second, logNode);
		iter->second->UpdateWriteTick(nowTime);
	}
	WriteLogNode(*channelInfo.mainLog, logNode);
}