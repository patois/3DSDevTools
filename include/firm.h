#pragma once

#include "common.h"

int get_section_by_address (void *address);
int is_valid_firm (void);
void dump_section_header (int index);
u32 get_FIRM_total_size (void);

#define ARM9_FIRM_IMAGE        0x24000000
#define FIRM_MAGIC             0x4D524946
#define FIRM_MAX_SECTION_COUNT 0x4

typedef struct {
	u32 offset;
	void *address;
	u32 size;
	u32 type;
	u8 hash[0x20];
} firm_section_header_t;

typedef struct {
	u32 magic;
	u32 reserved1;
	void *arm11_ep;
	void *arm9_ep;
	u8 reserved2[0x30];
	firm_section_header_t section_headers[4];
	u8 rsa_sig[0x100];
} firm_header_t;

static firm_header_t volatile *const firm = (firm_header_t *)ARM9_FIRM_IMAGE;  
