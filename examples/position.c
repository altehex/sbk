#include <sbk/sbk.h>

#include <pthread.h>
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
	int time;
	SbkLowFb fb;
	SbkConnection *conn = ((FbArgs*) args)->conn;
	
	time = 0;
	
	while (1) {
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
	SbkLowCtrl     ctrl = {0};
	unsigned       time;
	
	conn = ((CtrlArgs *) args)->conn;

	ctrl.head[0] = 0xFE;
	ctrl.head[1] = 0xEF;
	ctrl.levelFlag = 0xFF;

	time = 0;
	
	while (1) {
		usleep(1000);
		++time;
	   		
		sbk_udp_send(conn, (uint8_t *) (&ctrl), sizeof(SbkLowCtrl));
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
	
	CtrlArgs ctrlArgs = {0};
	FbArgs fbArgs = {0};

	if (sbk_udp_open(SBK_UDP_LOW_LEVEL_CONN,
					 0, &conn, NULL) < 0) {
		sbk_sync_printf("Exiting...\n");
		return -1;
	};

	fbArgs.conn = &conn;
	pthread_create(&fbt, NULL, feedback, (void *) &fbArgs);
	
	ctrlArgs.conn = &conn;
	pthread_create(&ctrlt, NULL, control, (void *) &ctrlArgs);

	while (1) if (getc(stdin) == '\n') break;

	sbk_sync_printf("Exiting...\n");
	
	pthread_cancel(fbt);
	pthread_cancel(ctrlt);
		
	sbk_udp_close(&conn, NULL);
	
	return 0;
}
