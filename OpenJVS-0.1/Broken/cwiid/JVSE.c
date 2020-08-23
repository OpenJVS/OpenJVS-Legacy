#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <cwiid.h>

#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))


int mot_x = 0;
int mot_y = 0;

void print_state(struct cwiid_state *state) {
	float tmpx = 0;
	float tmpy = 0;
	float count = 0;
	for (int i = 0; i < CWIID_IR_SRC_COUNT; i++) {
		if (state->ir_src[i].valid) {
			//printf("(%d,%d) ", state->ir_src[i].pos[CWIID_X], state->ir_src[i].pos[CWIID_Y]);
			count++;
			tmpx += state->ir_src[i].pos[CWIID_X];
			tmpy += state->ir_src[i].pos[CWIID_Y];
		}
		
	}
	float n_x = ((tmpx / count) / 1000) * 255;
	float n_y = ((tmpy / count) / 1000) * 255;
	mot_x = 255 - (int)n_x;
	mot_y = (int)n_y;
	printf("%d, %d\n", mot_x, mot_y);
}

int main(int argc, char *argv[]) {

	cwiid_wiimote_t *wiimote;		/* wiimote handle */
	struct cwiid_state state;		/* wiimote state */
	bdaddr_t bdaddr = *BDADDR_ANY;;		/* bluetooth device address */
	
	unsigned char mesg = 0;
	unsigned char led_state = 0;
	unsigned char rpt_mode = 0;
	unsigned char rumble = 0;
	
	int exit = 0;

	printf("Put Wiimote in discoverable mode now (press 1+2)...\n");
	while (!(wiimote = cwiid_open(&bdaddr, 0))) {
		fprintf(stderr, "Unable to connect to wiimote, try again\n");
	}
	
	toggle_bit(rpt_mode, CWIID_RPT_STATUS);
	toggle_bit(rpt_mode, CWIID_RPT_IR);
	toggle_bit(rpt_mode, CWIID_RPT_BTN);

	if (cwiid_set_rpt_mode(wiimote, rpt_mode)) {
		fprintf(stderr, "Error setting report mode\n");
	}

	printf("Report Mode Toggled\n");

	while(1) {
		if (cwiid_get_state(wiimote, &state)) {
				fprintf(stderr, "Error getting state\n");
			}
		print_state(&state);
		sleep(1);
	}

	if (cwiid_close(wiimote)) {
		fprintf(stderr, "Error on wiimote disconnect\n");
		return -1;
	}

	printf("Finished!\n");

	return 0;

}
