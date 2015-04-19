#pragma once

#include "dumper.h"
#include "textmenu.h"

#define BRAHMA_TOOL_NAME "3DS DEVELOPMENT TOOLS"

void clear_top (void);
int ask_dump (char *what);
void newline (int count);
int menu_cb_dump_bootrom (int idx, void *notused);
int menu_cb_dump_arm9internal (int idx, void *notused);
int menu_cb_dump_fcram (int idx, void *notused);
int menu_cb_dump_axiwram (int idx, void *notused);
int menu_cb_dump_firm (int idx, void *notused);
int menu_cb_decrypt_loader(int idx, void *notused);
int print_menu (int idx, struct menu_t *menu);
int print_main_menu (int idx, struct menu_t *menu);
int dump_cb_print_progress(int oldstep, u32 val, u32 maxval);

extern struct menu_t main_menu;
