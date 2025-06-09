#ifndef __UTILS_H__
#define __UTILS_H__

#include <Ntdefs.h>

BOOLEAN NTAPI
TpwRequestDebugPrivilege(
    VOID
);

BOOLEAN NTAPI
TpwArbitaryWriteByte(
    _In_ ULONGLONG      VirtualAddress,
    _In_ BYTE           Value
);

BOOLEAN NTAPI
TpwArbitaryWriteWord(
    _In_ ULONGLONG      VirtualAddress,
    _In_ WORD           Value
);

BOOLEAN NTAPI
TpwArbitaryWriteDword(
    _In_ ULONGLONG      VirtualAddress,
    _In_ DWORD          Value
);

BOOLEAN NTAPI
TpwArbitaryWriteQword(
    _In_ ULONGLONG      VirtualAddress,
    _In_ ULONGLONG      Value
);

BOOLEAN NTAPI
TpwArbitaryReadByte(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PBYTE         OutValue
);

BOOLEAN NTAPI
TpwArbitaryReadWord(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PWORD         OutValue
);

BOOLEAN NTAPI
TpwArbitaryReadDword(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PDWORD        OutValue
);

BOOLEAN NTAPI
TpwArbitaryReadQword(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PULONGLONG    OutValue
); 

BOOLEAN NTAPI
TpwArbitaryWriteIOByte(
	_In_ DWORD          Port,
	_In_ DWORD          Value
);

#endif // __UTILS_H__