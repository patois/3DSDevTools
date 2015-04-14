#include "firm.h"
#include "draw.h"

int get_section_by_address (void *address) {
	int i;
	firm_section_header_t volatile *sh;
	
	for (i=0; i < FIRM_MAX_SECTION_COUNT; i++) {
		sh = &firm->section_headers[i];
		if ((sh->address <= address) && (address < (sh->address + sh->size))) {
		   return i;
		}
	}
	return -1;
}

/* this could/should be added a few more checks */
int is_valid_firm (void) {
	return (firm->magic == FIRM_MAGIC);
}

void dump_section_header (int index) {
	firm_section_header_t volatile *sh;
	
	sh = &firm->section_headers[index];
	Debug("Section %02d/%02d (ARM%s):", index, FIRM_MAX_SECTION_COUNT, sh->type ? "11" : "9");
	Debug("%08X - %08X", sh->address, sh->address + sh->size);
	Debug("");
	return;
}
