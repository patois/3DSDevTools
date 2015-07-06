#pragma once

#define PATH_BOOTROM          "/bootrom.dmp"
#define PATH_AXIWRAM          "/axiwram.dmp"
#define PATH_ARM9INTERNAL     "/arm9internal.dmp"
#define PATH_FCRAM            "/fcram.dmp"
#define PATH_FIRM_HEADER      "/firm.hdr.dmp"
#define PATH_FMT_FIRM_SECTION "/firm.section[%d].dmp"

#define ARM9_AXIWRAM          0x1FF80000
#define ARM9_AXIWRAM_SIZE     0x00080000

#define ARM9_FCRAM            0x20000000
#define ARM9_FCRAM_SIZE       0x08000000
#define ARM9_FCRAM_SIZE_N3DS  0x10000000

#define ARM9_INTERNAL         0x08000000
#define ARM9_INTERNAL_SIZE    0x00100000

#define ARM9_BOOTROM          0xFFFF0000
#define ARM9_BOOTROM_SIZE     0x00010000

typedef int progress_printer_t(int oldstep, u32 val, u32 maval);

u32 dump_mem (const char *filename, void *addr, u32 size, progress_printer_t *f);
