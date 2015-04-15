#include "stdio.h"
#include "draw.h"
#include "dumper.h"
#include "hid.h"
#include "firm.h"
#include "textmenu.h"
#include "platform.h"
#include "arm9loader.h"
#include "interface.h"
#include "loader.h"

struct menu_t main_menu = {
	5,
	{
		{"Dump FIRM", &menu_cb_dump_firm},
		{"Dump Bootrom", &menu_cb_dump_bootrom},
		{"Dump ARM9 RAM", &menu_cb_dump_arm9internal},
		{"Dump AXI WRAM", &menu_cb_dump_axiwram},
		{"Dump FCRAM", &menu_cb_dump_fcram},
		//{"Decrypt Loader", &menu_cb_decrypt_loader} // disabled for now
	}
};

void newline (int count) {
	int i;
	for(i=0; i<count; i++)
		Debug("");
}

void clear_top (void) {
    ClearScreen(TOP_SCREEN0, RGB(255, 255, 255));
    ClearScreen(TOP_SCREEN1, RGB(255, 255, 255));
    current_y = 0;
}

int print_progress(int oldstep, u32 val, u32 maxval) {
	char buf[256];
	char *p = (char *)&buf;
	int i;
	int curstep;
	const int maxstep = 10;

	curstep = (val * maxstep) / maxval;

	if (curstep > oldstep) {
		strcpy(p++, "[");
		for (i = 0; i < maxstep; i++) {
			snprintf(p++, sizeof(buf) - 1 - i, "%c", i < curstep ? '#' : ' ');
		}
		strncat(p, "]", sizeof(buf) - strlen(buf) - 1);
		clear_top();
		newline(1);
		Debug("Dumping %s %3d%%", buf, (val * 100 + maxval/2) / maxval);
	}
	return curstep > oldstep ? curstep : oldstep;
}

int ask_dump (char *what) {
    u32 key;

    clear_top();
    newline(2);
    Debug("Dump %s?", what);
    newline(1);
    Debug("---------------------------");
    newline(1);
    Debug("START : Confirm");
    Debug("B     : Abort");
    while (1) {
        key = wait_key("");
        if (key & BUTTON_START) {
            clear_top();
            newline(1);
            break;
        } else if (key & BUTTON_B) {
            clear_top();
            newline(1);
            Debug("Aborted");
            break;
        }
    }
    return (key & BUTTON_START);
}

int menu_cb_dump_bootrom (int idx, void *notused) {
	u32 result = 0;

	if (ask_dump("ARM9 RAM")) {
			result = dump_mem(PATH_BOOTROM, (void *)ARM9_BOOTROM, ARM9_BOOTROM_SIZE, &print_progress);
			newline(2);
			Debug("Done: %s!", result ? "success":"failure");
			result = 1;
	}
	return result;
}

int menu_cb_dump_arm9internal (int idx, void *notused) {
	u32 result = 0;

	if (ask_dump("ARM9 RAM")) {
			result = dump_mem(PATH_ARM9INTERNAL, (void *)ARM9_INTERNAL, ARM9_INTERNAL_SIZE, &print_progress);
			newline(2);
			Debug("Done: %s!", result ? "success":"failure");
			result = 1;	
	}
	return result;
}

int menu_cb_dump_fcram (int idx, void *notused) {
	u32 result = 0;

	if (ask_dump("FCRAM")) {
			result = dump_mem(PATH_FCRAM, (void *)ARM9_FCRAM, ARM9_FCRAM_SIZE, &print_progress);
			newline(2);
			Debug("Done: %s!", result ? "success":"failure");
			result = 1;
	}
	return result;
}

int menu_cb_dump_axiwram (int idx, void *notused) {
	u32 result = 0;

	if (ask_dump("AXI WRAM")) {
			result = dump_mem(PATH_AXIWRAM, (void *)ARM9_AXIWRAM, ARM9_AXIWRAM_SIZE, &print_progress);
			newline(2);
			Debug("Done: %s!", result ? "success":"failure");
			result = 1;
	}
	return result;
}

int menu_cb_dump_firm (int idx, void *notused) {
	u32 result = 0;
	char filename[256];
	u32 size;
	void *addr;

	/* TODO: fix. this might be problematic as it does not take
	   into account sections that are not 'properly' aligned */
	if (is_valid_firm()) {
		if (ask_dump("FIRM")) {
			newline(1);
			Debug("FIRM header:");
			Debug("%08X-%08X (%08X)", firm, (u32)firm + sizeof(firm_header_t), sizeof(firm_header_t));
			newline(1);
			dump_mem(PATH_FIRM_HEADER, (void *)firm, sizeof(firm_header_t), 0);
			for (int i=0; i<FIRM_MAX_SECTION_COUNT; i++) {

				snprintf((char *)&filename, sizeof(filename) - 1, PATH_FMT_FIRM_SECTION, i);
				addr = firm->section_headers[i].address;
				size = firm->section_headers[i].size;
			
				Debug("Section %d/%d:", i + 1, FIRM_MAX_SECTION_COUNT);
				if (addr && size) {
					Debug("%08X-%08X (%08X)", addr, addr + size, size); 
					if (!dump_mem((char *)&filename, addr, size, 0)) {
						Debug("[!] ERROR");
					}
				} else {
					Debug("Empty section: skipping");
				}
				newline(1);
			}
			result = 1;
		}
	} else {
		Debug("Invalid FIRM header. Please turn off console.");
		while (1)
			;
	}
	return result;
}

int menu_cb_decrypt_loader (int idx, void *param) {
	u32 result = 0;
	loader_info_t *li = param;

	clear_top();
	newline(2);

	if (GetUnitPlatform() == PLATFORM_N3DS) {
		if (li) {
			if (li->is_supported) {
				if (!li->is_decrypted) {
					Debug("This is an experimental feature.");
					newline(2);
					if (wait_confirm_cancel("Attempt to decrypt loader?")) {
						if (patch_loader(li)) {
							Debug("loader patched");
							if (run_loader(li)) {
								Debug("loader returned");
							}
							restore_loader(li);
						}
						li->is_decrypted = 1;
					}
					else {
						Debug("Aborted!");
					}
				} else {
					Debug("Loader already decrypted!");
				}
			} else {
				Debug("Platform not supported (yet)!");
			}
		} else {
			Debug("Unknown error!");
		}
	} else {
		Debug("No loader on this platform!");
	}
	return result;
}

int print_menu (int idx, struct menu_t *menu) {
	int i;
	int newidx;
	int count = menu_get_element_count(menu);
	
	newidx = menu_update_index(idx, menu);

	for (i=0; i<count; i++) {
		if (newidx == i)
			Debug("[ %s ]", menu_get_element_name(i, menu));
		else
			Debug("  %s  ", menu_get_element_name(i, menu));
	}
	
	return newidx;	
}

int print_main_menu (int idx, struct menu_t *menu) {
	int newidx = 0;

	newline(1);
	Debug("%s", BRAHMA_TOOL_NAME);
	newline(2);
	Debug("===========================");
	newidx = print_menu(idx, menu);
	Debug("===========================");
	newline(1);
	Debug("A:     Confirm");
	Debug("START: Boot firmware");
	
	return newidx;
}
