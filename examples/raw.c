#include <sbk/sbk.h>

#include <sys/socket.h>
#include <arpa/inet.h>


int
main(void)
{
	int sockfd;
	SbkHighCtrl initCtrl = {0};
	SbkHighCtrl ctrl = {0};
	SbkHighFb fb;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(8082);
	inet_pton(AF_INET, "192.168.123.161", &addr.sin_addr);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	int b = sendto(sockfd, &initCtrl, sizeof(SbkHighCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));

	printf("Sent: %d\n", b);

	ctrl.levelFlag = 0xff;
	ctrl.head[0] = 0xFE;
	ctrl.head[1] = 0xEF;
	b = sendto(sockfd, &ctrl, sizeof(SbkHighCtrl), 0,
			   (struct sockaddr *) &addr, sizeof(addr));

	printf("Sent: %d\n", b);

	while (1) {
		b = sendto(sockfd, &initCtrl, sizeof(SbkHighCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));

		puts("receiving...");
		
		b = recvfrom(sockfd, &fb, sizeof(SbkHighFb), 0,
					 (struct sockaddr *) &addr, &addrlen);
		
		printf("Received: %d\n", b);
		__SBK_debug_print_high_fb(&fb);
		
		addrlen = sizeof(struct sockaddr_in);
	}	
}
