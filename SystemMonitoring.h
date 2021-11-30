#include "main.h"
TCHAR *PrintProcessName(HANDLE hProcess);
void PrintModules(HANDLE hProcess);
void PrintMemoryInfo(HANDLE hProcess);
void PrintFileIOInfo(HANDLE hProcess);
void PrintCPUusageInfo(DWORD processID);
void PrintSystemInfo();
int NumThreadsOfProcess(DWORD processID);
