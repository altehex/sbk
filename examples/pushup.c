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

	sbk_init_high_ctrl(&ctrl);

	time = 0;
	
	while (1) {
		usleep(1000000);
		++time;
		
		if (time & 1)
			ctrl.mode = SBK_MODE_STAND_UP;
		else
			ctrl.mode = SBK_MODE_STAND_DOWN;

		sbk_sync_printf("[ CTRL(%ld) ] ---------------------\n"
				   "mode:        %d\n"
				   "motion time: %d\n"
				   "-----------------------------------\n",
				   syscall(__NR_gettid), ctrl.mode,
				   time);
		
		sbk_udp_send(conn, (uint8_t *) (&ctrl));
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

	if (sbk_udp_open(SBK_UDP_HIGH_LEVEL_CONN, &conn, false,
					 SOCK_NONBLOCK, false) < 0) {
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
