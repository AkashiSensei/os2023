#include <lib.h>

int main(int argc, char **argv) {
	u_int us;
	u_int s;
	s = get_time(&us);
	debugf("%d %d\n", s, us);
	return 0;
}
