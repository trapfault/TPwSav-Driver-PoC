#ifndef __SF_H__
#define __SF_H__

#define SUPERFETCH_VERSION      0x2D
#define SUPERFETCH_MAGIC        0x6B756843

#include <Ntdefs.h>

typedef struct _SUPERFETCH_INFORMATION {
	ULONG               Version;
	ULONG               Magic;
	ULONG               InfoClass;
	PVOID               Data;
	ULONG               Length;
} SUPERFETCH_INFORMATION, * PSUPERFETCH_INFORMATION;

typedef struct _PF_PHYSICAL_MEMORY_RANGE {
	ULONG_PTR           BasePfn;
	ULONG_PTR           PageCount;
} PF_PHYSICAL_MEMORY_RANGE, * PPF_PHYSICAL_MEMORY_RANGE;

typedef struct _PF_MEMORY_RANGE_INFO {
	ULONG               Version;
	ULONG               RangeCount;
	PF_PHYSICAL_MEMORY_RANGE Ranges[ANYSIZE_ARRAY];
} PF_MEMORY_RANGE_INFO, * PPF_MEMORY_RANGE_INFO;

typedef struct __declspec(align(8))
_PF_MEMORY_RANGE_INFO_V2 {
	ULONG               Version;
	ULONG               Flags;
	ULONG               RangeCount;
	PF_PHYSICAL_MEMORY_RANGE Ranges[ANYSIZE_ARRAY];
} PF_MEMORY_RANGE_INFO_V2, * PPF_MEMORY_RANGE_INFO_V2;

typedef struct _MEMORY_FRAME_INFORMATION {
	ULONGLONG 			UseDescription : 4;
	ULONGLONG 			ListDescription : 3;
	ULONGLONG 			Reserved0 : 1;
	ULONGLONG 			Pinned : 1;
	ULONGLONG 			DontUse : 48;
	ULONGLONG 			Priority : 3;
	ULONGLONG 			Reserved : 4;
} MEMORY_FRAME_INFORMATION, * PMEMORY_FRAME_INFORMATION;

typedef struct _FILEOFFSET_INFORMATION {
	ULONGLONG 			DontUse : 9;
	ULONGLONG 			Offset : 48;
	ULONGLONG 			Reserved : 7;
} FILEOFFSET_INFORMATION, * PFILEOFFSET_INFORMATION;

typedef struct _PAGEDIR_INFORMATION {
	ULONGLONG 			DontUse : 9;
	ULONGLONG 			PageDirectoryBase : 48;
	ULONGLONG 			Reserved : 7;
} PAGEDIR_INFORMATION, * PPAGEDIR_INFORMATION;

typedef struct _UNIQUE_PROCESS_INFORMATION {
	ULONGLONG 			DontUse : 9;
	ULONGLONG 			UniqueProcessKey : 48;
	ULONGLONG 			Reserved : 7;
} UNIQUE_PROCESS_INFORMATION, * PUNIQUE_PROCESS_INFORMATION;

typedef struct _MMPFN_IDENTITY {
	union {
		MEMORY_FRAME_INFORMATION   e1;
		FILEOFFSET_INFORMATION     e2;
		PAGEDIR_INFORMATION        e3;
		UNIQUE_PROCESS_INFORMATION e4;
	} u1;
	SIZE_T 				PageFrameIndex;
	union {
		struct {
			ULONG 		Image : 1;
			ULONG 		Mismatch : 1;
		} e1;
		PVOID 			FileObject;
		PVOID 			UniqueFileObjectKey;
		PVOID 			ProtoPteAddress;
		PVOID 			VirtualAddress;
	} u2;
} MMPFN_IDENTITY, * PMMPFN_IDENTITY;

typedef struct _SYSTEM_MEMORY_LIST_INFORMATION {
	SIZE_T    			ZeroPageCount;
	SIZE_T    			FreePageCount;
	SIZE_T    			ModifiedPageCount;
	SIZE_T    			ModifiedNoWritePageCount;
	SIZE_T    			BadPageCount;
	SIZE_T    			PageCountByPriority[8];
	SIZE_T    			RepurposedPagesByPriority[8];
	ULONG_PTR 			ModifiedPageCountPageFile;
} SYSTEM_MEMORY_LIST_INFORMATION, * PSYSTEM_MEMORY_LIST_INFORMATION;

typedef struct _PF_PFN_PRIO_REQUEST {
	ULONG				Version;
	ULONG               RequestFlags;
	SIZE_T				PfnCount;
	SYSTEM_MEMORY_LIST_INFORMATION MemInfo;
	MMPFN_IDENTITY		PageData[ANYSIZE_ARRAY];
} PF_PFN_PRIO_REQUEST, * PPF_PFN_PRIO_REQUEST;

ULONGLONG NTAPI
TpwTranslateVirtual(
    ULONGLONG           VirtualAddress
);

#endif // __SF_H__