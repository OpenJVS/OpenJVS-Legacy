#ifdef WII_INCLUDE
#include "Wii.h"
#define PI	3.14159265358979323
pthread_t thread_id;

int mot_x = 0;
int mot_y = 0;

int initWii() {
  return 0;
}

void runWii() {
    pthread_create(&thread_id, NULL, wiiThread, NULL);
    printf("Wii Remote Module Started\n");
}

void print_state(struct cwiid_state *state) {
	double a_x = state->acc[CWIID_X] - 128;
	double a_z = state->acc[CWIID_Z] - 128;
	double roll, pitch;
	roll = atan(a_x/a_z);
	if (a_z <= 0.0) {
		roll += PI * ((a_x > 0.0) ? 1 : -1);
	}
	double roll_d = roll * 180.0 / PI;
        roll = -roll; 
	float tmpy = 0;
        float tmpx = 0;
        float count = 0;
        for (int i = 0; i < CWIID_IR_SRC_COUNT; i++) {
                if (state->ir_src[i].valid) {
                        count++;
			float ix = state->ir_src[i].pos[CWIID_X];
			float iy = state->ir_src[i].pos[CWIID_Y];
                        //tmpx += ((ix - 512) * cos(roll) - (iy - 512) * sin(roll)) + 512;
                        //tmpy += ((iy - 512) * cos(roll) + (ix - 512) * sin(roll)) + 512;
                	tmpx += ix;
			tmpy += iy;
		}

        }

	if(count == 0) {
		
		setPlayerSwitch(0, 7, 1);
	} else {

		setPlayerSwitch(0, 7, 0);
	}

        float n_x = ((tmpx / count) / 1000) * 255;
        float n_y = ((tmpy / count) / 1000) * 255;
        mot_x = 255 - (int)n_x;
        mot_y = (int)n_y;
	//printf("Roll: %f X: %d Y: %d\n", roll, mot_x, mot_y);



	setPlayerSwitch(0, 0, (state->buttons & 8) == 8);
	setPlayerSwitch(0, 8, (state->buttons & 8) == 8);
	
	setPlayerSwitch(0, 6, (state->buttons & 4) == 4);
	setSystemSwitch(0, (state->buttons & 1) == 1);
	setPlayerSwitch(0, 1, (state->buttons & 16) == 16);
	setPlayerSwitch(0, 2, (state->buttons & 16) == 16);
	setPlayerSwitch(0, 3, (state->buttons & 16) == 16);
	//printf("%d\n", state->buttons);
	if(state->buttons == 2) {
		incrementCoin();
	}
        //printf("%d, %d\n", mot_x, mot_y);
	if(count > 1) {
		setAnalogue(0, mot_x);
		setAnalogue(1, mot_y);
	}
}


void *wiiThread(void *arg) {

	 cwiid_wiimote_t *wiimote;               /* wiimote handle */
        struct cwiid_state state;               /* wiimote state */
        bdaddr_t bdaddr = *BDADDR_ANY;;         /* bluetooth device address */

        unsigned char mesg = 0;
        unsigned char led_state = 0;
        unsigned char rpt_mode = 0;
        unsigned char rumble = 0;

        int exit = 0;

        printf("Put Wiimote in discoverable mode now (press 1+2)...\n");
        while (!(wiimote = cwiid_open(&bdaddr, 0))) {
                fprintf(stderr, "Unable to connect to wiimote, try again\n");
        }

	toggle_bit(rpt_mode, CWIID_RPT_ACC);        
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
        }

        if (cwiid_close(wiimote)) {
                fprintf(stderr, "Error on wiimote disconnect\n");
                return 0;
        }

        printf("Finished!\n");

        return 0;

}

void closeWii() {
    pthread_join(thread_id, NULL);
}
#endif
