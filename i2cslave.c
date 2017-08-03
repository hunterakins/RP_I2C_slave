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
#include <i2cslave.h>

#define I2C_SLAVE_ADDRESS	0x66
#define TRANSFER_RATE 400000 // 400kHz transfer rate


void start_ISR(rp_dpin_t SDA, rp_dpin_t SCL, rp_pinState_t SDA_state, rp_pinState_t SCL_state, struct timespec * beg);
int readByte(bool addrMatch, rp_dpin_t SDA, rp_dpin_t SCL, rp_pinState_t SDA_state, rp_pinState_t SCL_state, struct timespec * beg);
int PrintTime(struct timespec start);


int main() {
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

	rp_DpinGetState(SDA, &SDA_state);
	rp_DpinGetState(SCL, &SCL_state);
	

	// wait for START call

	while (1) {
		// while SDA is high
		while (SDA_state) {
			rp_DpinGetState(SDA, &SDA_state);
		}	
		// now SDA is low, check if we have a start condition
		struct timespec beg;
		clock_gettime(CLOCK_MONOTONIC, &beg);
		printf("starting interrupt");
		fflush(stdout);
		start_ISR(SDA, SCL, SDA_state, SCL_state, &beg);
	}	

	// here I check if the address is right, if it's not, I go back to the waiting

	// if i have the right address, check it master wants to read or write
	
	// if master wants write, then listen 

	// if master wants to read, then write

	// 
	rp_Release();

	return 0;
}

int readByte(bool addrMatch, rp_dpin_t SDA, rp_dpin_t SCL, rp_pinState_t SDA_state, rp_pinState_t SCL_state, struct timespec * start);

void start_ISR(rp_dpin_t SDA, rp_dpin_t SCL, rp_pinState_t SDA_state, rp_pinState_t SCL_state, struct timespec *beg) {
	char retval;   // 1 if start, 0 if positive

	i2c_trig_t start;
	start = I2C_START_DETECTED;	
	rp_DpinGetState(SCL, &SCL_state);
	PrintTime(*beg);
	if (SCL_state) {
	 	retval = start;
	}
	
	else {
		// false alarm
		return; 
	}
	
	while (retval == start) { //last bit s a 1
		/*
		printf("reading byte\n");
		fflush(stdout);
		printf("retval\t");
		printf("%d\n", retval);
		*/
		retval = readByte(1, SCL, SCL_state, SDA, SDA_state, beg); //check address, return 1 if correct
		if ((retval & 0xfe) == I2C_ADDRESS_MISMATCH) {
			break;
		}
		/*
		if (retval & 1) {	
			retval = sendData();
		}
		else {
			retval = receiveData();
		}
		*/	
	return;
	}
}

// if addrMatch is 1 then I am reading an address byte 

int readByte(bool addrMatch, rp_dpin_t SDA, rp_dpin_t SCL, rp_pinState_t SDA_state, rp_pinState_t SCL_state, struct timespec * beg) {
	PrintTime(*beg);
	clock_gettime(CLOCK_MONOTONIC, beg);
	rp_DpinGetState(SCL, &SCL_state);
	PrintTime(*beg);
	printf("clock state: ");
	printf("%d\n", SCL_state);
	char j;
	char retval;	
	for (j = 0; j < 8; j++) {
		rp_DpinGetState(SCL, &SCL_state);
		//waiting for clock to come high, signaling it is time write 
		while (!SCL_state) {
			rp_DpinGetState(SCL, &SCL_state);
		}
		// now it's time to read;
		rp_DpinGetState(SDA, &SDA_state);
		// read the transmitted bit
		printf("%d\t", SDA_state);
		fflush(stdout);
		retval = (retval << 1) | SDA_state;
		// now that SCL is high, I need to monitor for a STOP bit or a START bit
		rp_DpinGetState(SCL, &SCL_state);
		while (SCL_state) {
			rp_DpinGetState(SDA, &SDA_state);
			// retval & 1 gives us the last SDA state
			if ((retval & 1) != SDA_state) {
				// if they are not equal, and SDA is high, that means I went low to high, which is the STOP condition
				return ((SDA_state) ? (I2C_STOP_DETECTED) : (I2C_START_DETECTED));
			}
		}
	}
	printf("retval:\t");
	printf("%d\n", retval);
	fflush(stdout);
	// now SCL is low, need ot send back my ACK bit

	if (!addrMatch || ((retval & 0xfe) == I2C_SLAVE_ADDRESS)) {
		rp_DpinSetDirection(SDA, RP_OUT);
		rp_DpinSetState(SDA, RP_LOW); // to ACK 
		rp_DpinGetState(SCL, &SCL_state);
		while (!SCL) {
			rp_DpinGetState(SCL, &SCL_state);
		}
		rp_DpinSetDirection(SDA, RP_IN);
		
	}
	else {
		return I2C_ADDRESS_MISMATCH;
	}
	
	return retval;
}	
		
			





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
