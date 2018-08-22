#include "INVESTIGATION.h"
#include <stdio.h>
#include <tchar.h>
#include <winbase.h>

#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")

#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")

// --------------------------------------------------
// Global variables (only in INVESTIGATION.cpp file.)
// --------------------------------------------------
static Logger loggers[5] = { NULL, NULL, NULL, NULL, NULL };
static TCHAR cleanupFilter[256];

// --------------------------------------------------
// Private function declarations (only in INVESTIGATION.cpp file.)
// --------------------------------------------------
static void CleanupSymbols();
static void InitializeSymbols(HANDLE& process, LPCTSTR filter, DWORD& stackSize);
static void OutputDebugStringToDebugView(LPCTSTR filter, LPCTSTR log);

// --------------------------------------------------
// Public function definitions
// --------------------------------------------------
void Investigation_AddLogger(Logger logger)
{
	for (size_t i = 0; i < _countof(loggers); i++)
	{
		if (loggers[i] == NULL)
		{
			loggers[i] = logger;
			break;
		}
	}
}

void Investigation_MakeLog(LPCTSTR filter, LPCTSTR file, int line, LPCTSTR function, LPTSTR log, size_t countOfLog, LPCTSTR format, ...)
{
	// Position (file, line, function)
	DWORD threadId = GetCurrentThreadId();
	TCHAR position[1024];
	ZeroMemory(position, sizeof(position));
	_stprintf_s(position, _countof(position), _T("(TID:%lu)[%s:(%d)/%s] "), threadId, PathFindFileName(file), line, function);

	// Message
	TCHAR message[1024];
	ZeroMemory(message, sizeof(message));
	va_list args;
	va_start(args, format);
	int result = _vstprintf_s(message, _countof(message), format, args);
	va_end(args);

	// Make Log
	ZeroMemory(log, sizeof(log));
	_tcscat_s(log, countOfLog, filter);
	_tcscat_s(log, countOfLog, position);
	_tcscat_s(log, countOfLog, message);
}

void Investigation_DebugLog(LPCTSTR filter, LPCTSTR log)
{
	// Local static variables to judge initial time and keep configuration values.
	static bool isInitialized = false;
	if (isInitialized == false)
	{
		isInitialized = true;
		bool toView = GetPrivateProfileInt(_T("Output"), _T("DebugView"), 1, _T(INVESTIGATION_INI_FILE_PATH)) == 0 ? false : true;
		if (toView) Investigation_AddLogger(OutputDebugStringToDebugView);
	}

	// Write Log
	for (size_t i = 0; i < _countof(loggers); i++)
	{
		if (loggers[i] == NULL)
			break;
		loggers[i](filter, log);
	}
}

void Investigation_DebugLogStack(LPCTSTR filter, LPCTSTR file, int line, LPCTSTR function, LPCTSTR format, ...)
{
	static bool isInitialized = false;
	static DWORD stackSize = 20;

	TCHAR log[1024];
	HANDLE process = GetCurrentProcess();

	if (isInitialized == false)
	{
		isInitialized = true;
		InitializeSymbols(process, filter, stackSize);
	}

	PVOID* pStack = (PVOID*)malloc(sizeof(PVOID) * stackSize);
	if (pStack == nullptr)
	{
		return;
	}
	unsigned short frames = CaptureStackBackTrace(0, stackSize, pStack, NULL);
	SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	if (pSymbol == nullptr)
	{
		free(pStack);
		return;
	}
	pSymbol->MaxNameLen = 255;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	for (unsigned int i = 0; i < frames; i++)
	{
		SymFromAddr(process, (DWORD64)(pStack[i]), 0, pSymbol);
		TCHAR symbolName[256];
		ZeroMemory(symbolName, sizeof(symbolName));
		MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, pSymbol->Name, (int)strlen(pSymbol->Name), symbolName, (int)(sizeof symbolName) / 2);
		DWORD displacement = 0;
		IMAGEHLP_LINE lineOfAddress = { sizeof(IMAGEHLP_LINE) };
		SymGetLineFromAddr(process, (DWORD64)pStack[i], &displacement, &lineOfAddress);
		// Write
		ZeroMemory(log, sizeof(log));
		Investigation_MakeLog(filter, file, line, function, log, _countof(log), _T("   %i: %s - 0x%0X(Line:%d)"), frames - i - 1, symbolName, pSymbol->Address, lineOfAddress.LineNumber);
		Investigation_DebugLog(filter, log);
	}
	free(pStack);
	free(pSymbol);
}

bool Investigation_MakeBitsString(unsigned long unsignedValue, size_t byteSizeOfValue, LPTSTR pBuffer, size_t countOfBuffer)
{
	if (pBuffer == nullptr)
	{
		return false;
	}
	ZeroMemory(pBuffer, countOfBuffer * sizeof(TCHAR));
	size_t allBitsCount = 8 * byteSizeOfValue;
	if (countOfBuffer < allBitsCount)
	{
		return false;
	}

	unsigned long tmpUnsignedValue = unsignedValue;
	for (size_t i = 0; i < allBitsCount; i++)
	{
		size_t index = (allBitsCount - 1) - i;
		pBuffer[index] = (tmpUnsignedValue % 2 == 0) ? L'0' : L'1';
		tmpUnsignedValue = tmpUnsignedValue / 2;
	}
	return true;
}

// --------------------------------------------------
// Private function definitions (only in INVESTIGATION.cpp file.)
// --------------------------------------------------
static void CleanupSymbols()
{
	SymCleanup(GetCurrentProcess());

	TCHAR log[1024];
	ZeroMemory(log, sizeof(log));
	Investigation_MakeLog(cleanupFilter, _T(__FILE__), __LINE__, _T(__FUNCTION__), log, _countof(log), _T("Symbols cleanup completed."));
	Investigation_DebugLog(cleanupFilter, log);
}

static void InitializeSymbols(HANDLE& process, LPCTSTR filter, DWORD& stackSize)
{
	// Read Stack Size
	stackSize = GetPrivateProfileInt(_T("StackTrace"), _T("Size"), stackSize, _T(INVESTIGATION_INI_FILE_PATH));

	// Read Symbol Path
	char symbolPathA[1024];
	ZeroMemory(symbolPathA, sizeof(symbolPathA));
	GetPrivateProfileStringA("StackTrace", "SymbolPath", "C:\\Symbols", symbolPathA, sizeof(symbolPathA), INVESTIGATION_INI_FILE_PATH);

	// Initialize(Load) Symbol files
	SymInitialize(process, symbolPathA, TRUE);
	DWORD64 result = SymLoadModule(process, NULL, NULL, NULL, 0, 0);

	// Register a function which cleans up symbols when current process exit.
	_tcscpy_s(cleanupFilter, filter);
	atexit(CleanupSymbols);

	// Write Log
	TCHAR log[1024];
	ZeroMemory(log, sizeof(log));
	Investigation_MakeLog(filter, _T(__FILE__), __LINE__, _T(__FUNCTION__), log, _countof(log), _T("InitializeSymbols(...) completed."));
	Investigation_DebugLog(filter, log);
}

static void OutputDebugStringToDebugView(LPCTSTR filter, LPCTSTR log)
{
	OutputDebugString(log);
}