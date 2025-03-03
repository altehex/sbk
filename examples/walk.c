#include <sbk/sbk.h>

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>


typedef struct {
	SbkConnection *conn;
} FbArgs;

void *
feedback(void *args)
{
	SbkConnection  *conn;
	SbkHighFb      fb = {0};
	unsigned       time;
	
	conn = ((FbArgs *) args)->conn;

	time = 0;

	while (1) {
		usleep(1000000);
		++time;

		sbk_udp_recv(conn, &fb);
		__SBK_debug_print_high_fb(&fb);
	}
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

	sbk_init_high_ctrl(&ctrl);

	time = 0;
	
	while (1) {
		usleep(1000000);
		++time;

		switch (time) {
			case 1: {
				ctrl.mode = SBK_MODE_FORCE_STAND;
				ctrl.euler[0] = -0.4;
				break;
			}
			case 2: {
				ctrl.euler[0] = 0.4;
				break;
			}
			case 3: {
				ctrl.euler[0] = 0;
				ctrl.euler[1] = -0.3;
				break;
			}
			case 4: {
				ctrl.euler[1] = 0.3;
				break;
			}
			case 5: {
				ctrl.euler[1] = 0;
				ctrl.bodyHeight = -0.2;
				break;
			}
			case 6: {
				ctrl.bodyHeight = 0.1;
				break;
			}
			case 7: {
				ctrl.bodyHeight = 0;
				break;
			}
			case 8: {
				ctrl.mode = SBK_MODE_STAND_DOWN;
				break;
			}
			case 10: {
				ctrl.mode = SBK_MODE_STAND_UP;
				break;
			}
			case 12: {
				ctrl.mode = SBK_MODE_IDLE;
				break;
			}
			case 18: {
				ctrl.mode        = SBK_MODE_TARGET_VELOCITY;
				ctrl.gait        = SBK_GAIT_TROT;
				ctrl.velocity[0] = 0.2;
				break;	
			}
			case 22: {
				ctrl.mode        = SBK_MODE_FORCE_STAND;
				ctrl.gait        = SBK_GAIT_IDLE;
				ctrl.velocity[0] = 0.0;
				break;
			}
			case 24: {
				ctrl.mode = SBK_MODE_STRAIGHT_HAND;
				break;
			}
			case 26: {
				ctrl.mode = SBK_MODE_JUMP_YAW;
				break;
			}
			case 28: {
				ctrl.mode = SBK_MODE_IDLE;
				
				time = 0;
			}
		}
		
		sbk_sync_printf("[ CTRL(%ld) ] ---------------------\n"
				   "mode:        %d\n"
				   "motion time: %d\n"
				   "-----------------------------------\n",
				   syscall(__NR_gettid), ctrl.mode,
				   time);
		
		sbk_udp_send(conn, &ctrl);
	} // while (1)
	
	return NULL;
}


int
main()
{
	// ctrlt:  control thread
	pthread_t ctrlt, fbt;
	SbkConnection conn;
	CtrlArgs ctrlArgs = {0};
	FbArgs fbArgs = {0};
	char useWifiYn;
	bool useWifi;

	printf("Use wifi connection? (y/N): ");
	useWifiYn = getc(stdin);
	useWifi = useWifiYn == 'y' || useWifiYn == 'Y';
	if (useWifiYn != '\n') getc(stdin);
	
	if (sbk_udp_open(SBK_UDP_HIGH_LEVEL_CONN, false, useWifi,
					 SOCK_NONBLOCK, &conn) < 0) {
		sbk_sync_printf("Exiting...\n");
		return -1;
	};
	
	ctrlArgs.conn = &conn;
	pthread_create(&ctrlt, NULL, control, &ctrlArgs);

	fbArgs.conn = &conn;
	pthread_create(&fbt, NULL, feedback, &fbArgs);
	
	while (1) if (getc(stdin) == '\n') break;
	sbk_sync_printf("Exiting...\n");

	pthread_cancel(ctrlt);
	pthread_cancel(fbt);
	sbk_udp_close(&conn);
	
	return 0;
}
