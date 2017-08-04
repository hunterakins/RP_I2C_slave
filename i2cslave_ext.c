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


int readByte(bool addrMatch);
int PrintTime(struct timespec start);

rp_dpin_t SCL;
rp_dpin_t SDA;

rp_pinState_t SDA_state;
rp_pinState_t SCL_state;

struct timespec beg;
int main() {
	int i;
	if (rp_Init() < 0) {
			printf("error with initialization\n");
	}

	// initialize pins
	SDA = RP_DIO1_P;
	SCL = RP_DIO2_P;
	//listen to master
	rp_DpinSetDirection(SDA, RP_IN);
	rp_DpinSetDirection(SCL, RP_IN);
	
			
	
	rp_DpinGetState(SDA, &SDA_state);
	rp_DpinGetState(SCL, &SCL_state);
	


	// wait for START call

	while (1) {
		// while SDA is high
		while (SDA_state) {
			rp_DpinGetState(SDA, &SDA_state);
		}	
		char retval;   // 1 if start, 0 if positive
		rp_DpinGetState(SCL, &SCL_state);
		i2c_trig_t start; 
		start = I2C_START_DETECTED;
		// if SCL is high then it's a start condition
		if (SCL_state) {
			retval = start;
		}
		
		else {
			// false alarm
			retval = 0; 
		}
		while (retval == start) { //last bit s a 1
			// let the clock drop down
			while (SCL_state) {
				rp_DpinGetState(SCL, &SCL_state);
			}
			printf("SCL state before for loop :");
			printf("%d\n", SCL_state);
			// now the clock is down, got to read the address 
			rp_DpinGetState(SDA, &SDA_state);
			printf("SDA state: ");
			printf("%d\n", SDA_state);
			for (i = 0; i < 7; i++) {
				while(SCL_state) {
					rp_DpinGetState(SCL, &SCL_state);
				}
				while (!SCL_state) {
					rp_DpinGetState(SCL, &SCL_state);
				}
				rp_DpinGetState(SDA, &SDA_state);
				printf("%d\n", i);
				fflush(stdout);
				rp_DpinGetState(SCL, &SCL_state);
			}
			printf("\n");
			retval = readByte(1); //check address, return 1 if correct
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

// if addrMatch is 1 then I am reading an address byte 

int readByte(bool addrMatch) {
	rp_DpinGetState(SCL, &SCL_state);
	char j;
	char retval;	
	printf("%d\n", SCL_state);
	for (j = 0; j < 8; j++) {
		rp_DpinGetState(SCL, &SCL_state);
		//waiting for clock to come high, signaling it is time for transfer
		while (!SCL_state) {
			rp_DpinGetState(SCL, &SCL_state);
		}
		// now it's time to read;
		rp_DpinGetState(SDA, &SDA_state);
		// read the transmitted bit
		retval = (retval << 1) | SDA_state;
		// now that SCL is high, I need to monitor for a STOP bit or a START bit
		rp_DpinGetState(SCL, &SCL_state);
		printf("%d\t", SDA_state);
		/*
		while (SCL_state) {
			rp_DpinGetState(SCL, &SCL_state);
			rp_DpinGetState(SDA, &SDA_state);
			// retval & 1 gives us the last SDA state
			if ((retval & 1) != SDA_state) {
				// if they are not equal, and SDA is high, that means I went low to high, which is the STOP condition
				return ((SDA_state) ? (I2C_STOP_DETECTED) : (I2C_START_DETECTED));
			}
		}
		*/
	}
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
	fflush(stdout);
	return 0;
}
