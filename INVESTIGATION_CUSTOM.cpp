#include "INVESTIGATION_CUSTOM.h"

// --------------------------------------------------
// Private function declarations (only in INVESTIGATION_CUSTOM.cpp file.)
// --------------------------------------------------
static void FunctionToCallCustomLog(LPCTSTR filter, LPCTSTR message);

// --------------------------------------------------
// Public function definitions
// --------------------------------------------------

void InvestigationCustom_DebugLog(LPCTSTR filter, LPCTSTR log)
{
	// Local static variables to judge initial time and keep configuration values.
	static bool isInitialized = false;
	if (isInitialized == false)
	{
		isInitialized = true;
		bool toCustom = GetPrivateProfileInt(_T("Output"), _T("Custom"), 0, _T(INVESTIGATION_INI_FILE_PATH)) == 0 ? false : true;
		if (toCustom) Investigation_AddLogger(FunctionToCallCustomLog);
	}

	// Write Log
	Investigation_DebugLog(filter, log);
}

// --------------------------------------------------
// Private function definitions (only in INVESTIGATION.cpp file.)
// --------------------------------------------------
static void FunctionToCallCustomLog(LPCTSTR filter, LPCTSTR message)
{
	OutputDebugString(_T("Implement here to call something Log statement you want."));
}
