#include <lib.h>

#define MAX_SIZE (1 << 16)

int test[MAX_SIZE];

int main() {
	int s_start, s_end, s;
	int us_start, us_end, us;
	int i, j, k;

	for(k = 0; k < 3; k++) {
		debugf("round %d :\n", k + 1);
		s_start = (int)get_time((u_int *)&us_start);
		debugf("	tlb fast refill start at : %d.%06d\n", s_start, us_start);

		for(i = 0; i < 50000 * k + 1; i++) {
			for(j = 0; j < MAX_SIZE; j += 1024) {
				test[j]++;
			}
		}


		s_end = (int)get_time((u_int *)&us_end);
		s = s_end - s_start;
		us = us_end - us_start;
		if (us < 0) {
			s--;
			us += 1000000;
		}

		debugf("	tlb fast refill test end at : %d.%06d\n", s_end, us_end);
		debugf("	take up : %d.%06d\n", s, us);
	}
	return 0;
}
