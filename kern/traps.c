#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
    [12] = handle_ov,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	curenv->env_ov_cnt++;

	u_long user_bad_vpc = tf->cp0_epc;
	struct Page *bad_pc_page = page_lookup(curenv->env_pgdir, user_bad_vpc, NULL);
	assert(bad_pc_page != NULL);
	u_long user_bad_kvpc = KADDR(page2pa(bad_pc_page) | (user_bad_vpc & 0xFFF));
	u_int user_bad_ins = *(u_int *)user_bad_kvpc;
	// printk("%d\n", user_bad_ins);
	u_int t_inx = (user_bad_ins >> 16) & 0x1F;
	u_int s_inx = (user_bad_ins >> 21) & 0x1F;
	if ((user_bad_ins & 0xFC000000) == 0x20000000) {
		// addi
		printk("addi ov handled\n");
		u_int imm = user_bad_ins & 0xFFFF;
		u_int result;
		result = (tf->regs[s_inx] / 2) + (imm / 2);
		tf->regs[t_inx] = result;
		tf->cp0_epc += 4;
	} else if((user_bad_ins & 0x0000000F) == 0) {
		printk("add ov handled\n");
		*(u_int *)user_bad_kvpc = user_bad_ins | 1;
	} else {
		printk("sub ov handled\n");
		*(u_int *)user_bad_kvpc = user_bad_ins | 1;
	}

}
