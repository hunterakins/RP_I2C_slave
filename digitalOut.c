#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <redpitaya/rp.h>
#include <unistd.h>

int main() {
	printf("here");
	if (rp_Init() < 0) {
		printf("error with initialization\n");
	}

	rp_dpin_t output;
	output = RP_DIO1_P;
	rp_DpinSetDirection(output, RP_OUT);
	rp_DpinSetState(output, RP_HIGH);
	while(1) {
		rp_DpinSetState(output, RP_HIGH);
		usleep(100000);
		rp_DpinSetState(output, RP_LOW);
		usleep(100000);
	}
	rp_Release();

	return 0;
}
