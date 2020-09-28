REPLACE_POWER_CONTEXT, *PPNP_REPLACE_POWER_CONTEXT;

typedef struct _PNPR_MEMORY_BITMAP_ENTRY {
    LIST_ENTRY Links;
    PFN_NUMBER StartPfn;
    PFN_NUMBER Pages;
    RTL_BITMAP Bitmap;
} PNPR_MEMORY_BITMAP_ENTRY, *PPNPR_MEMORY_BITMAP_ENTRY;

typedef struct _PNPR_MAPPING_RESERVE {
    PMDL Mdl;
    PVOID Reserve;
    CSHORT MdlFlags;
} PNPR_MAPPING_RESERVE, *PPNPR_MAPPING_RESERVE;

#define PNPR_HAL_BEGIN_RUN 0x1
#define PNPR_DRIVER_BEGIN_RUN 0x2
#define PNPR_SWAP_IN_PROGRESS 0x4
#define PNPR_HARDWARE_MIRRORING 0x8
#define PNPR_HARDWARE_PAGE_COPY 0x10
#define PNPR_HARDWARE_QUIESCE 0x20

typedef struct _PNP_RE