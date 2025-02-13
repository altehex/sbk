#include <sbk/sbk.h>

#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/syscall.h>


#define PI ((double) 3.141592)


atomic_flag fbLock;


typedef struct {
	SbkConnection  *conn;
	SbkLowFb       *fb;
} FbArgs;

void *
feedback(void *args)
{	
	SbkLowFb       *fb;
	SbkConnection  *conn;

	fb   = ((FbArgs *) args)->queue;
	conn = ((SbkConnection *) args)->conn;
	
	while (1)
		if ( ! atomic_flag_test_and_set_explicit
			 (&fbLock, memory_order_acquire))
		{
			sbk_udp_recv(conn, fb, sizeof(SbkLowCtrl));
			atomic_flag_clear_explicit(
			    &fbLock, memory_order_release);
		}
	
	return NULL;
}


double joint_linear_interpolation(const double initPos,
								  const double targetPos,
								  const double rate)
{
	double r = min(max(rate,0.0), 1.0);
	return initPos*(1-r) + targetPos*r;
}

typedef struct {
	SbkConnection *conn;
	SbkLowFb      *fb;
} CtrlArgs;

void *
control(void *args)
{
	SbkConnection *conn;
	SbkLowFb *fb;
	SbkLowCtrl ctrl = {0};
	unsigned time, rateCount, sinCount;
	float qInit[3], qDes[3], kp[3], kd[3],
		  sinMidQ[3] = {0.0, 1.2, -2.0};
	double rate, sinJ1, sinJ2;
	
	fb   = ((CtrlArgs *) args)->fb;
	conn = ((CtrlArgs *) args)->conn;

	ctrl.head[0] = 0xFE;
	ctrl.head[1] = 0xEF;
	ctrl.levelFlag = 0xFF;

	time = 0;
	sinCount = 0;
	rateCount = 0;
	
	while (1) {
		usleep(500);
		++time;

		ctrl.joint[FR_HIP] = -0.65f;
		ctrl.joint[FL_HIP] = 0.65f;
		ctrl.joint[RR_HIP] = -0.65f;
		ctrl.joint[RL_HIP] = 0.65f;

		if (time == 0) {
			qInit[FR_HIP]   = fb->joint[FR_HIP].q;
			qInit[FR_THIGH] = fb->joint[FR_THIGH].q;
			qInit[FR_CALF]  = fb->joint[FR_CALF].q;
		} else if (time < 400) {
			rate = (++rateCount)/200.0;
            kp[FR_HIP] = 5.0; kp[FR_THIGH] = 5.0; kp[FR_CALF] = 5.0; 
            kd[FR_HIP] = 1.0; kd[FR_THIGH] = 1.0; kd[FR_CALF] = 1.0;

			qDes[FR_HIP]   = joint_linear_interpolation(qInit[FR_HIP],   sinMidQ[FR_HIP],   rate);
            qDes[FR_THIGH] = joint_linear_interpolation(qInit[FR_THIGH], sinMidQ[FR_THIGH], rate);
            qDes[FR_CALF]  = joint_linear_interpolation(qInit[FR_CALF],  sinMidQ[FR_CALF],  rate);
        } else if (time >= 400) {
			++sinCount;
			sinJ1 = 0.6*sin(3*PI*sinCount/1000.0);
			sinJ1 = -0.6*sin(0.8*PI*sinCount/1000.0);
			ctrl.joint[FR_HIP].q   = sinMidQ[FR_HIP];
			ctrl.joint[FR_THIGH].q = sinMidQ[FR_THIGH] + sinJ1;
			ctrl.joint[FR_CALF].q  = sinMidQ[FR_CALF] + sinJ2;
		}

		ctrl.joint[FR_HIP].q = ;
		ctrl.joint[FR_HIP].dq  = 0;
		ctrl.joint[FR_HIP].kp  = kp[FR_HIP];
		ctrl.joint[FR_HIP].kd  = kd[FR_HIP];
		ctrl.joint[FR_HIP].tau = ;

		ctrl.joint[FR_THIGH]. = ;
		ctrl.joint[FR_THIGH]. = ;
		ctrl.joint[FR_THIGH]. = ;
		ctrl.joint[FR_THIGH]. = ;
		ctrl.joint[FR_THIGH]. = ;

		ctrl.joint[FR_CALF]. = ;
		ctrl.joint[FR_CALF]. = ;
		ctrl.joint[FR_CALF]. = ;
		ctrl.joint[FR_CALF]. = ;
		ctrl.joint[FR_CALF]. = ;
		
		if (time > 10) {
			// Код для powerProteсt (реверсить эту хуйню надо)
		}
		
		sbk_udp_send(conn, (uint8_t *) (&ctrl), sizeof(SbkHighCtrl)+1);
	}
	
	return NULL;
}


int
main()
{
	// fbt:    feedback thread
	// ctrlt:  control thread
	// recvt:  receive thread
	pthread_t fbt, ctrlt, recvt;
	SbkConnection conn;
	SbkLowFb fb;

	CtrlArgs ctrlArgs = {0};
	FbArgs fbArgs = {0};
	
	char addrStr[INET_ADDRSTRLEN];

	sbk_udp_open(SBK_UDP_LISTEN_PORT,
				 SBK_UDP_HIGH_LEVEL_CONN,
				 0, &conn, NULL);

	inet_ntop(AF_INET, &conn.to.sin_addr,
			  addrStr, INET_ADDRSTRLEN);
	printf("conn = {\n"
		   "\t.sockfd = %d;\n"
		   "\t.endpoint = {\n"
		   "\t\t.sin_port = %d;\n"
		   "\t\t.sin_addr = { .s_addr = %s; };\n"
		   "\t};\n};\n",
		   conn.sendfd, ntohs(conn.to.sin_port), addrStr);

	fbArgs.conn = &conn;
	fbArgs.fb   = &fb;
	pthread_create(&fbt, NULL, feedback, (void *) &fbArgs);
	
	ctrlArgs.conn = &conn;
	ctrlArgs.fb   = &fb;
	pthread_create(&ctrlt, NULL, control, (void *) &ctrlArgs);

	while (1) if (getc(stdin) == '\n') break;

	printf("Exiting...");
	
	pthread_cancel(fbt);
	pthread_cancel(ctrlt);
		
	sbk_udp_close(&conn, NULL);
	
	return 0;
}
