#include <Sf.h>
#include <Utils.h>

BOOLEAN NTAPI
TpwInitializeMemoryRange(
    _Inout_ PPF_MEMORY_RANGE_INFO_V2 *OutInfo,
    _Out_ PSIZE_T       OutSize
);

BOOLEAN
TpwFindPageFrameIndex(
    _In_ ULONGLONG      AlignedVirtualAddress,
    _In_ PPF_MEMORY_RANGE_INFO_V2 MemoryRange,
    _In_ SIZE_T         BufferSize,
    _Out_ PULONG64      PageFrameIndex
);

ULONGLONG NTAPI
TpwTranslateVirtual(
    _In_ ULONGLONG      VirtualAddress
) {
    PPF_MEMORY_RANGE_INFO_V2 MemoryRange = NULL;
    SIZE_T              MemoryRangeSize = 0;

    ULONGLONG AlignedVirtualAddress =
        VirtualAddress & ~0xFFFULL;

    ULONG64             PageFrameIndex = 0;
    ULONGLONG           PhysicalAddress = 0;

    if (TpwInitializeMemoryRange(&MemoryRange, &MemoryRangeSize) == FALSE)
        return 0;

    if (TpwFindPageFrameIndex(AlignedVirtualAddress, MemoryRange, MemoryRangeSize, &PageFrameIndex))
        PhysicalAddress = (PageFrameIndex << 12) | (VirtualAddress & 0xFFFULL);

    HeapFree(
        GetProcessHeap(),
        0,
        MemoryRange
    );

    return PhysicalAddress;
}

BOOLEAN NTAPI
TpwInitializeMemoryRange(
    _Inout_ PPF_MEMORY_RANGE_INFO_V2 *OutInfo,
    _Out_ PSIZE_T       OutSize
) {
    PF_MEMORY_RANGE_INFO_V2 MemoryRangeInfo = { 0 };
    SUPERFETCH_INFORMATION Query = { 0 };

    NtQuerySystemInformation_t TpwNtQuerySystemInformation =
        (NtQuerySystemInformation_t)GetProcAddress(
            GetModuleHandleA("ntdll.dll"),
            "NtQuerySystemInformation"
        );

    if (TpwNtQuerySystemInformation == NULL)
        return FALSE;
    
    MemoryRangeInfo.Version = 2;

    Query.Version   = SUPERFETCH_VERSION;
    Query.Magic     = SUPERFETCH_MAGIC;
    Query.Data      = &MemoryRangeInfo;
    Query.Length    = sizeof(MemoryRangeInfo);
    Query.InfoClass = 17;

    NTSTATUS Status = TpwNtQuerySystemInformation(
        79,         // SystemSuperfetchInformation
        &Query,
        sizeof(Query),
        OutSize
    );

    if (Status != STATUS_BUFFER_TOO_SMALL)
        return FALSE;

    *OutInfo = (PPF_MEMORY_RANGE_INFO_V2)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        *OutSize
    );

    if (*OutInfo == NULL)
        return FALSE;

    (*OutInfo)->Version = 2;

    Query.Data      = *OutInfo;
    Query.Length    = *OutSize;

    Status = TpwNtQuerySystemInformation(
        79,         // SystemSuperfetchInformation
        &Query,
        sizeof(Query),
        OutSize
    );

    if (NT_SUCCESS(Status) == FALSE) {
        HeapFree(
            GetProcessHeap(),
            0,
            *OutInfo
        );

        *OutInfo = NULL;

        return FALSE;
    }

    return TRUE;
}

BOOLEAN
TpwFindPageFrameIndex(
    _In_ ULONGLONG      AlignedVirtualAddress,
    _In_ PPF_MEMORY_RANGE_INFO_V2 MemoryRange,
    _In_ SIZE_T         BufferSize,
    _Out_ PULONG64      PageFrameIndex
) {

    NtQuerySystemInformation_t TpwNtQuerySystemInformation =
        (NtQuerySystemInformation_t)GetProcAddress(
            GetModuleHandleA("ntdll.dll"),
            "NtQuerySystemInformation"
        );

    if (TpwNtQuerySystemInformation == NULL)
        return FALSE;

    for (ULONG i = 0; i < MemoryRange->RangeCount; ++i) {
        PF_PHYSICAL_MEMORY_RANGE* Range = &MemoryRange->Ranges[i];

        SIZE_T RequestSize = sizeof(PF_PFN_PRIO_REQUEST) +
            sizeof(MMPFN_IDENTITY) * Range->PageCount;

        PPF_PFN_PRIO_REQUEST Request = (PPF_PFN_PRIO_REQUEST)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            RequestSize
        );

        if (!Request)
            continue;

        Request->Version = 1;
        Request->RequestFlags = 1;
        Request->PfnCount = Range->PageCount;

        for (ULONG j = 0; j < Range->PageCount; ++j)
            Request->PageData[j].PageFrameIndex = Range->BasePfn + j;

        SUPERFETCH_INFORMATION Query = { 0 };

        Query.Version   = SUPERFETCH_VERSION;
        Query.Magic     = SUPERFETCH_MAGIC;
        Query.Data      = Request;
        Query.Length    = (ULONG)RequestSize;
        Query.InfoClass = 6;

        NTSTATUS Status = TpwNtQuerySystemInformation(
            79,     // SystemSuperfetchInformation
            &Query,
            sizeof(Query),
            NULL
        );

        if (NT_SUCCESS(Status) == FALSE) {
            HeapFree(
                GetProcessHeap(),
                0,
                Request
            );

            continue;
        }

        for (ULONG j = 0; j < Range->PageCount; ++j) {
            MMPFN_IDENTITY* Page = &Request->PageData[j];

            if (Page->u2.VirtualAddress == AlignedVirtualAddress && Page->u2.VirtualAddress != 0) {
                *PageFrameIndex = Page->PageFrameIndex;

                HeapFree(
                    GetProcessHeap(),
                    0,
                    Request
                );
                return TRUE;
            }
        }

        HeapFree(
            GetProcessHeap(),
            0,
            Request
        );
    }

    return FALSE;
}