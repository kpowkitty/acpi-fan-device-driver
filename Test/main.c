/**************************************************************
* GitHub-Name:: kpowkitty
*
* File:: powell_kat_HW6_main.c
*
* Description:: User application to test functionality of my
* ACPI Virtualized Device Driver. Tests all functions.
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/ioctl.h>

/* Path to device driver. */
#define DEVICE_PATH "/dev/virtfans"

/* Return codes. */
#define OPEN_FAILURE -1
#define READ_FAILURE -2
#define WRITE_FAILURE -3
#define SUCCESS 0

#define NUM_OF_STATES 5

typedef struct fan_fst {
        uint32_t revision;
        uint32_t control;  
        uint32_t speed;                                                         
} fan_fst;

struct fan_fsp {
        uint32_t level;
        uint32_t trip_point;
        uint32_t speed;
        uint32_t noise_level;
        uint32_t power;
};

typedef struct fsp_table {
        uint32_t size;
        struct fan_fsp fsp[NUM_OF_STATES];
} fsp_table;

#define FAN_MAGIC 'f'

#define FAN_IOC_GET_FST _IOR(FAN_MAGIC, 0, fan_fst)
#define FAN_IOC_GET_FPSP _IOR(FAN_MAGIC, 0, fsp_table)
#define FAN_IOC_SET_FSL _IOW(FAN_MAGIC, 1, uint32_t)

int main() {
	int fd = open(DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		perror("Failed to open device");
		return OPEN_FAILURE;
	}



	uint64_t fan_state;
	ssize_t n = read(fd, &fan_state, sizeof(fan_state));
	if (n != sizeof(fan_state)) {
		perror("Read failed");
		return READ_FAILURE;
	}
	
	printf("fan_state: %llu.\n", (unsigned long long)fan_state);

	uint64_t new_state = 1;
	n = write(fd, &new_state, sizeof(new_state));
	if (n != sizeof(new_state)) {
		perror("Write failed");
		return WRITE_FAILURE;
	}

	uint64_t read_state;
	n = read(fd, &read_state, sizeof(read_state));
	if (n != sizeof(read_state)) {
		perror("Read failed");
		return READ_FAILURE;
	}

	printf("new fan_state: %llu.\n", (unsigned long long)read_state);

	struct fan_fst my_fst;
	
	if (ioctl(fd, FAN_IOC_GET_FST, &my_fst) < 0) {
		perror("Failed to get fst.");
		close(fd);
		return READ_FAILURE;
	}

	printf("my_fst.revision: %llu.\n", (unsigned long long)my_fst.revision);
	printf("my_fst.control: %llu.\n", (unsigned long long)my_fst.control);
	printf("my_fst.speed: %llu.\n", (unsigned long long)my_fst.speed);

	struct fsp_table my_table = {0};

	if (ioctl(fd, FAN_IOC_GET_FPSP, &my_table) < 0) {
		perror("Failed to get fpsp.");
		close(fd);
		return READ_FAILURE;
	}

	for (int i = 0; i < NUM_OF_STATES; ++i) {
		printf("fsp: %d.\n", i);
		printf("fsp[%d].level: %d.\n", i, my_table.fsp[i].level);
		printf("fsp[%d].trip_point: %d.\n", i, my_table.fsp[i].trip_point);
		printf("fsp[%d].speed: %d.\n", i, my_table.fsp[i].speed);
		printf("fsp[%d].noise_level: %d.\n", i, my_table.fsp[i].noise_level);
		printf("fsp[%d].power: %d.\n", i, my_table.fsp[i].power);
	}                                                       
                                                                
	uint32_t FPS = 3;                                          
	if (ioctl(fd, FAN_IOC_SET_FSL, &FPS) < 0) {             
		perror("Failed to set _FSL.");
		close(fd);
		return WRITE_FAILURE;
	}
	
	struct fan_fst new_fst;

	if (ioctl(fd, FAN_IOC_GET_FST, &new_fst) < 0) {
		perror("Failed to get fst.");
		close(fd);
		return READ_FAILURE;
	}

	printf("my_fst.revision: %llu.\n", (unsigned long long)new_fst.revision);
	printf("my_fst.control: %llu.\n", (unsigned long long)new_fst.control);
	printf("my_fst.speed: %llu.\n", (unsigned long long)new_fst.speed);
	
	close(fd);
	return SUCCESS;
}
