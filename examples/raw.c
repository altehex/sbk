#include <sbk/sbk.h>

#include <sys/socket.h>
#include <arpa/inet.h>


int
main(void)
{
	int sockfd;
	SbkHighCtrl ctrl = {0};
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(8082);
	inet_pton(AF_INET, "192.168.123.161", &addr.sin_addr);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	int b = sendto(sockfd, &ctrl, sizeof(SbkHighCtrl), 0,
				   (struct sockaddr *) &addr, sizeof(addr));

	printf("Sent: %d\n", b);

	ctrl.mode = SBK_MODE_STAND_UP;
	b = sendto(sockfd, &ctrl, sizeof(SbkHighCtrl), 0,
			   (struct sockaddr *) &addr, sizeof(addr));
}
