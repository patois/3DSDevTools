#include "common.h"
#include "arm9loader.h"
#include "firm.h"
#include "loader.h"


#define NUM_LOADER_HACKS 1
static supported_loader_t loaders[NUM_LOADER_HACKS] = {
	{0x3FE0D972, 0x08091580, 0x0808C428} // N3DS 9.0.0.20E
};

u32 get_checksum (void *address, u32 size) {
	u32 result = 0;
	u32 i;

	if ((size % 4) == 0) {
		for(i = 0; i < size / 4; i++) {
			result += *((u32 *)address + i);
		}
	}
	return result;
}

int patch_loader (loader_info_t *li) {
	u32 result = 0;

	if (li) {
		li->arm9_address_backup = *(u32 *)li->arm9_address;
		li->check_address_backup = *(u32 *)li->check_address;
		*(u32 *)li->arm9_address = (u32)&loader_return;
		*(u32 *)li->check_address = ARM_NOP;
		li->is_dirty = 1;
		result = 1;
	}
	return result;
}

int run_loader (loader_info_t *li) {
	u32 result = 0;

	if (li && li->is_dirty && li->is_supported) {
		loader_execute(li->address);
		result = 1;
	}
	return result;
}

int restore_loader (loader_info_t *li) {
	u32 result = 0;

	if (li && li->is_dirty) {
		*(u32 *)li->arm9_address = li->arm9_address_backup;
		*(u32 *)li->check_address = li->check_address_backup;
		li->is_dirty = 0;
		result = 1;
	}
	return result;
}

int init_loader_info (loader_info_t *li) {
	u32 result = 0;
	u32 chksm = 0;
	int i;

	if (li) {
		memset(li, 0, sizeof(loader_info_t));
		chksm = get_checksum((void *)firm, sizeof(firm_header_t));
		if (chksm) {
			for (i=0; i<NUM_LOADER_HACKS; i++) {
				if (chksm == loaders[i].checksum) {
					li->is_supported = 1;
					li->arm9_address = loaders[i].arm9_address;
					li->check_address = loaders[i].check_address;
					result = 1;
					break;
				}
			}
		}
		li->is_decrypted = 0;
	}
	return result;
}
