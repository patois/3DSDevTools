#include "common.h"
#include "dumper.h"
#include "fs.h"
#include "firm.h"

// TODO: add callback argument for progress bar
// TODO: implement progress bar :)
u32 dump_mem (const char *filename, void *addr, u32 size, progress_printer_t *f) {
	u32 written = 0;
	u32 total = 0;
	u32 result = 0;
	u32 num = 0;
	const u32 sz_chunk = 0x10000;
	int step = -1;
	
	if (FileCreate(filename, true)) {
		while (total < size) {
			num = size - total < sz_chunk ? size - total : sz_chunk;
			written = FileWrite((u8 *)addr + total,
				num,
				total);

			if (written != num) {
				break;
			}
			total += written;
			if (f) {
				step = f(step, total/100, size/100);
			}
		}
		FileClose();
		result = (size == total);
	}	
	return result;	
}
