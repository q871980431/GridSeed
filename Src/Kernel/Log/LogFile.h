#ifndef __LogFile_h__
#define __LogFile_h__
#include "MultiSys.h"
#include "TString.h"

#define LOG_FILE_ATT    ".log"
#define LOG_CONNECT_SIGN "_"

struct LogNode
{
	tlib::TString<64> _time;
	tlib::TString<64> _name;
	tlib::TString<LOG_BUFF_SIZE> _contents;
	LogNode() :_time(), _contents(), _name() {};
	LogNode(const char *time, const char *contents) :_time(time), _contents(contents) {};
};

class LogFile
{
public:
    LogFile(const std::string &name, const std::string &suffixName) :_name(name), _suffixName(suffixName){};
    ~LogFile(){ Close(); };

    bool Open(const char *path, const char *name);
    void Close();
    void Write(const char *contents);
	void Write(const LogNode &logNode);
    void Flush();

public:
    inline bool IsOpen(){ return nullptr != _file; };
    inline s64  CreateTick(){ return _createTick; };
	inline void SetAutoFlushNum(s32 autoFlushNum) { _flushNum = autoFlushNum; };
	inline std::string & GetFileSuffixName() { return _suffixName; };
	inline std::string & GetName() { return _name; };
	inline s32  GetFileSize() { return _fileSize; };
	inline void UpdateWriteTick(s64 tick) { _writeTick = tick; };
	inline s64  GetWriteTick() { return _writeTick; };

private:
	s32  GetFileSize(const char *filePath);

private:
	FILE    *_file{nullptr};
	s64     _createTick{0};
	s32		_writeCount{0};
	s32		_flushNum{256};
	std::string _name;
	std::string _suffixName;
	s32		_fileSize{0};
	s64		_writeTick{0};	//只对命名文件有效
};
#endif