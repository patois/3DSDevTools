#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "textmenu.h"
#include "interface.h"
#include "platform.h"
#include "arm9loader.h"
#include "firm.h"
#include "global.h"

void check_trigger (void) {
	u8 is_n3ds = (GetUnitPlatform() == PLATFORM_N3DS);

	if (ARM9_is_key_trigger) {
		clear_top();
		newline(1);
		Debug("%s", BRAHMA_TOOL_NAME);
		newline(4);

		if (is_n3ds) {
			if (ARM9_is_decrypted) {
				Debug("N3DS ARM9 binary successfully decrypted!");
				newline(1);
				Debug("ARM9 Loader EP %08X, OEP %08X", ARM9_EP, ARM9_OEP_bkp);
			} else {
				Debug("Decryption of the N3DS binary failed!");
				newline(1);
				Debug("Your platform has not yet been added");
				Debug("support for ARM9 binary decryption.");
				newline(1);
				if (is_valid_firm()) {
					Debug("Your FIRM header checksum is %08X",
						get_checksum((void *)firm, sizeof(firm_header_t)));
					Debug("Please get in touch with the author");
				}
			}
		} else { // old 3ds
			newline(1);
			Debug("Isn't it fun to be able to run code");
			Debug("on this neat little handheld?");
		}
		wait_any_key();
	}
}

int main () {
	InitFS();

	u32 pad_state;
	int menuidx = 0;

	//loader_info_t li;

	//result = init_loader_info(&li);
	//li.address = ARM9_EP;

	check_trigger();

	while (true) {
		clear_top();

		menuidx = print_main_menu(menuidx, &main_menu);		
		pad_state = InputWait();

		if (pad_state & BUTTON_START) {
			break;        	
		}
		else if (pad_state & BUTTON_A) {
			//menu_execute_function(menuidx, &main_menu, &li);
			if (menu_execute_function(menuidx, &main_menu, 0)) {
				wait_any_key();
			}
		}
		else if (pad_state & BUTTON_UP) {
			menuidx--;
		}
		else if (pad_state & BUTTON_DOWN) {
			menuidx++;
		}
	}
    
	DeinitFS();

	// return control to firmware
	return 0;
}