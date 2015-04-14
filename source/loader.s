.arm
.align 4
.code 32
.text

.global loader_return
.global loader_execute


@ running the ARM9 loader does not work
@ as I forgot about that it changes the
@ stack pointer :[
@ so I reverted to unpacking the loader in
@ start.s again..
loader_execute:
	stmfd sp!, {r0-r12, lr}
	bx r0
loader_return:
	ldmfd sp!, {r0-r12, lr}
	bx lr
