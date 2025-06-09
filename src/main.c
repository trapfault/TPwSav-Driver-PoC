#include <Ntdefs.h>
#include <Sf.h>
#include <Utils.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <Psapi.h>


#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

#pragma comment(lib, "ntdll.lib")

int main() {
    BOOLEAN             Result;

    if (TpwRequestDebugPrivilege() == FALSE) {
        printf("[!] Failed to request debug privilege. RUN AS ADMINISTRATOR!\xA");
        return 1;
    }

    printf("[+] Debug privilege granted.\xA");

    ULONGLONG SystemProcess = TpwGetEprocessFromPid(4);
    if (SystemProcess == 0) {
        printf("[!] Failed to get EPROCESS for system process.\xA");
        return 1;
    }

    DWORD ProcessIdCheck = 0;
    Result = TpwArbitaryReadDword(
        TpwTranslateVirtual(SystemProcess + 0x1D0),     // 0x1D0 : UniqueProcessId
        &ProcessIdCheck
    );

    if (Result == FALSE) {
        printf("[!] Failed to read UniqueProcessId from system process.\xA");
        return 1;
    }

    printf("[+] System process UniqueProcessId: %lu\xA", ProcessIdCheck);
    printf("[+] System process UniqueProcessId %s match expected value.\xA",
           (ProcessIdCheck == 4) ? "does" : "does not");

    // Utilize TpwArbitaryReadByte, TpwArbitaryReadWord, TpwArbitaryReadDword and TpwArbitaryReadQword
    // DON'T FORGET TO USE TpwTranslateVirtual FOR ALL VIRTUAL ADDRESSES!

    return 0;
}