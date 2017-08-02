#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <redpitaya/rp.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	printf("here");
	if (rp_Init() < 0) {
		printf("error with initialization\n");
	}
	
	rp_dpin_t output;
	rp_dpin_t input;
	output = RP_DIO1_P;
	input = RP_DIO1_P;
	rp_DpinSetDirection(output, RP_OUT);
	rp_DpinSetState(output, RP_HIGH);
	rp_DpinSetDirection(input, RP_IN);
	int i;
	rp_pinState_t state;
	state = RP_HIGH;
	for (i = 0; i < 100; i++) {
		fflush(stdout);		
		usleep(3000000);
		if (state == RP_HIGH) {
			state = RP_LOW;
			rp_DpinSetState(output, state);
			printf("goinglow\n");
		}
		else {
			state = RP_HIGH;
			rp_DpinSetState(output, state);
			printf("going high\n");
		}	
		
	}

	rp_Release();

	return 0;
}
