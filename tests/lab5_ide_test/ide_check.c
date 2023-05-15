#include "../../fs/serv.h"
#include <lib.h>

void test_ide() {
	int data[4096], read[4096];
	int i;
	for (i = 0; i < 4096; i++) {
		data[i] = i;
	}
	ide_write(0, 4096, data, 32);
	ide_read(0, 4096, read, 32);
	for (i = 0; i < 4096; i++) {
		if (data[i] != read[i]) {
			user_panic("ide read/write is wrong");
		}
	}
	debugf("TEST IDE read/write test passed!\n");
}

int main() {
	test_ide();
	return 0;
}
