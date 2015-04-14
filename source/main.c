#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "textmenu.h"
#include "interface.h"
#include "arm9loader.h"

//extern u32 entry_point_arm9;
extern u32 is_arm9_decrypted;

int main () {
	InitFS();

	u32 pad_state;
	int menuidx = 0;

	if (is_arm9_decrypted) {
		clear_top();
		newline(1);
		Debug("N3DS ARM9 binary successfully decrypted!");
		newline(2);
		Debug("Hold 'Y' during startup if you do not");
		Debug("want the ARM9 binary to be decrypted");
		wait_any_key();
	}
	//loader_info_t li;

	//result = init_loader_info(&li);
	//li.address = entry_point_arm9;

    while (true) {
		clear_top();

		menuidx = print_main_menu(menuidx, &main_menu);		
        pad_state = InputWait();
        
		if (pad_state & BUTTON_START) {
        	break;        	
        }
        else if (pad_state & BUTTON_A) {
			//menu_execute_function(menuidx, &main_menu, &li);
			menu_execute_function(menuidx, &main_menu, 0);
			wait_any_key();
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