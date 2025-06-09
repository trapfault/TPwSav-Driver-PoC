#include <Utils.h>

BOOLEAN NTAPI
TpwRequestDebugPrivilege(
    VOID
) {
    NTSTATUS            Status;
    BOOLEAN             OldValue;

    RtlAdjustPrivilege_t TpwRtlAdjustPrivilege =
        (RtlAdjustPrivilege_t)GetProcAddress(
            GetModuleHandleA("ntdll.dll"),
            "RtlAdjustPrivilege"
        );

    if (TpwRtlAdjustPrivilege == NULL)
        return FALSE;

    Status = TpwRtlAdjustPrivilege(
        (13L),          // SE_PROF_SINGLE_PROCESS_PRIVILEGE
        TRUE,
        FALSE,
        &OldValue
    );

    if (NT_SUCCESS(Status) == FALSE) 
        return FALSE;

    Status = TpwRtlAdjustPrivilege(
        (20L),          // SE_DEBUG_PRIVILEGE
        TRUE,
        FALSE,
        &OldValue
    );

    if (NT_SUCCESS(Status) == FALSE) 
        return FALSE;

    return TRUE;
}
