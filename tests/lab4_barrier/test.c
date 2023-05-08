#include <lib.h>

int main() {
	barrier_alloc(5);
	for (int i = 0; i < 9; i++) {
		int who = fork();
		if (who == 0) {
			debugf("I'm son!\n");
			//int whomm = fork();
			//if(whomm == 0) {
			//	debugf("Im ss\n");
			//	barrier_wait();
			//	syscall_panic("Wrong www\n");
			//}
			barrier_wait();
			debugf("OUT!\n");
			//syscall_panic("Wrong block!");
		}
	}
	debugf("I'm finished!\n");
	return 0;
}
