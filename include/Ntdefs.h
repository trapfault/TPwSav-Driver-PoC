#ifndef __NTDEFS_H__
#define __NTDEFS_H__

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <Windows.h>
#include <winternl.h>

typedef
NTSTATUS(NTAPI* RtlAdjustPrivilege_t)(
	ULONG               Privilege,
	BOOLEAN             Enable,
	BOOLEAN             CurrentThread,
	PBOOLEAN            OldValue
	);

typedef
NTSTATUS(NTAPI* NtQuerySystemInformation_t)(
	ULONG               SystemInformationClass,
	PVOID               SystemInformation,
	ULONG               SystemInformationLength,
	PULONG              ReturnLength
	);

typedef struct _SYSTEM_HANDLE
{
	PVOID 				Object;
	HANDLE 				UniqueProcessId;
	HANDLE 				HandleValue;
	ULONG 				GrantedAccess;
	USHORT 				CreatorBackTraceIndex;
	USHORT 				ObjectTypeIndex;
	ULONG 				HandleAttributes;
	ULONG 				Reserved;
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
	ULONG_PTR 			HandleCount;
	ULONG_PTR 			Reserved;
	SYSTEM_HANDLE 		Handles[ANYSIZE_ARRAY];
} SYSTEM_HANDLE_INFORMATION_EX, * PSYSTEM_HANDLE_INFORMATION_EX;

ULONGLONG NTAPI
TpwGetEprocessFromPid(
    _In_ DWORD          TargetPid
);

#endif // __NTDEFS_H__