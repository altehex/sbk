#include <sbk/sbk.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>


// #define MSG_QUEUE


typedef struct {
#ifdef MSG_QUEUE
	SbkMsgQueue *queue;
#else
	SbkConnection *conn;
#endif
} FbArgs;

void *
feedback(void *args)
{	
	int time;

#ifdef MSG_QUEUE
	ssize_t     msgRead;
	SbkHighFb   fb[25];
	SbkMsgQueue *queue = ((FbArgs *) args)->queue;
#else
	SbkHighFb     fb;
	SbkConnection *conn = ((FbArgs*) args)->conn;
#endif
	
	time = 0;
	
	while (1) {
#ifdef MSG_QUEUE
		msgRead = sbk_udp_try_get_msgs(queue, fb, 25);
		
		for (int i = 0; i < msgRead; ++i) {
			usleep(500);
			++time;

			if (time%2000 == 0)
					sbk_sync_printf("[ FB(%ld):%d ] Status:\n"
							   "\tSN:  %d%d\n"
							   "\tVer: %d%d\n",
							   syscall(__NR_gettid), time,
							   fb[0].serialNumber[0], fb[0].serialNumber[1],
							   fb[0].version[0], fb[0].version[1]);
		}
#else
		sbk_udp_recv(conn, &fb, sizeof(SbkHighFb));
		
		usleep(500);
		++time;

		if (time%2000 == 0)
			sbk_sync_printf("[ FB(%ld):%d ] Status:\n"
					   "\tSN:  %d%d\n"
					   "\tVer: %d%d\n",
					   syscall(__NR_gettid), time,
					   fb.serialNumber[0], fb.serialNumber[1],
					   fb.version[0], fb.version[1]);
#endif	
	}
	
	return NULL;
}


typedef struct {
	SbkConnection *conn;
} CtrlArgs;

void *
control(void *args)
{
	SbkConnection  *conn;
	SbkHighCtrl    ctrl = {0};
	unsigned       time;
	
	conn = ((CtrlArgs *) args)->conn;

	ctrl.head[0] = 0xFE;
	ctrl.head[1] = 0xEF;
	ctrl.levelFlag = 0;

	time = 0;
	
	while (1) {
		usleep(500);
		++time;
		
		if (time%2000 != 0)
			continue;
			
		switch (time) {
			case 1: {
				ctrl.mode = SBK_MODE_FORCE_STAND;
				ctrl.euler[0] = -0.3;
				break;
			}
			case 2000: {
				ctrl.euler[0] = 0.3;
				break;
			}
		    case 4000: {
				ctrl.euler[0] = 0;
				ctrl.euler[1] = -0.2;
				break;
			}
		    case 6000: {
				ctrl.euler[1] = 0.2;
				break;
			}
			case 8000: {
				ctrl.euler[1] = 0;
				ctrl.euler[2] = -0.2;
				break;
			}
			case 10000: {
				ctrl.euler[2] = 0.2;
				break;
			}
			case 12000: {
				ctrl.euler[2] = 0;
				ctrl.bodyHeight = -0.2;
				break;
			}
			case 14000: {
				ctrl.bodyHeight = 0.1;
				break;
			}
			case 16000: {
				ctrl.bodyHeight = 0;
				break;
			}
			case 18000: {
				ctrl.mode = SBK_MODE_STAND_DOWN;
				break;
			}
		    case 22000: {
				ctrl.mode = SBK_MODE_STAND_UP;
				break;
			}
			case 24000: {
				ctrl.mode = SBK_MODE_IDLE;
				break;
			}
			case 32000: {
				ctrl.mode        = SBK_MODE_TARGET_VELOCITY;
				ctrl.gait        = SBK_GAIT_TROT;
				ctrl.velocity[0] = 0.4;
				ctrl.yawSpeed    = 2.0;
				ctrl.bodyHeight  = 0.1;
				break;
			}
			case 39000: {
				ctrl.mode        = SBK_MODE_IDLE;
				ctrl.velocity[0] = 0.0;
				break;
			}
			case 44000: {
				ctrl.mode        = SBK_MODE_TARGET_VELOCITY;
				ctrl.gait        = SBK_GAIT_TROT;
				ctrl.velocity[0] = 0.2;
				break;
			}
			case 48000: {
				ctrl.mode        = SBK_MODE_FORCE_STAND;
				ctrl.gait        = SBK_GAIT_IDLE;
				ctrl.velocity[0] = 0.0;
				break;
			}
			case 50000: {
				ctrl.mode = SBK_MODE_STRAIGHT_HAND;
				break;
			}
			case 54000: {
				ctrl.mode = SBK_MODE_JUMP_YAW;
				break;
			}
			case 58000: {
				ctrl.mode = SBK_MODE_IDLE;
				
				time = 0;
			}
		} // switch (time)

		sbk_sync_printf("[ CTRL(%ld) ] ---------------------\n"
				   "mode:        %d\n"
				   "euler:       0: %f, 1: %f, 2: %f\n"
				   "bodyHeight:  %f\n"
				   "motion time: %d\n"
				   "-----------------------------------\n",
				   syscall(__NR_gettid), ctrl.mode,
				   ctrl.euler[0], ctrl.euler[1], ctrl.euler[2],
				   ctrl.bodyHeight,
				   time);
		
		sbk_udp_send(conn, (uint8_t *) (&ctrl), sizeof(SbkHighCtrl));
	} // while (1)
	
	return NULL;
}


int
main()
{
	// fbt:    feedback thread
	// ctrlt:  control thread
	pthread_t fbt, ctrlt;
	SbkConnection conn;
	SbkMsgQueue *queue;

	CtrlArgs ctrlArgs = {0};
	FbArgs fbArgs = {0};

#ifndef MSG_QUEUE
	SbkRecvLoopArgs recvLoopArgs = {0};
	queue = NULL;
#endif
	if (sbk_udp_open(SBK_UDP_LISTEN_PORT,
					 SBK_WIRED_LISTEN_IP,
					 SBK_UDP_HIGH_LEVEL_CONN,
					 0, &conn, &queue) < 0) {
		sbk_sync_printf("Exiting...\n");
		return -1;
	};

#ifdef MSG_QUEUE	
	// recvt:  receive thread
	pthread_t recvt;
	
	recvLoopArgs.conn       = &conn;
	recvLoopArgs.queue      = queue;
	recvLoopArgs.recvLength = 25;
	pthread_create(&recvt, NULL, sbk_udp_recv_loop,
				   (void *) &recvLoopArgs);

	fbArgs.queue = queue;
#else
	fbArgs.conn = &conn;
#endif
	pthread_create(&fbt, NULL, feedback, (void *) &fbArgs);
	
	ctrlArgs.conn = &conn;
	pthread_create(&ctrlt, NULL, control, (void *) &ctrlArgs);

	while (1) if (getc(stdin) == '\n') break;

	sbk_sync_printf("Exiting...\n");
	
	pthread_cancel(fbt);
	pthread_cancel(ctrlt);
#ifdef MSG_QUEUE
	pthread_cancel(recvt);
#endif
		
	sbk_udp_close(&conn, queue);
	
	return 0;
}
