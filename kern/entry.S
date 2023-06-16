#include <asm/asm.h>
#include <stackframe.h>

LEAF(set_cp0_context)
.set noreorder
	mtc0 a0, CP0_CONTEXT
	jr ra
	nop
.set reorder
END(set_cp0_context)




.section .text.tlb_miss_entry
tlb_miss_entry:
	j direct_tlb_refill
	# j exc_gen_entry






.section .text.exc_gen_entry
exc_gen_entry:
	SAVE_ALL
/* Exercise 3.9: Your code here. */
	mfc0 t0, CP0_CAUSE
	andi t0, 0x7c
	lw t0, exception_handlers(t0)
	jr t0




.data
direct_refill_ra:
	.word 0
direct_refill_hi:
	.word 0
direct_refill_badv:
	.word 0
.text
direct_tlb_refill:
.set noreorder
.set noat
	mfc0 k0, CP0_EPC
	la k1, direct_refill_ra
	sw k0, 0(k1)

	mfc0 k0, CP0_ENTRYHI
	la k1, direct_refill_hi
	sw k0, 0(k1)

	mfc0 k0, CP0_BADVADDR
	la k1, direct_refill_badv
	sw k0, 0(k1)

	mfc0 k1, CP0_CONTEXT
	lw k1, 0(k1) # 如果此时再次发生TLB MISS，则发生异常重入会跳转到 0x80000080 进行处理，为此需要保存 EPC 等的值
	nop
	la k0, direct_refill_hi
	lw k0, 0(k0)
	mtc0 k1, CP0_ENTRYLO0
	mtc0 k0, CP0_ENTRYHI
	andi k1, k1, PTE_V
	beqz k1, direct_refill_invalid # 如果页表项无效则也需要转到 0x80000080 进行处理
	nop
	la k1, direct_refill_ra
	lw k1, 0(k1)
	nop
	tlbwr
	nop
	jr k1
	rfe
direct_refill_invalid:
	la k0, direct_refill_badv
	la k1, direct_refill_ra
	lw k0, 0(k0)
	lw k1, 0(k1)
	mtc0 k0, CP0_BADVADDR
	mtc0 k1, CP0_EPC
	j exc_gen_entry
	nop
.set at
.set reorder
