#define DEBUG
#include <sbk/sbk.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int
main(void)
{
	int sockfd, b;
	SbkHighCtrl initCtrl = {0};
	SbkHighCtrl ctrl = {0};
	SbkHighFb fb;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(8082);
	inet_pton(AF_INET, "192.168.123.161", &addr.sin_addr);
	
	initCtrl.levelFlag = 0x0;
	initCtrl.head     = 0xEFFE;
	
	ctrl.levelFlag = 0x0;
	ctrl.head      = 0xEFFE;

	ctrl._r2 = 0;
	ctrl.crc = sbk_gen_crc((uint32_t *)&ctrl, sizeof(SbkHighCtrl));	
	sbk_print_raw_msg(&ctrl, sizeof(SbkHighCtrl));
	
	ctrl.mode = SBK_MODE_STAND_DOWN;
	
	sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
	ctrl.crc = sbk_gen_crc((uint32_t *)&ctrl, sizeof(SbkHighCtrl));	
	b = sendto(sockfd, &initCtrl, sizeof(SbkHighCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));
	printf("Sent: %d\n", b);
	sbk_print_raw_msg(&ctrl, sizeof(SbkHighCtrl));

	__SBK_debug_print_motor_high_fb(&(fb.joint[FR_CALF]));
	__SBK_debug_print_high_fb(&fb);
	__SBK_debug_print_high_ctrl(&ctrl);
	printf("bms size: %d\n", sizeof(SbkBmsFb));
	
	int t = 0;
	
	while (1) {
		usleep(2000000);
		++t;
		
		if (t & 1)
			ctrl.mode = SBK_MODE_STAND_UP;
		else
			ctrl.mode = SBK_MODE_STAND_DOWN;

		ctrl.crc = sbk_gen_crc((uint32_t *)&ctrl, sizeof(SbkHighCtrl));
		b = sendto(sockfd, &ctrl, sizeof(SbkHighCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));
		printf("Sent: %d\n", b);

		puts("receiving...");
		
		b = recvfrom(sockfd, &fb, sizeof(SbkHighFb), 0,
					 (struct sockaddr *) &addr, &addrlen);
		
		printf("Received: %d\n", b);
		/* __SBK_debug_print_high_fb(&fb); */
		sbk_print_raw_msg(&ctrl, sizeof(SbkHighCtrl));
		
		addrlen = sizeof(struct sockaddr_in);
	}	
}
