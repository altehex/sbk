#include <sbk/sbk.h>

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/syscall.h>


atomic_flag stdoutLock = ATOMIC_FLAG_INIT;


typedef struct {
	SbkMsgQueue *queue;
} FbArgs;

void *
feedback(void *args)
{	
	int          time;
	SbkMsgQueue  *queue;
	SbkHighFb    fb[25];
	ssize_t      msgRead;

	queue = ((FbArgs *) args)->queue;

	time = 0;
	
	while (1) {
		msgRead = sbk_udp_try_get_msgs(queue, fb, 25);
		
		for (int i = 0; i < msgRead; ++i) {
			usleep(500);
			++time;

			if (time%2000 == 0)
				if ( ! atomic_flag_test_and_set_explicit(
						   &stdoutLock, memory_order_acquire)) {
					printf("[ FB(%ld):%d ] Status:\n"
						   "\tSN:  %d%d\n"
						   "\tVer: %d%d\n",
						   syscall(__NR_gettid), time,
						   fb[0].serialNumber[0], fb[0].serialNumber[1],
						   fb[0].version[0], fb[0].version[1]);
					
					atomic_flag_clear_explicit(&stdoutLock,
											   memory_order_release);
				}
		}
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

		sbk_udp_send(conn, (uint8_t *) (&ctrl), sizeof(SbkHighCtrl)+1);
			
		if (time%1000 != 0)
			continue;

		if ( ! atomic_flag_test_and_set_explicit(&stdoutLock,
											  memory_order_acquire)) {
			printf("[ CTRL(%ld) ] motion time: %d\n",
				   syscall(__NR_gettid), time);
			
			atomic_flag_clear_explicit(&stdoutLock,
									   memory_order_release);
		}
			
		switch (time) {
			case 1: {
				ctrl.mode = SBK_MODE_FORCE_STAND;
				ctrl.euler[0] = -0.3;
				break;
			}
			case 1000: {
				ctrl.euler[0] = 0.3;
				break;
			}
		    case 2000: {
				ctrl.euler[0] = 0;
				ctrl.euler[1] = -0.2;
				break;
			}
		    case 3000: {
				ctrl.euler[1] = 0.2;
				break;
			}
			case 4000: {
				ctrl.euler[1] = 0;
				ctrl.euler[2] = -0.2;
				break;
			}
			case 5000: {
				ctrl.euler[2] = 0.2;
				break;
			}
			case 6000: {
				ctrl.euler[2] = 0;
				ctrl.bodyHeight = -0.2;
				break;
			}
			case 7000: {
				ctrl.bodyHeight = 0.1;
				break;
			}
			case 8000: {
				ctrl.bodyHeight = 0;
				break;
			}
			case 9000: {
				ctrl.mode = SBK_MODE_STAND_DOWN;
				break;
			}
		    case 11000: {
				ctrl.mode = SBK_MODE_STAND_UP;
				break;
			}
			case 12000: {
				ctrl.mode = SBK_MODE_IDLE;
				break;
			}
			case 16000: {
				ctrl.mode        = SBK_MODE_TARGET_VELOCITY;
				ctrl.gait        = SBK_GAIT_TROT;
				ctrl.velocity[0] = 0.4;
				ctrl.yawSpeed    = 2.0;
				ctrl.bodyHeight  = 0.1;
				break;
			}
			case 19000: {
				ctrl.mode        = SBK_MODE_IDLE;
				ctrl.velocity[0] = 0.0;
				break;
			}
			case 22000: {
				ctrl.mode        = SBK_MODE_TARGET_VELOCITY;
				ctrl.gait        = SBK_GAIT_TROT;
				ctrl.velocity[0] = 0.2;
				break;
			}
			case 24000: {
				ctrl.mode        = SBK_MODE_FORCE_STAND;
				ctrl.gait        = SBK_GAIT_IDLE;
				ctrl.velocity[0] = 0.0;
				break;
			}
			case 25000: {
				ctrl.mode = SBK_MODE_STRAIGHT_HAND;
				break;
			}
			case 27000: {
				ctrl.mode = SBK_MODE_JUMP_YAW;
				break;
			}
			case 29000: {
				ctrl.mode = SBK_MODE_IDLE;
				
				time = 0;
			}
		} // switch (time)
		
	} // while (1)
	
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
	SbkMsgQueue *queue;

	SbkRecvLoopArgs recvLoopArgs = {0};
	CtrlArgs ctrlArgs = {0};
	FbArgs fbArgs = {0};
	
	char addrStr[INET_ADDRSTRLEN];

	sbk_udp_open(SBK_UDP_LISTEN_PORT,
				 SBK_UDP_HIGH_LEVEL_CONN,
				 0, &conn, &queue);

	inet_ntop(AF_INET, &conn.to.sin_addr,
			  addrStr, INET_ADDRSTRLEN);
	printf("conn = {\n"
		   "\t.sockfd = %d;\n"
		   "\t.endpoint = {\n"
		   "\t\t.sin_port = %d;\n"
		   "\t\t.sin_addr = { .s_addr = %s; };\n"
		   "\t};\n};\n",
		   conn.sendfd, ntohs(conn.to.sin_port), addrStr);
	
	recvLoopArgs.conn       = &conn;
	recvLoopArgs.queue      = queue;
	recvLoopArgs.recvLength = 25;
	pthread_create(&recvt, NULL, sbk_udp_recv_loop,
				   (void *) &recvLoopArgs);

	fbArgs.queue = queue;
	pthread_create(&fbt, NULL, feedback, (void *) &fbArgs);
	
	ctrlArgs.conn = &conn;
	pthread_create(&ctrlt, NULL, control, (void *) &ctrlArgs);

	while (1) if (getc(stdin) == '\n') break;

	printf("Exiting...");
	
	pthread_cancel(fbt);
	pthread_cancel(ctrlt);
	pthread_cancel(recvt);
		
	sbk_udp_close(&conn, queue);
	
	return 0;
}
