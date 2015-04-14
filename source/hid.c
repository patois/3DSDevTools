#include "draw.h"
#include "hid.h"
#include "interface.h"

u32 InputWait (void) {
    u32 pad_state_old = HID_STATE;
    while (true) {
        u32 pad_state = HID_STATE;
        if (pad_state ^ pad_state_old)
            return ~pad_state;
    }
}

u32 wait_key (char *msg) {
	if (msg) {
		Debug("");
		Debug("%s", msg);
	}
	return InputWait();
}

void wait_any_key (void) {
    newline(2);
    wait_key("Press key to continue");
    InputWait();
}

u32 wait_confirm_cancel (char *msg) {
    u32 key;

    Debug("%s", msg);
    newline(1);
    Debug("---------------------------");
    newline(1);
    Debug("START : Confirm");
    Debug("B     : Abort");
    while (1) {
        key = wait_key("");
        if (key & BUTTON_START) {
            break;
        } else if (key & BUTTON_B) {
            break;
        }
    }
    return (key & BUTTON_START);
}

// TODO
/*
u32 wait_key_mask (u32 mask) {
    u32 key;
    while (1) {
        key = wait_key("");
        if (key & BUTTON_START) {
            clear_top();
            newline(1);
            Debug("Do NOT turn off your console!");
            break;
        } else if (key & BUTTON_B) {
            clear_top();
            newline(1);
            Debug("Aborted");
            break;
        }
    }
}
*/
