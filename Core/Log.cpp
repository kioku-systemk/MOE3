

#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

enum LOG_LEVEL
{
	LOG_ERROR = 0,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG
};


void MOELog( enum LOG_LEVEL level, const std::string& msg )
{
	static const char *log_header[4] = {
		"[ERR]", "[WRN]", "[INF]", "[DGB]"
	};

	std::string logstr(log_header[level]);
	
	logstr += std::string(" ") + msg;

#ifdef _WIN32
	OutputDebugStringA(logstr.c_str());
#else
	printf("%s", logstr.c_str());	
#endif
}

void MOELogE (const char *format, ...)
{
	char buf[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(buf, format, arg);
	va_end(arg);
	
	MOELog(LOG_ERROR, buf);
}

void MOELogW (const char *format, ...)
{
	char buf[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(buf, format, arg);
	va_end(arg);
	
	MOELog(LOG_WARN, buf);
}

void MOELogI (const char *format, ...)
{
	char buf[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(buf, format, arg);
	va_end(arg);
	
	MOELog(LOG_INFO, buf);
}

void MOELogD (const char *format, ...)
{
#ifdef _DEBUG
	char buf[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(buf, format, arg);
	va_end(arg);
	
	MOELog(LOG_DEBUG, buf);
#endif
}

