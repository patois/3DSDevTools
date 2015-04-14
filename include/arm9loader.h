#pragma once

#define ARM_NOP     0xE1A00000 // NOP

typedef struct {
	u32 is_supported;
	u32 is_decrypted;
	u32 is_dirty;

	u32 address;
	u32 arm9_address;
	u32 check_address;

	u32 arm9_address_backup;
	u32 check_address_backup;
} loader_info_t;

typedef struct {
	u32 checksum;
	u32 arm9_address;
	u32 check_address;
} supported_loader_t;

int init_loader_info (loader_info_t *li);
int patch_loader (loader_info_t *li);
int run_loader (loader_info_t *li);
int restore_loader (loader_info_t *li);
