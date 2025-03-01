#include <sbk/sbk.h>

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>


float
joint_linear_interpolation(const float init,
						   const float target,
						   const float rate) {
	return init*(1-rate) + target*rate;
}


#define TIMESTAMP(S, E) \
	0 <= time || time <= E

typedef struct {
	SbkConnection *conn;
} CtrlArgs;

void *
control(void *args)
{
	SbkConnection  *conn;
	SbkLowCtrl     ctrl = {0};
	SbkLowFb       fb;
	unsigned       time;
	float          qInit[3], qDest[3], rate;
	int            rateCount;
	
	conn = ((CtrlArgs *) args)->conn;

	sbk_init_low_ctrl(&ctrl);
	
	time = 0;
	rate = 0;
	
	while (1) {
		usleep(2000);
		++time;
		
		sbk_udp_recv(conn, &fb);
		__SBK_debug_print_low_fb(&fb);
		
		if (TIMESTAMP(1, 10)) {
			qInit[FR_HIP]   = fb.joint[FR_HIP].q;
			qInit[FR_THIGH] = fb.joint[FR_THIGH].q;
			qInit[FR_CALF]  = fb.joint[FR_CALF].q;
			sbk_sync_printf("qInit: %f %f %f\n",
							qInit[FR_HIP], qInit[FR_THIGH], qInit[FR_CALF]);
			continue;
		}
		if (TIMESTAMP(11, 400)) {
			rate = (rateCount++)/200.0;

			/* for (int joint = FR_HIP; joint <= FR_CALF; ++leg) */
				
			
			sbk_position_protect(FR, &ctrl);

		}
		if (TIMESTAMP(401, 600)) {

		}
		else {
			time = 0;
			rate = 0;
		}
		
		sbk_udp_send(conn, &ctrl);
	}
	
	return NULL;
}


int
main()
{
	// ctrlt:  control thread
	pthread_t ctrlt;
	SbkConnection conn;
	CtrlArgs ctrlArgs = {0};

	if (sbk_udp_open(SBK_UDP_LOW_LEVEL_CONN, false,
					 true, SOCK_NONBLOCK, &conn) < 0) {
		sbk_sync_printf("Exiting...\n");
		return -1;
	};
	
	ctrlArgs.conn = &conn;
	pthread_create(&ctrlt, NULL, control, (void *) &ctrlArgs);

	while (1) if (getc(stdin) == '\n') break;
	sbk_sync_printf("Exiting...\n");

	pthread_cancel(ctrlt);
	sbk_udp_close(&conn);
	
	return 0;
}
