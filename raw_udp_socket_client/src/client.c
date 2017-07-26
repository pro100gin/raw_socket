#include "../include/client.h"

int main( int argc, char **argv ) {
	int sk;
	struct sockaddr_in server;
	char buf[MAXBUF] = "helloIServer";
	unsigned char send_buf[100];
	int n_sent;
	int n_read;
	struct udphdr *udph = NULL;
	struct iphdr *iph = NULL;

	udph = malloc(sizeof(struct udphdr));

	if ((sk = socket( AF_INET, SOCK_RAW, IPPROTO_UDP )) < 0){
		printf("Problem creating socket\n");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	server.sin_port = htons(SERVER_PORT);

	udph->source = htons(MY_PORT);
	udph->dest = htons(SERVER_PORT);
	udph->len  = htons(sizeof(struct udphdr) + strlen(buf));
	udph->check = 0;

	memcpy(send_buf, udph, sizeof(struct udphdr));
	memcpy(send_buf + sizeof(struct udphdr), buf, strlen(buf));

	n_sent = sendto(sk, send_buf, MAXBUF, 0,
		(struct sockaddr*) &server, sizeof(server));

	if (n_sent<0) {
		perror("Problem sending data");
		exit(1);
	}

	memset(buf, 0, 40);

	while(1){
		memset(send_buf, 0, 100);

		n_read = recvfrom(sk,send_buf,100,0,NULL,NULL);
		if (n_read<0) {
			perror("Problem in recvfrom");
			exit(1);
		}
		iph = (struct iphdr*) send_buf;
		udph = (struct udphdr*) (send_buf + iph->ihl * 4);
		if(ntohs(udph->dest) == MY_PORT) break;
	}

	memcpy(buf, send_buf + sizeof(struct udphdr) + iph->ihl*4, 40);

	printf("IP Header\n");
        printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
        printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",
                        (unsigned int)iph->ihl,((unsigned int)iph->ihl*4));
        printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",
                                                        ntohs(iph->tot_len));
        printf("   |-TTL               : %d\n",(unsigned int)iph->ttl);
        printf("   |-Protocol          : %d\n",(unsigned int)iph->protocol);
        printf("   |-Checksum          : %d\n", ntohs(iph->check));
        /* printf("   |-Source IP         : %s\n", inet_ntoa(source.sin_addr));
        printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
	*/

	printf("\nUDP Header\n");
        printf("   |-Source Port      : %d\n", ntohs(udph->source));
        printf("   |-Destination Port : %d\n", ntohs(udph->dest));
        printf("   |-UDP Length       : %d\n", ntohs(udph->len));
        printf("   |-UDP Checksum     : %d\n", ntohs(udph->check));


	printf("Got back %d bytes\n",n_read);
	printf("%s\n", buf);

	return 0;
}
