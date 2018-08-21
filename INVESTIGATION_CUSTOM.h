#ifndef _INVESTIGATION_CUSTOM_
#define _INVESTIGATION_CUSTOM_

#include "INVESTIGATION.h"

// --------------------------------------------------
// Public function declarations
// --------------------------------------------------
void InvestigationCustom_DebugLog(LPCTSTR filter, LPCTSTR log);

// --------------------------------------------------
// Public Macros (Normally you should use these macros.)
// --------------------------------------------------
#define DEBUG_LOG_CUSTOM(filter, format, ...) \
	do { \
		if (filter == nullptr || _tcslen(filter) == 0) break; \
		TCHAR _____log[1024];\
		ZeroMemory(_____log, sizeof(_____log));\
		Investigation_MakeLog(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), _____log, _countof(_____log), format, __VA_ARGS__);\
		InvestigationCustom_DebugLog(filter, _____log);\
	} while (0);

#define DEBUG_LOG_CUSTOM_STACK(filter, format, ...) \
	do { \
		if (filter == nullptr || _tcslen(filter) == 0) break;\
		TCHAR _____description[256];\
		ZeroMemory(_____description, sizeof(_____description));\
		_stprintf(_____description, format, __VA_ARGS__);\
		DEBUG_LOG_CUSTOM(filter, _T(">> CallStack: %s"), _____description);\
		Investigation_DebugLogStack(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), format, __VA_ARGS__);\
		DEBUG_LOG_CUSTOM(filter, _T("<< CallStack"));\
	} while (0);

#endif _INVESTIGATION_CUSTOM_
