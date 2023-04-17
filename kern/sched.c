#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
	static int user_time[5] = {0};
	// static Env * last_env[5] = {NULL};
	/* We always decrease the 'count' by 1.
	 *
	 * If 'yield' is set, or 'count' has been decreased to 0, or 'e' (previous 'curenv') is
	 * 'NULL', or 'e' is not runnable, then we pick up a new env from 'env_sched_list' (list of
	 * all runnable envs), set 'count' to its priority, and schedule it with 'env_run'. **Panic
	 * if that list is empty**.
	 *
	 * (Note that if 'e' is still a runnable env, we should move it to the tail of
	 * 'env_sched_list' before picking up another env from its head, or we will schedule the
	 * head env repeatedly.)
	 *
	 * Otherwise, we simply schedule 'e' again.
	 *
	 * You may want to use macros below:
	 *   'TAILQ_FIRST', 'TAILQ_REMOVE', 'TAILQ_INSERT_TAIL'
	 */
	/* Exercise 3.12: Your code here. */
	int has_ready_env[5] = {0};
	struct Env * env_i;
	TAILQ_FOREACH(env_i, &env_sched_list, env_sched_link) {
		has_ready_env[env_i->env_user] = 1;
	}

	if(yield || count <= 0 || e == NULL || e->env_status != ENV_RUNNABLE) {
		if(e != NULL && e->env_status == ENV_RUNNABLE) {
			TAILQ_REMOVE(&env_sched_list, e, env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);
			user_time[e->env_user] += e->env_pri;
		}
		if(TAILQ_EMPTY(&env_sched_list)) {
			panic("schedule: no runnable envs\n");
		}
		

		int user_next = -1, min_time = 100000000;
		for(int i = 0; i < 5; i++) {
			if (has_ready_env[i] == 1 && user_time[i] < min_time) {
				user_next = i;
				min_time = user_time[i];
			}
		}
		assert(user_next != -1);
		/*
		env_i = last_env[user_next];
		if(env_i != NULL) {
			env_i = TAILQ_NEXT(env_i, env_sched_link);
		}

		if(env_i == NULL) {
			env_i = TAILQ_FIRST(&env_sched_list);
		}

		e = NULL;
		for(; env_i; env_i = env_i->env_sched_link.tqe_next) {
			if(env_i.env_user == user_next) {
				e = env_i;
				break;
			}
		}
		if(e == NULL) {
			TAILQ_FOREACH(env_i, &env_sched_list, env_sched_link) {
				if(env_i.env_user == user_next) {
					e = env_i;
					break;
				}
			}
		}
		*/
		TAILQ_FOREACH(env_i, &env_sched_list, env_sched_link) {
			if(env_i->env_user == user_next) {
				e = env_i;
				break;
			}
		}
		assert(e != NULL);
		count = e->env_pri;
	}
	count--;
	env_run(e);

}
