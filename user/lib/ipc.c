// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}
void ipc_broadcast(u_int val, void * srcva, u_int perm) {
	u_int id;
	id = syscall_getenvid();
	ipc_broadcast_send(id, val, srcva, perm);
}

void ipc_broadcast_send(u_int p_envid, u_int val, void * srcva, u_int perm) {
	int num = 0;
	struct Env *child_envs[10];
	int i;

	for(i = 0; i < NENV; i++) {
		if(envs[i].env_status != ENV_FREE && env[i].env_parent_id == p_envid) {
			child_envs[num++] = &envs[i];
		}
	}

	if(num == 0) {
		return;
	}

	for(i = 0; i < num; i++) {
		ipc_send(child_envs[i]->env_id, val, srcva, perm);
		ipc_broadcast_send(child_envs[i]->env_id, val, srcva, perm);
	}
}
// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

