#include "../include/client.h"

int main( int argc, char **argv ) {
	int sk;
	struct sockaddr_in server;
	char buf[MAXBUF] = "helloIServer";
	unsigned char send_buf[256];
	char source_ip[32] = "192.168.2.1";
	int n_sent;
	int n_read;
	struct udphdr *udph = NULL;
	struct iphdr *iph = NULL;
	int one = 1;
	const int *val = &one;

	udph = malloc(sizeof(struct udphdr));
	iph = malloc(sizeof(struct iphdr));

	if ((sk = socket( AF_INET, SOCK_RAW, IPPROTO_UDP )) < 0){
		printf("Problem creating socket\n");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("192.168.2.59")/*htonl(INADDR_LOOPBACK)*/;
	server.sin_port = htons(SERVER_PORT);
	/*create UDP structure*/
	memset(udph, 0, sizeof(struct udphdr));

	udph->source = htons(MY_PORT);
	udph->dest = htons(SERVER_PORT);
	udph->len  = htons(sizeof(struct udphdr) + strlen(buf));
	udph->check = 0;

	/*create IP structure*/
	memset(iph, 0, sizeof(struct iphdr));

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(buf));
	iph->id = htons (12345);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = 17;
	iph->check = 0;
	iph->daddr = inet_addr ("192.168.2.59");
	iph->saddr = inet_addr (source_ip);

	memcpy(send_buf, iph, sizeof(struct iphdr));
	memcpy(send_buf + sizeof(struct iphdr), udph, sizeof(struct udphdr));
	memcpy(send_buf + sizeof(struct udphdr) + sizeof(struct iphdr),
							 buf, strlen(buf));

	if (setsockopt (sk, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0){
		perror("Error setting IP_HDRINCL");
		exit(0);
	}

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
