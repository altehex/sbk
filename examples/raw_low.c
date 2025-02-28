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
	SbkLowCtrl ctrl = {0};
	SbkLowFb fb;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	__SBK_debug_print_motor_low_fb(&(fb.joint[FR_CALF]));
	__SBK_debug_print_low_fb(&fb);
	__SBK_debug_print_low_ctrl(&ctrl);
	printf("bms size: %d\n", sizeof(SbkBmsFb));
	
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(8007);
	inet_pton(AF_INET, "192.168.123.10", &addr.sin_addr);
	
	ctrl.levelFlag = 0xFF;
	ctrl.head = 0xFEEF;
	ctrl.bandwidth = 0xC03A;
	for (int i = 0; i < 20; ++i)
		ctrl.joint[i].mode = SBK_MOTOR_SERVO;

	ctrl.crc = sbk_gen_crc_encode((uint32_t *)&ctrl, sizeof(SbkLowCtrl));		
	sbk_print_raw_msg(&ctrl, sizeof(SbkLowCtrl));
		
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	/* int flags = fcntl(sockfd, F_GETFL, 0); */
	/* fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); */

	ctrl.crc = sbk_gen_crc_encode((uint32_t *)&ctrl, sizeof(SbkLowCtrl));	
	b = sendto(sockfd, &ctrl, sizeof(SbkLowCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));
	printf("Sent: %d\n", b);
		
	int t = 0;
	
	while (0) {
		usleep(2000000);
		++t;
		
		ctrl.crc = sbk_gen_crc_encode((uint32_t *)&ctrl, sizeof(SbkLowCtrl));
		b = sendto(sockfd, &ctrl, sizeof(SbkLowCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));
		printf("Sent: %d\n", b);

		puts("receiving...");
		
		b = recvfrom(sockfd, &fb, sizeof(SbkLowFb), 0,
					 (struct sockaddr *) &addr, &addrlen);
		
		printf("Received: %d\n", b);
		/* __SBK_debug_print_low_fb(&fb); */
		sbk_print_raw_msg(&ctrl, sizeof(SbkLowCtrl));
		
		addrlen = sizeof(struct sockaddr_in);
	}	
}
