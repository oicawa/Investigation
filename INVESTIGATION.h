#ifndef _INVESTIGATION_
#define _INVESTIGATION_

#include <windows.h>

#define INVESTIGATION_INI_FILE_PATH "C:\\Investigation.ini"

typedef void(*Logger)(LPCTSTR filter, LPCTSTR message);

// --------------------------------------------------
// Public function declarations
// --------------------------------------------------
void Investigation_AddLogger(Logger logger);
void Investigation_MakeLog(LPCTSTR filter, LPCTSTR file, int line, LPCTSTR function, LPTSTR log, size_t countOfLog, LPCTSTR format, ...);
void Investigation_DebugLog(LPCTSTR filter, LPCTSTR log);
void Investigation_DebugLogStack(LPCTSTR filter, LPCTSTR file, int line, LPCTSTR function, LPCTSTR format, ...);

// --------------------------------------------------
// Public Macros (Normally you should use these macros.)
// --------------------------------------------------
#define DEBUG_LOG(filter, format, ...) \
	do { \
		if (filter == nullptr || _tcslen(filter) == 0) break; \
		TCHAR _____log[1024];\
		ZeroMemory(_____log, sizeof(_____log));\
		Investigation_MakeLog(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), _____log, _countof(_____log), format, __VA_ARGS__);\
		Investigation_DebugLog(filter, _____log);\
	} while (0);

#define DEBUG_LOG_STACK(filter, format, ...) \
	do { \
		if (filter == nullptr || _tcslen(filter) == 0) break;\
		TCHAR _____description[256];\
		ZeroMemory(_____description, sizeof(_____description));\
		_stprintf(_____description, format, __VA_ARGS__);\
		DEBUG_LOG(filter, _T(">> CallStack: %s"), _____description);\
		Investigation_DebugLogStack(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), format, __VA_ARGS__);\
		DEBUG_LOG(filter, _T("<< CallStack"));\
	} while (0);

#endif _INVESTIGATION_
