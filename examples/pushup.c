#include <sbk/sbk.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>


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
		usleep(1000000);
		++time;

		ctrl.mode = SBK_MODE_STAND_DOWN^(time&1);

		sbk_sync_printf("[ CTRL(%ld) ] ---------------------\n"
				   "mode:        %d\n"
				   "motion time: %d\n"
				   "-----------------------------------\n",
				   syscall(__NR_gettid), ctrl.mode,
				   time);
		
		sbk_udp_send(conn, (uint8_t *) (&ctrl), sizeof(SbkHighCtrl));
	} // while (1)
	
	return NULL;
}


int
main()
{
	// ctrlt:  control thread
	pthread_t ctrlt;
	SbkConnection conn;
	CtrlArgs ctrlArgs = {0};

	if (sbk_udp_open(SBK_UDP_HIGH_LEVEL_CONN,
					 0, &conn, NULL) < 0) {
		sbk_sync_printf("Exiting...\n");
		return -1;
	};
	
	ctrlArgs.conn = &conn;
	pthread_create(&ctrlt, NULL, control, (void *) &ctrlArgs);

	while (1) if (getc(stdin) == '\n') break;

	sbk_sync_printf("Exiting...\n");
	pthread_cancel(ctrlt);
	sbk_udp_close(&conn, NULL);
	
	return 0;
}
