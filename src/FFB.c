#include "FFB.h"

int ffb_fd;
pthread_t thread_id;

int ffb_running = 0;
int ffb_serial;

char* ffwheel_device_name = "/dev/input/event0";

struct ff_effect effect;
struct input_event event;

int initFFB() {
  /* Setup the serial interface here */
  /*
  ffb_serial = open(ffbName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);

  if (ffb_serial < 0) {
      printf("Failed to open RS232 FFB file descriptor\n");
      return -1;
  }
  set_interface_attribs(ffb_serial, B38400);
  */
  memset(&event,0,sizeof(event));
	event.type=EV_FF;
	event.code=FF_AUTOCENTER;
	event.value=0;
	if (write(device_handle,&event,sizeof(event))!=sizeof(event)) {
		printf("Failed to turn off autocenter\n");
	}


  /* Initialize constant force effect */
  memset(&effect,0,sizeof(effect));
  effect.type=FF_CONSTANT;
  effect.id=-1;
  effect.trigger.button=0;
  effect.trigger.interval=0;
  effect.replay.length=0xffff;
  effect.replay.delay=0;
  effect.u.constant.level=0;
  effect.direction=0xC000;
  effect.u.constant.envelope.attack_length=0;
  effect.u.constant.envelope.attack_level=0;
  effect.u.constant.envelope.fade_length=0;
  effect.u.constant.envelope.fade_level=0;

  /* Upload effect */
  if (ioctl(device_handle,EVIOCSFF,&effect)<0) {
    fprintf(stderr,"ERROR: uploading effect failed (%s) [%s:%d]\n",
            strerror(errno),__FILE__,__LINE__);
    exit(1);
  }

  /* Start effect */
	memset(&event,0,sizeof(event));
	event.type=EV_FF;
	event.code=effect.id;
	event.value=1;
	if (write(device_handle,&event,sizeof(event))!=sizeof(event)) {
		fprintf(stderr,"ERROR: starting effect failed (%s) [%s:%d]\n",
		        strerror(errno),__FILE__,__LINE__);
		exit(1);
  }

  ffb_running = 1;
  return 0;
}

/* update the device: set force and query joystick position */
void update_device(double force)
{
	struct input_event event;

	/* Delete effect */
	if (stop_and_play && effect.id!=-1) {
		if (ioctl(device_handle,EVIOCRMFF,effect.id)<0) {
			fprintf(stderr,"ERROR: removing effect failed (%s) [%s:%d]\n",
			        strerror(errno),__FILE__,__LINE__);
			exit(1);
		}
		effect.id=-1;
	}

	/* Set force */
	if (force>1.0) force=1.0;
	else if (force<-1.0) force=-1.0;
	effect.u.constant.level=(short)(force*32767.0);
	effect.direction=0xC000;
	effect.u.constant.envelope.attack_level=(short)(force*32767.0); /* this one counts! */
	effect.u.constant.envelope.fade_level=(short)(force*32767.0); /* only to be safe */

	/* Upload effect */
	if (ioctl(device_handle,EVIOCSFF,&effect)<0) {
		perror("upload effect");
		/* We do not exit here. Indeed, too frequent updates may be
		 * refused, but that is not a fatal error */
	}

	/* Start effect */
	if (stop_and_play && effect.id!=-1) {
		memset(&event,0,sizeof(event));
		event.type=EV_FF;
		event.code=effect.id;
		event.value=1;
		if (write(device_handle,&event,sizeof(event))!=sizeof(event)) {
			fprintf(stderr,"ERROR: re-starting effect failed (%s) [%s:%d]\n",
			        strerror(errno),__FILE__,__LINE__);
			exit(1);
		}
	}
}


void runFFB() {
    pthread_create(&thread_id, NULL, FFBThread, NULL);
    printf("FFB Module Started\n");
}

void *FFBThread(void *arg) {

  while (ffb_running) {
    /*
    unsigned char buffer[] = {
        0x00
    };
    int n = -1;
    while (n < 1) {
        n = read(ffb_serial, buffer, 1);
    }
    printf("Serial FFB: %d", buffer[0]);
    */
    update_device(0.5);
  }
}

void closeFFB() {
    pthread_join(thread_id, NULL);
    close(ffb_fd);
}
