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
						   const float rate)
{
	return init*(1-rate) + target*rate;
}


#define TIMESTAMP(S, E) \
	S <= ++time && time <= E

typedef struct {
	SbkConnection *conn;
} CtrlArgs;

void *
control(void *args)
{
	const float FREQ_RAD = 1*2*3.24,
		        DT = 0.004;
	
	SbkConnection  *conn;
	SbkLowCtrl     ctrl = {0};
	SbkLowFb       fb;
	unsigned       time, rateCount, sinCount;
	float          qInit[3], qDes[3], rate, t,
	               sinMidQ[3] = { 0.0, 1.2, -2.0 };
	
	conn = ((CtrlArgs *) args)->conn;

	sbk_init_low_ctrl(&ctrl);
	sbk_set_torque(-0.65, FR_HIP, &ctrl);
	for (int joint = FR_HIP; joint <= FR_CALF; ++joint) {
		sbk_set_kp(5, joint, &ctrl);
		sbk_set_kd(1, joint, &ctrl);
	}
	
	time = 0;
	rate = 0;
	sinCount = 0;
	
	while (1) {
		usleep(4000);
		sbk_sync_printf("Time: %d\n", time);

		sbk_udp_recv(conn, &fb);	
		
		if (TIMESTAMP(1, 10)) {
			qInit[HIP]   = fb.joint[FR_HIP].q;
			qInit[THIGH] = fb.joint[FR_THIGH].q;
			qInit[CALF]  = fb.joint[FR_CALF].q;

			sbk_sync_printf("qInit: %f %f %f\n",
							qInit[HIP], qInit[THIGH], qInit[CALF]);

			continue;
		}
		else if (TIMESTAMP(11, 400)) {
			rate = (rateCount++)/200.0;

			for (int joint = FR_HIP; joint <= FR_CALF; ++joint) 
				qDes[joint] = joint_linear_interpolation(qInit[joint], sinMidQ[joint], rate);
		}
		else if (TIMESTAMP(401, 100000)) {
			t = DT*(++sinCount);

			qDes[HIP]   = sinMidQ[HIP];
			qDes[THIGH] = sinMidQ[THIGH] + 0.6*sin(t*FREQ_RAD);
			qDes[CALF]  = sinMidQ[CALF] - 0.9*sin(t*FREQ_RAD);
		}
		else {
			time = 0;
			rate = 0;
			sinCount = 0;
			continue;
		}

		sbk_sync_printf("qDes: %f %f %f\n",
						qDes[HIP], qDes[THIGH], qDes[CALF]);

		ctrl.joint[FR_HIP].q   = qDes[HIP];
		ctrl.joint[FR_THIGH].q = qDes[THIGH];
		ctrl.joint[FR_CALF].q  = qDes[CALF];

		sbk_position_protect(FR, &ctrl);
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
					 false, SOCK_NONBLOCK, &conn) < 0) {
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
