.section ".init"

.global _start
.global ARM9_is_decrypted
.global ARM9_is_key_trigger
.global ARM9_EP
.global ARM9_OEP_bkp
.global ARM9_OEP_chk_bkp

.extern main

.align 4
.arm


_start:
    b _init

	@ required, don't move :)	
	ARM9_EP:              .long 0xFFFF0000
	
	ARM9_is_decrypted:    .long 0
	ARM9_is_key_trigger:  .long 0
	
	@ backup of ARM9 binary Original Entry Point
	ARM9_OEP_bkp:         .long 0
	@ backup of code which is going to be patched
	ARM9_OEP_chk_bkp:     .long 0

	HID_PAD:              .long 0x10146000
	PDN_MPCORE_CFG:       .long 0x10140FFC
	ARM_NOP:              .long 0xE1A00000
	MAPPED_FIRM:          .long 0x24000000	
	
	@ Firmware specific data (structure)
	@ +0 checksum (see tools/calcchk.py)
	@ +4 pointer to ARM9 loader variable which
	@    contains ARM9 binary's OEP
	@ +8 pointer to ARM9 loader code which
	@    verifies whether OEP falls within
	@    a particular range
	
	firm_specific_data:
	.long 0x3FE0D972 @ N3DS 9.0.0.20E
	.long 0x08091580 @ p_oep
	.long 0x0808C428 @ p_oep_chk
	
	.long 0 @ terminate array of struct


_init:
	stmfd sp!, {r0-r12, lr}

	bl fix_firm_hdr

	@ attempt decryption only if
	@ 'Y' is held during startup
	ldr r0, HID_PAD
	ldr r0, [r0]
	ands r0, #0x800 @ BUTTON_Y
	bne j_main

	mov r0, #1
	str r0, ARM9_is_key_trigger

	bl is_n3ds
	cmp r0, #0
	beq j_main
	
	bl is_kernel_supported
	cmp r0, #1
	beq unpack_arm9_ldr
	

j_main:	
	ldmfd sp!, {r0-r12, lr}
	b _main
		
unpack_arm9_ldr:	
	bl patch_n3ds_ldr
	
	ldmfd sp!, {r0-r12, lr}

	@ execute N3DS ARM9 loader (decrypts ARM9 binary in memory)
	ldr pc, ARM9_EP
	@ ARM9 loader continues execution here
cont:

	stmfd sp!, {r0-r12, lr}
	bl restore_n3ds_ldr
	mov r1, #1
	str r1, ARM9_is_decrypted
	ldmfd sp!, {r0-r12, lr}

_main:
	stmfd sp!, {r0-r12, lr}
	bl main
	
	ldmfd sp!, {r0-r12, lr}
	
	@ return control to ARM9	
	ldr r2, ARM9_OEP_bkp @ jmp to ARM9 OEP (address of decrypted ARM9 code)
	cmp r2, #0x0
	ldreq r2, ARM9_EP
	bx r2

is_n3ds:
	ldr r0, PDN_MPCORE_CFG
	ldrb r0, [r0]
	cmp r0, #0x07
	moveq r0, #1
	movne r0, #0
	bx lr

fix_firm_hdr:
	@ fix FIRM header (restore ARM9 EP)
	ldr r0, MAPPED_FIRM
	ldr r1, ARM9_EP
	str r1, [r0, #0xC]
	bx lr	
	
patch_n3ds_ldr:
	stmfd sp!, {r10, lr}
	
	bl get_kernel_info_idx
	cmp r0, #-1
	beq _lp
	
	adr r10, firm_specific_data
	add r10, r10, r0, lsl#2 @ r10 points to struct for our kernel
	
	@ patch N3DS ARM9 loader OEP
	ldr r0, [r10,#4] @ p_oep
	ldr r1, [r0]
	str r1, ARM9_OEP_bkp
	adr r2, cont
	str r2, [r0]
	
	@ patch N3DS ARM9 loader OEP check
	ldr r0, [r10,#8] @ p_oep_chk
	ldr r1, [r0]
	str r1, ARM9_OEP_chk_bkp
	ldr r2, ARM_NOP
	str r2, [r0]

_lp:	
	ldmfd sp!, {r10, lr}
	bx lr
	
restore_n3ds_ldr:
	stmfd sp!, {lr}
	
	bl get_kernel_info_idx
	cmp r0, #-1
	beq _lr
	
	adr r2, firm_specific_data
	
	@ r2 points to struct for our arm9 loader version
	add r2, r2, r0, lsl#2

	@ fix N3DS ARM9 loader (restore OEP)
	ldr r0, [r2,#4] @ p_oep
	ldr r1, ARM9_OEP_bkp
	str r1, [r0]
	
	@ fix N3DS ARM9 loader (restore OEP check)
	ldr r0, [r2,#8] @ p_oep_chk
	ldr r1, ARM9_OEP_chk_bkp
	str r1, [r0]

_lr:	
	ldmfd sp!, {lr}
	bx lr

is_kernel_supported:
	stmfd sp!,{lr}
	bl get_kernel_info_idx
	cmp r0, #-1
	moveq r0, #0
	movne r0, #1
	ldmfd sp!,{lr}
	bx lr

calc_hdr_checksum:
	stmfd sp!,{r3}
	mov r0, #0
	mov r3, #0x100
	ldr r1, MAPPED_FIRM

next_word:
	subs r3, r3, #4
	ldrpl r2, [r1, r3]
	addpl r0, r0, r2
	bpl next_word

	ldmfd sp!,{r3}
	bx lr

get_kernel_info_idx:
	stmfd sp!,{r3,lr}
	
	bl calc_hdr_checksum
	mov r1, r0
	adr r0, firm_specific_data

	mov r3, #0	
next:
	ldr r2, [r0] @ get checksum from struct
	cmp r2, #0
	moveq r3, #-1
	beq leave
	
	cmp r2, r1
	beq leave
	
	add r0, r0, #0x0C
	add r3, r3, #1
	b next 
	
leave:
	mov r0, r3
	
	ldmfd sp!,{r3,lr}
	bx lr
