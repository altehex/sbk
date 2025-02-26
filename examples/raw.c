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

	__SBK_debug_print_motor_fb(&(fb.joint[FR_CALF]));
	__SBK_debug_print_high_fb(&fb);
	__SBK_debug_print_high_ctrl(&ctrl);
	printf("bms size: %d\n", sizeof(SbkBmsFb));
	
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(8082);
	inet_pton(AF_INET, "192.168.123.161", &addr.sin_addr);
	
	initCtrl.levelFlag = 0x0;
	initCtrl.head[0] = 0xFE;
	initCtrl.head[1] = 0xEF;
	
	ctrl.levelFlag = 0x0;
	ctrl.head[0] = 0xFE;
	ctrl.head[1] = 0xEF;

	ctrl._r2 = 0;
	ctrl.crc = sbk_gen_crc((uint32_t *)&ctrl, sizeof(SbkHighCtrl));	
	__SBK_debug_print_high_ctrl_packet(&ctrl);
	
	ctrl.mode = SBK_MODE_STAND_DOWN;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	/* int flags = fcntl(sockfd, F_GETFL, 0); */
	/* fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); */

	ctrl.crc = sbk_gen_crc((uint32_t *)&ctrl, sizeof(SbkHighCtrl));	
	b = sendto(sockfd, &initCtrl, sizeof(SbkHighCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));
	printf("Sent: %d\n", b);
	__SBK_debug_print_high_ctrl_packet(&ctrl);

	int t = 0;
	
	while (0) {
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
		__SBK_debug_print_high_ctrl(&ctrl);
		__SBK_debug_print_high_ctrl_packet(&ctrl);
		
		addrlen = sizeof(struct sockaddr_in);
	}	
}
