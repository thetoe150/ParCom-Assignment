#include "SystemMonitoring.h"

TCHAR *PrintProcessName(HANDLE hProcess) {
    HMODULE hMods[1024];
    
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        TCHAR szModName[MAX_PATH];
        if (GetModuleBaseNameW(hProcess, hMods[0], szModName,
            sizeof(szModName) / sizeof(TCHAR))) {
            _tprintf(TEXT("\t%s \n"), szModName);
            return szModName;
        }
    }
}

void PrintModules(HANDLE hProcess){
    HMODULE hMods[1024];
    DWORD cbNeeded;
    unsigned int i;

    printf("\n--------------------------------- Paths to the file containing the modules:---------------------------------\n");

    // Get a handle to the process.

    // Get a list of all the modules in this process.

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)){

        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++){
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
                sizeof(szModName) / sizeof(TCHAR))){
            
                // Print the module name and handle value.
                
                _tprintf(TEXT("\t%s \n"), szModName);
            }
        }
    }

    // Release the handle to the process.

    

    return ;
}

void PrintMemoryInfo(HANDLE hProcess){

    PROCESS_MEMORY_COUNTERS_EX pmc;

    printf("\n------------------------------- Information about memory usage of the process:---------------------------------\n");

    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))){
        printf("\tVirtual memory currently used: %u KB\n", pmc.PrivateUsage/1024);
        printf("\tPhysical memory currently used: %u KB\n", pmc.WorkingSetSize / 1024);
        printf("\tPageFaultCount: %u\n", pmc.PageFaultCount);
        printf("\tPeakWorkingSetSize: 0x%08X\n", pmc.PeakWorkingSetSize);       
        printf("\tQuotaPeakPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakPagedPoolUsage);
        printf("\tQuotaPagedPoolUsage: 0x%08X\n", pmc.QuotaPagedPoolUsage);
        printf("\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakNonPagedPoolUsage);
        printf("\tQuotaNonPagedPoolUsage: 0x%08X\n", pmc.QuotaNonPagedPoolUsage);
        printf("\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage);
        printf("\tPeakPagefileUsage: 0x%08X\n", pmc.PeakPagefileUsage);
    }
}

void PrintFileIOInfo(HANDLE hProcess) {

    IO_COUNTERS pmc;

    printf("\n -------------------------------Information about disk read/write operation of the process:---------------------------\n");

    if (GetProcessIoCounters(hProcess, &pmc)) {
        printf("\tThe number of read operations performed: %u\n", pmc.ReadOperationCount);
        printf("\tThe total number of bytes read: %u KB\n", pmc.ReadTransferCount / 1024);
        printf("\tThe number of write operations performed: %u\n", pmc.WriteOperationCount);
        printf("\tThe total number of bytes written: %u KB\n", pmc.WriteTransferCount / 1024);
        printf("\tThe number of other operations performed: %u\n", pmc.OtherOperationCount);        
    }
}

void PrintCPUusageInfo(DWORD processID) {
    HANDLE hProcess;
    SYSTEM_INFO sysInfo;
    FILETIME  lpCreationTime, lpExitTime, lpKernelTime, lpUserTime, CurentSysTime;
    ULARGE_INTEGER KernelTime, UserTime, CreationTime, totalUseTime, totalRealTime, Currenttime;

    SYSTEMTIME CreationSystemTime, KernelSystemTime, UserSystemTime, CurentSystemTime;
    double CPUusagePercent;


    GetSystemInfo(&sysInfo);
    int numProcessors = sysInfo.dwNumberOfProcessors;
    
    GetSystemTimeAsFileTime(&CurentSysTime);

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (NULL == hProcess) return ;
    printf("\n ---------------------------------Information about CPU usage of the process:---------------------------------\n");
    if (GetProcessTimes(hProcess, &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTime)) {
        
        memcpy(&Currenttime, &CurentSysTime, sizeof(FILETIME));
        memcpy(&CreationTime, &lpCreationTime, sizeof(FILETIME));
        /*
        uint64_t EndTime;
        if (&lpExitTime == NULL) {
             EndTime = Currenttime;
        }
        else {
             EndTime = FromFileTime(&lpExitTime);
        }
        */
         memcpy(&KernelTime, &lpKernelTime, sizeof(FILETIME));
         memcpy(&UserTime, &lpUserTime, sizeof(FILETIME));
         CPUusagePercent = (KernelTime.QuadPart + UserTime.QuadPart)/ numProcessors;
         CPUusagePercent /= (Currenttime.QuadPart - CreationTime.QuadPart);
         CPUusagePercent *=  100;

         //Change filetime type to readable time format
         FileTimeToSystemTime(&lpCreationTime, &CreationSystemTime);
         FileTimeToSystemTime(&CurentSysTime, &CurentSystemTime);
         FileTimeToSystemTime(&lpKernelTime, &KernelSystemTime);
         FileTimeToSystemTime(&lpUserTime, &UserSystemTime);
    }
    printf("\tCreation time of this process:%02d:%02d:%02d.%03d  %02d/%02d/%d\n", 
         CreationSystemTime.wHour, CreationSystemTime.wMinute,
        CreationSystemTime.wSecond, CurentSystemTime.wMilliseconds, CreationSystemTime.wDay, CreationSystemTime.wMonth,
        CreationSystemTime.wYear);
    printf("\tCurrent time:%02d:%02d:%02d.%03d  %02d/%02d/%d\n", 
         CurentSystemTime.wHour, CurentSystemTime.wMinute,
        CurentSystemTime.wSecond, CurentSystemTime.wMilliseconds, CurentSystemTime.wDay, CurentSystemTime.wMonth,
        CurentSystemTime.wYear );
    printf("\tKernel time of this process: %02d:%02d:%02d.%03d\n",
         KernelSystemTime.wHour, KernelSystemTime.wMinute,
        KernelSystemTime.wSecond, KernelSystemTime.wMilliseconds);
    printf("\tUser time of this process: %02d:%02d:%02d.%03d\n",
         UserSystemTime.wHour, UserSystemTime.wMinute,
        UserSystemTime.wSecond, UserSystemTime.wMilliseconds);
   // printf("\tTotal CPU time of this process: %llu\n", static_cast<long long int>(totalUseTime));
   // printf("\tTotal real time running of this process: %llu\n", static_cast<long long int>(totalRealTime));
    printf("\tAverage CPU usage of this process since it was created: %.2f%%\n", CPUusagePercent);
    CloseHandle(hProcess);
}

void PrintSystemInfo() {
    PERFORMANCE_INFORMATION pPerformanceInformation;
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

    printf("\tTotal virtual memory of the system: %lu MB\n", totalVirtualMem/(1048576));
    printf("\tTotal physical memory of the system: %lu MB\n", totalPhysMem / (1048576));
    printf("\tPercentage of used virtual memory: %lu%% \n", virtualMemUsed*100/totalVirtualMem);
    printf("\tPercentage of used physical memory: %lu%% \n", physMemUsed*100/totalVirtualMem);

    if (GetPerformanceInfo(&pPerformanceInformation, sizeof(pPerformanceInformation))) {
        printf("\tSystemCache: 0x%08X\n", pPerformanceInformation.SystemCache);
        printf("\tPageSize: 0x%08X\n",
            pPerformanceInformation.PageSize);
        printf("\tHandleCount: %u\n", pPerformanceInformation.HandleCount);
        printf("\tProcessCount: %u\n",
            pPerformanceInformation.ProcessCount);
        printf("\tThreadCount: %u\n",
            pPerformanceInformation.ThreadCount);
        printf("\tKernelTotal: %u\n",
            pPerformanceInformation.KernelTotal);
    }
}
int NumThreadsOfProcess(DWORD processID){
    // first determine the id of the current process
    DWORD const  id = processID;

    // then get a process list snapshot.
    HANDLE const  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

    // initialize the process entry structure.
    PROCESSENTRY32 entry = { 0 };
    entry.dwSize = sizeof(entry);

    // get the first process info.
    BOOL  ret = true;
    ret = Process32First(snapshot, &entry);
    while (ret && entry.th32ProcessID != id) {
        ret = Process32Next(snapshot, &entry);
    }
    CloseHandle(snapshot);
    return ret
        ? entry.cntThreads
        : 0;
}