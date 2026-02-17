#pragma once

#include <windows.h>

#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE 0x00000040L
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#ifndef STARTF_USESHOWWINDOW
#define STARTF_USESHOWWINDOW 0x00000001
#endif

#ifndef STARTF_USESTDHANDLES
#define STARTF_USESTDHANDLES 0x00000100
#endif

#ifndef SW_HIDE
#define SW_HIDE 0
#endif

#ifndef CREATE_NO_WINDOW
#define CREATE_NO_WINDOW 0x08000000
#endif

#ifndef CREATE_UNICODE_ENVIRONMENT
#define CREATE_UNICODE_ENVIRONMENT 0x00000400
#endif

#ifndef LOGON_WITH_PROFILE
#define LOGON_WITH_PROFILE 0x00000001
#endif

#ifndef HEAP_ZERO_MEMORY
#define HEAP_ZERO_MEMORY 0x00000008
#endif

#ifndef TOKEN_ALL_ACCESS
#define TOKEN_ALL_ACCESS 0x000F01FF
#endif

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor; 
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES ); \
	(p)->RootDirectory = r; \
	(p)->Attributes = a; \
	(p)->ObjectName = n; \
	(p)->SecurityDescriptor = s; \
	(p)->SecurityQualityOfService = NULL; \
}

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemMirrorMemoryInformation,
	SystemPerformanceTraceInformation,
	SystemObsolete0,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemVerifierAddDriverInformation,
	SystemVerifierRemoveDriverInformation,
	SystemProcessorIdleInformation,
	SystemLegacyDriverInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation,
	SystemTimeSlipNotification,
	SystemSessionCreate,
	SystemSessionDetach,
	SystemSessionInformation,
	SystemRangeStartInformation,
	SystemVerifierInformation,
	SystemVerifierThunkExtend,
	SystemSessionProcessInformation,
	SystemLoadGdiDriverInSystemSpace,
	SystemNumaProcessorMap,
	SystemPrefetcherInformation,
	SystemExtendedProcessInformation,
	SystemRecommendedSharedDataAlignment,
	SystemComPlusPackage,
	SystemNumaAvailableMemory,
	SystemProcessorPowerInformation,
	SystemEmulationBasicInformation,				
	SystemEmulationProcessorInformation,		
	SystemExtendedHandleInformation,
	SystemLostDelayedWriteInformation,
	SystemBigPoolInformation,
	SystemSessionPoolTagInformation,
	SystemSessionMappedViewInformation,
	SystemHotpatchInformation,
	SystemObjectSecurityMode,
	SystemWatchdogTimerHandler,
	SystemWatchdogTimerInformation,
	SystemLogicalProcessorInformation,
	SystemWow64SharedInformation,
	SystemRegisterFirmwareTableInformationHandler,
	SystemFirmwareTableInformation,
	SystemModuleInformationEx,
	SystemVerifierTriageInformation,
	SystemSuperfetchInformation,
	SystemMemoryListInformation,
	SystemFileCacheInformationEx,
	SystemThreadPriorityClientIdInformation,
	SystemProcessorIdleCycleTimeInformation,
	SystemVerifierCancellationInformation,
	SystemProcessorPowerInformationEx,
	SystemRefTraceInformation,
	SystemSpecialPoolInformation,
	SystemProcessIdInformation,
	SystemErrorPortInformation,
	SystemBootEnvironmentInformation,
	SystemHypervisorInformation,
	SystemVerifierInformationEx,
	SystemTimeZoneInformation,
	SystemImageFileExecutionOptionsInformation,
	SystemCoverageInformation,
	SystemPrefetchPatchInformation,
	SystemVerifierFaultsInformation,
	SystemSystemPartitionInformation,
	SystemSystemDiskInformation,
	SystemProcessorPerformanceDistribution,
	SystemNumaProximityNodeInformation,
	SystemDynamicTimeZoneInformation,
	SystemCodeIntegrityInformation,
	SystemProcessorMicrocodeUpdateInformation,
	SystemProcessorBrandString,
	SystemVirtualAddressInformation,
	MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

WINBASEAPI BOOL WINAPI KERNEL32$CloseHandle(HANDLE hObject);
WINBASEAPI DWORD WINAPI KERNEL32$GetLastError();
WINBASEAPI PVOID WINAPI KERNEL32$HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
WINBASEAPI PVOID WINAPI KERNEL32$HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
WINBASEAPI PVOID WINAPI KERNEL32$HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
WINBASEAPI HANDLE WINAPI KERNEL32$GetProcessHeap();
WINBASEAPI HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI FARPROC WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI HANDLE WINAPI KERNEL32$CreateNamedPipeA(LPCSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD dwDefaultTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
WINBASEAPI BOOL WINAPI KERNEL32$ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped);
WINBASEAPI BOOL WINAPI KERNEL32$OpenProcess(PHANDLE ProcessHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
DECLSPEC_IMPORT NTSTATUS NTAPI NTDLL$NtOpenProcess(PHANDLE ProcessHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
WINBASEAPI BOOL WINAPI KERNEL32$CreatePipe(PHANDLE hReadPipe, PHANDLE hWritePipe, LPSECURITY_ATTRIBUTES lpPipeAttributes, DWORD nSize);
WINBASEAPI BOOL WINAPI KERNEL32$ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPDWORD lpOverlapped);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI ADVAPI32$ImpersonateNamedPipeClient(HANDLE hNamedPipe);
DECLSPEC_IMPORT NTSTATUS NTAPI NTDLL$NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
DECLSPEC_IMPORT NTSTATUS NTAPI NTDLL$NtDuplicateObject(HANDLE, HANDLE *, HANDLE, HANDLE *, ACCESS_MASK, BOOLEAN, ULONG);
WINBASEAPI BOOL WINAPI NTDLL$NtQueryInformationToken(HANDLE TokenHandle, DWORD TokenInformationClass, PVOID TokenInformation, ULONG TokenInformationLength, PULONG ReturnLength);
WINBASEAPI BOOL WINAPI ADVAPI32$ImpersonateLoggedOnUser(HANDLE hToken);
WINBASEAPI BOOL WINAPI ADVAPI32$RevertToSelf();
WINBASEAPI BOOL WINAPI ADVAPI32$DuplicateTokenEx(HANDLE hExistingToken, DWORD dwDesiredAccess, LPSECURITY_ATTRIBUTES lpTokenAttributes, SECURITY_IMPERSONATION_LEVEL ImpersonationLevel, TOKEN_TYPE TokenType, PHANDLE phNewToken);
WINBASEAPI BOOL WINAPI ADVAPI32$CreateProcessWithTokenW(HANDLE hToken, DWORD dwLogonFlags, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
WINBASEAPI DWORD WINAPI KERNEL32$WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
WINBASEAPI VOID WINAPI KERNEL32$GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
WINBASEAPI BOOL WINAPI KERNEL32$GetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode);
WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
WINBASEAPI size_t WINAPI MSVCRT$mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count);
