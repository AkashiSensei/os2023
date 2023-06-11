#include <asm/asm.h>
#include <stackframe.h>


.extern direct_refill_badv
.data
fast_refill_ra:
	.word 0

.text
NESTED(fast_tlb_refill, 0, zero)
.set noreorder
    mfc0 a0, CP0_BADVADDR # 保存失配虚地址映射的表项的指针，kseg2
    srl a0, a0, 12
    sll a0, a0, 2
    li k0, K2VPT
    addu a0, a0, k0
    mfc0 s0, CP0_ENTRYHI # 上下文保存并不保存 ENTRYHI
    li k0, K2VPDE
    beq k0, a0, fast_get_pgdir_pa # 如果要读入的是页目录所在页的映射关系，则跳转
    nop

    lw v0, 0(a0) # 读入相应页表项，可能产生异常重入
    j fast_valid_check
    nop

fast_get_pgdir_pa:
    sw ra, fast_refill_ra
    jal _get_pgdir_pa # 使用内核虚地址获取页目录物理地址，不会发生异常重入
    nop
    lw ra, fast_refill_ra
    # 此时 v0 内容为待填充的表项内容，转换自页目录所在页的内核虚地址
    j fast_refill
    nop

fast_valid_check: # 按理说来到这里成功读入页目录项的应该只可能是无效的页目录项，但保险起见还是做一下判断
    andi k0, v0, PTE_V
    bnez k0, fast_refill # 如果页表项有效位置位，则不进行分配
    nop
    sw ra, fast_refill_ra
    jal _fast_alloc # 使用内核虚地址，不会发生异常重入
    nop
    lw ra, fast_refill_ra
    # 此时 v0 为待填充的表项内容，获取自新分配的页

fast_refill:
    mtc0 s0, CP0_ENTRYHI # 从上下文的 s0 中恢复 CP0_ENTRYHI
    mtc0 v0, CP0_ENTRYLO0
    nop
    tlbwr
    nop
fast_end:
    jr ra
    nop
.set reorder
END(fast_tlb_refill)








LEAF(tlb_out)
.set noreorder
	mfc0    t0, CP0_ENTRYHI
	mtc0    a0, CP0_ENTRYHI
	nop
	/* Step 1: Use 'tlbp' to probe TLB entry */
	/* Exercise 2.8: Your code here. (1/2) */
	tlbp
	
	nop
	/* Step 2: Fetch the probe result from CP0.Index */
	mfc0    t1, CP0_INDEX
.set reorder
	bltz    t1, NO_SUCH_ENTRY
.set noreorder
	mtc0    zero, CP0_ENTRYHI
	mtc0    zero, CP0_ENTRYLO0
	nop
	/* Step 3: Use 'tlbwi' to write CP0.EntryHi/Lo into TLB at CP0.Index  */
	/* Exercise 2.8: Your code here. (2/2) */
	tlbwi

.set reorder

NO_SUCH_ENTRY:
	mtc0    t0, CP0_ENTRYHI
	j       ra
END(tlb_out)

.data
tlb_refill_ra:
.word 0
.text
NESTED(do_tlb_refill, 0, zero)
	mfc0    a0, CP0_BADVADDR
	mfc0    a1, CP0_ENTRYHI
	srl     a1, a1, 6
	andi    a1, a1, 0b111111
	sw      ra, tlb_refill_ra
	jal     _do_tlb_refill
	lw      ra, tlb_refill_ra
	mtc0    v0, CP0_ENTRYLO0
	// See <IDT R30xx Family Software Reference Manual> Chapter 6-8
	nop
	/* Hint: use 'tlbwr' to write CP0.EntryHi/Lo into a random tlb entry. */
	/* Exercise 2.10: Your code here. */
	tlbwr

	jr      ra
END(do_tlb_refill)
