/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>
int ssdmap[32];
char ssdcan[32];
int ssdcnt[32];

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
		temp = begin + off;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
		temp = DEV_DISK_OPERATION_READ;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
		panic_on(syscall_read_dev(dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
		if (temp == 0) {
			user_panic("ide_read failed!/n");
		}
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
		temp = begin + off;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
		panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));

		temp = DEV_DISK_OPERATION_WRITE;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));

		panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
		if (temp == 0) {
			user_panic("ide_write failed!\n");
		}

	}
}


void ssd_init() {
	int i;
	for(i = 0; i < 32; i++) {
		ssdmap[i] = -1;
		ssdcan[i] = 1;
		ssdcnt[i] = 0;
	}
}

int ssd_read(u_int logic_no, void *dst) {
	if(ssdmap[logic_no] == -1) {
		return -1;
	}
	ide_read(0, ssdmap[logic_no], dst, 1);
	return 0;
}

void ssd_write(u_int logic_no, void *src) {
	if (ssdmap[logic_no] != -1) {
		// has data
		ssd_erase(logic_no);
	}
	
	int new_pno;
	int less_cnt;
	less_cnt = 2000000;
	new_pno = -1;
	int i;
	for(i = 0; i < 32; i++) {
		if(ssdcan[i] && ssdcnt[i] < less_cnt) {
			less_cnt = ssdcnt[i];
			new_pno = i;
		}
	}

	if (new_pno == -1) {
		user_panic("No ssd!\n");
	}

	if (less_cnt >= 5) {
		int other_pno;
		less_cnt = 2000000;
		other_pno = -1;
		for(i = 0; i < 32; i++ ) {
			if(!ssdcan[i] && ssdcnt[i] < less_cnt) {
				less_cnt = ssdcnt[i];
				other_pno = i;
			}
		}

		if(other_pno == -1) {
			user_panic("No ssd2\n");
		}

		char temp_data[512];
		ide_read(0, other_pno, (void *)temp_data, 1);
		ide_write(0, new_pno, (void *)temp_data, 1);
		// ssdcan[new_pno] = 0;
		
		for(i = 0; i < 32; i++) {
			if(ssdmap[i] == other_pno) {
				ssd_erase(i);
				ssdmap[i] = new_pno;
				ssdcan[new_pno] = 0;
				break;
			}
		}

		//memset((void *)temp_data, 0, 512);
		//ide_write(0, other_pno, (void *)temp_data, 1);
		//ssdcnt[other_pno]++;
		//ssdcan[other_pno] = 1;

		new_pno = other_pno;
	}

	ide_write(0, new_pno, src, 1);
	ssdmap[logic_no] = new_pno;
	ssdcan[new_pno] = 0;
	return;
}

void ssd_erase(u_int logic_no) {
	if (ssdmap[logic_no] == -1) {
		return;
	}

	char empty[512];
	memset(empty, 0, 512);

	ide_write(0, ssdmap[logic_no], empty, 1);
	ssdcnt[ssdmap[logic_no]]++;
	ssdcan[ssdmap[logic_no]] = 1;
	ssdmap[logic_no] = -1;
}


