#ifndef _INVESTIGATION_
#define _INVESTIGATION_

#include <windows.h>

#define INVESTIGATION_INI_FILE_PATH "C:\\Investigation.ini"
#define INVESTIGATION_DEBUG_FILTER _T("Investigation-Debug")

typedef void(*Logger)(LPCTSTR filter, LPCTSTR message);

// --------------------------------------------------
// Public function declarations
// --------------------------------------------------
void Investigation_AddLogger(Logger logger);
void Investigation_MakeLog(LPCTSTR filter, LPCTSTR file, int line, LPCTSTR function, LPTSTR log, size_t countOfLog, LPCTSTR format, ...);
void Investigation_DebugLog(LPCTSTR filter, LPCTSTR log);
void Investigation_DebugLogStack(LPCTSTR filter, LPCTSTR file, int line, LPCTSTR function, LPCTSTR format, ...);
bool Investigation_MakeBitsString(unsigned long unsignedValue, size_t byteSizeOfValue, LPTSTR pBuffer, size_t countOfBuffer);
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
		TCHAR _____formatEx[256];\
		ZeroMemory(_____formatEx, sizeof(_____formatEx));\
		_tcscat(_____formatEx, _T(">> CallStack: "));\
		_tcscat(_____formatEx, format);\
		TCHAR _____log[1024];\
		ZeroMemory(_____log, sizeof(_____log));\
		Investigation_MakeLog(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), _____log, _countof(_____log), _____formatEx, __VA_ARGS__);\
		Investigation_DebugLog(filter, _____log);\
		Investigation_DebugLogStack(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), format, __VA_ARGS__);\
		Investigation_DebugLog(filter, _T("<< CallStack"));\
	} while (0);

#define DEBUG_LOG_BITS(filter, unsignedValue, byteSizeOfValue, format, ...) \
	do { \
		if (filter == nullptr || _tcslen(filter) == 0) break; \
		TCHAR _____log[1024];\
		ZeroMemory(_____log, sizeof(_____log));\
		Investigation_MakeLog(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), _____log, _countof(_____log), format, __VA_ARGS__);\
		TCHAR _____bits[1024];\
		ZeroMemory(_____bits, sizeof(_____bits));\
		Investigation_MakeBitsString(unsignedValue, byteSizeOfValue, _____bits, _countof(_____bits));\
		_tcscat(_____log, _T(" >> "));\
		_tcscat(_____log, _T(#unsignedValue));\
		_tcscat(_____log, _T("="));\
		_tcscat(_____log, _____bits);\
		Investigation_DebugLog(filter, _____log);\
	} while (0);

#endif _INVESTIGATION_
