/* 
An I2C slave emulator using Red Pitaya digital output pins.
Author: Hunter Akins

Designed following the excellent article by Steve Kolokowsky, found here:
http://www.cypress.com/file/73106/download


*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <redpitaya/rp.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define ADDRESS	0x66
#define TRANSFER_RATE 400000 // 400kHz transfer rate
#define I2C_START_DETECTED



int PrintTime(struct timespec start);

int main() {
	printf("here");
	if (rp_Init() < 0) {
		printf("error with initialization\n");
	}

	// initialize pins
	rp_dpin_t SCL;
	rp_dpin_t SDA;
	SDA = RP_DIO1_P;
	SCL = RP_DIO2_P;
	//listen to master
	rp_DpinSetDirection(SDA, RP_IN);
	rp_DpinSetDirection(SCL, RP_IN);
	
	
	rp_pinState_t SDA_state;
	rp_pinState_t SCL_state;
	char retval = '\0';

	rp_DpinGetState(SDA, &SDA_state);
	rp_DpinGetState(SCL, &SCL_state);
	

	// wait for START call
	while (true) {
		rp_DpinGetState(SDA, &SDA_state);
		if (SDA_state == RP_LOW) {	
			struct timespec start;
			clock_gettime(CLOCK_MONOTONIC, &start);
			rp_DpinGetState(SCL, &SCL_state);
			if (SCL_state == RP_HIGH) {
				retval = 's'; 
				printf("%c", retval);
				break;
			}
			PrintTime(start);	
			printf("checking\n");
			fflush(stdout);
		}
	}
	

	// here I check if the address is right, if it's not, I go back to the waiting

	// if i have the right address, check it master wants to read or write
	
	// if master wants write, then listen 

	// if master wants to read, then write

	// 
	rp_Release();

	return 0;
}

	


void Start_ISR(void) {
	bool retval;   // 1 if start, 0 if positive
	bool readmode; // 0 if 
	
	rp_DpinGetState(SCL, &SCL_state);
	if (SCL == RP_HIGH) {
		retval = 1;
	}
	
	else {
		// false alarm
		return; 
	}
	
	while (retval == 1);
		retval = readAddress(1); //check address, return 1 if correct
		if (retval == 0) {
			break;
		}
		

	}

int readAddress(int i) {
	char i;
			





//abstraction for the timer function. Start is declared at the beginning. This will compute the time elasped since then and print it to stdout
int PrintTime(struct timespec start){
	long elapsedtime;
	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsedtime = end.tv_nsec - start.tv_nsec;
	printf("Time elapsed = ");
	printf("%lu\n", elapsedtime);
	return 0;
}
