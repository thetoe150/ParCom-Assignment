#include "SystemMonitoring.h"

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

#define OMP_NUM_THREADS 12
const wchar_t* GetWC(const char* c);
int main(void) {
    DWORD aProcesses[1024];
    DWORD cbNeeded;
    DWORD cProcesses;
    int i;


    PrintSystemInfo();
    printf("-----------------------------------------------------------------\n");
    // Get the list of process identifiers.

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return 1;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the names of the modules for each process.

        omp_set_num_threads(OMP_NUM_THREADS);
        #pragma omp parallel 
        {
            #pragma omp for private(i) schedule(static)
            for (i = 0; i < cProcesses; i++) {
                HANDLE hProcess;
                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

                // Print the process identifier.
                printf("\nProcess ID: %u\n", aProcesses[i]);
                printf("Process's name: ");
                TCHAR toFind[] = _T("devenv.exe");

                //TCHAR searchProcess[MAX_PATH];
                //TCHAR currentProcess[MAX_PATH];
                //_tprintf(TEXT("\t%s \n"), PrintProcessName(aProcesses[i]));
                //wcscat(searchProcess, L"ParCom_Assignment.exe"); printf("searchProcess: %s", searchProcess);
                //wcscat(currentProcess, PrintProcessName(aProcesses[i])); printf("currentProcess: %s", currentProcess);


                PrintProcessName(hProcess);
                printf("\tThe number of threads in this process is: %u\n", NumThreadsOfProcess(aProcesses[i]));
                //PrintModules(hProcess);
                PrintCPUusageInfo(aProcesses[i]);
                PrintMemoryInfo(hProcess);
                PrintFileIOInfo(hProcess);

                printf("-------------------------------------------------------------------------------------\n");

                CloseHandle(hProcess);

            }
        }
        

    return 0;
}

const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}