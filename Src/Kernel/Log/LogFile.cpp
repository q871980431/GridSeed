#include "LogFile.h"
#include "Tools_time.h"
#include <sys/stat.h>


bool LogFile::Open(const char *path, const char *name)
{
    char filePath[MAX_PATH];
    ASSERT(nullptr == _file, "file exist");
    SafeSprintf(filePath, sizeof(filePath), "%s/%s", path, name);
	_fileSize = GetFileSize(filePath);
    _file = fopen(filePath, "ab+");
    if ( nullptr == _file )
    {
        ASSERT(false, "file open failed, file name = %s", filePath);
        return false;
    }
    _createTick = tools::GetTimeMillisecond();
    return true;
}

void LogFile::Close()
{
    if (_file)
    {
        fflush(_file);
        fclose(_file);

        _file = nullptr;
        _createTick = 0;
    }
}
void LogFile::Write(const char *contents)
{
    if (_file)
    {
        if (nullptr != contents )
        {
            fwrite(contents, (s32)strlen(contents), 1, _file);
        }else
        {
            fwrite("NULL", sizeof("NULL") - 1, 1, _file);
        }
		++_writeCount;
    }
}

void LogFile::Write(const LogNode &logNode)
{
	if (_file)
	{
		s32 count = logNode._time.Length();
		fwrite(logNode._time.GetString(), (s32)logNode._time.Length(), 1, _file);
		fwrite(" | ", sizeof(" | ") - 1, 1, _file);
		fwrite(logNode._contents.GetString(), (s32)logNode._contents.Length(), 1, _file);
		fwrite("\n", 1, 1, _file);
		count += sizeof(" | ") - 1;
		count += (s32)logNode._contents.Length();
		count += 1;
		_fileSize += count;
		++_writeCount;
		if (_writeCount > _flushNum)
			Flush();
	}
}

void LogFile::Flush()
{
	if (_writeCount > 0)
	{
		if (_file)
		{
			fflush(_file);
		}
		_writeCount = 0;
	}
}

s32  LogFile::GetFileSize(const char *filePath)
{
#ifdef WIN32
	struct _stat info;
	_stat(filePath, &info);
	return info.st_size;
#else
	struct stat info;
	stat(filePath, &info);
	return info.st_size;
#endif
}