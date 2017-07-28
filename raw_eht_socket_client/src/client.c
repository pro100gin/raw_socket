#include "../include/client.h"


#define MY_DEST_MAC0	0xA8
#define MY_DEST_MAC1	0xF9
#define MY_DEST_MAC2	0x4B
#define MY_DEST_MAC3	0x3F
#define MY_DEST_MAC4	0x0F
#define MY_DEST_MAC5	0xAE

#define MY_SRC_MAC0	0xF0
#define MY_SRC_MAC1	0xDE
#define MY_SRC_MAC2	0xF1
#define MY_SRC_MAC3	0xFE
#define MY_SRC_MAC4	0xED
#define MY_SRC_MAC5	0x58



unsigned short compute_checksum(unsigned short *addr, unsigned int count) {
        unsigned long sum = 0;
        while (count > 1) {
                sum += *addr++;
                count -= 2;
        }

        if(count > 0) {
                sum += ((*addr)&htons(0xFF00));
        }

        while (sum>>16) {
                sum = (sum & 0xffff) + (sum >> 16);
        }

        sum = ~sum;

        return ((unsigned short)sum);
}



int main( int argc, char **argv ) {
	int sk;
	struct sockaddr_ll socket_address;
	struct sockaddr_in source, dest;

	char buf[MAXBUF] = "helloIServer";
	char eth_snd_buf[256];
	unsigned char send_buf[256];
	char source_ip[32] = "192.168.2.1";
	char dest_ip[32] = "192.168.2.59";
	int n_sent;
	int n_read;
	struct udphdr *udph = NULL;
	struct iphdr *iph = NULL;
	/*struct ether_header *eh = NULL;*/
	struct ethhdr *eh = NULL;
	struct ifreq if_idx;
	struct ifreq if_mac;



	udph = malloc(sizeof(struct udphdr));
	iph = malloc(sizeof(struct iphdr));
	eh = malloc (sizeof(struct ethhdr));

	if ((sk = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){
		printf("Problem creating socket\n");
		exit(1);
	}
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

	iph->daddr = inet_addr (dest_ip);
	iph->saddr = inet_addr (source_ip);
	iph->check = compute_checksum((unsigned short *) iph, sizeof(struct iphdr));

	memcpy(send_buf, iph, sizeof(struct iphdr));
	memcpy(send_buf + sizeof(struct iphdr), udph, sizeof(struct udphdr));
	memcpy(send_buf + sizeof(struct udphdr) + sizeof(struct iphdr),
							 buf, strlen(buf));

	/*create ETH stucture*/
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, "eth1", strlen("eth1"));
	if (ioctl(sk, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");

	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, "eth1", strlen("eth1"));
	if (ioctl(sk, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");

	memset(eh, 0, sizeof(struct ethhdr));
	eh->h_source[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->h_source[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->h_source[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->h_source[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->h_source[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->h_source[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];

	eh->h_dest[0] = MY_SRC_MAC0;
	eh->h_dest[1] = MY_SRC_MAC1;
	eh->h_dest[2] = MY_SRC_MAC2;
	eh->h_dest[3] = MY_SRC_MAC3;
	eh->h_dest[4] = MY_SRC_MAC4;
	eh->h_dest[5] = MY_SRC_MAC5;
	eh->h_proto  = htons(ETH_P_IP);

	memcpy(eth_snd_buf, eh, sizeof(struct ethhdr));
	memcpy(eth_snd_buf + sizeof(struct ethhdr), send_buf,
	 					strlen(buf) + sizeof(struct iphdr) + sizeof(struct udphdr));

	memset(&socket_address, 0, sizeof(struct sockaddr_ll));
	socket_address.sll_family   = PF_PACKET;
	socket_address.sll_protocol = htons(ETH_P_IP);
	socket_address.sll_ifindex  = if_nametoindex("eth1");
	socket_address.sll_halen    = ETH_ALEN;

	socket_address.sll_addr[0] = MY_SRC_MAC0;
	socket_address.sll_addr[1] = MY_SRC_MAC1;
	socket_address.sll_addr[2] = MY_SRC_MAC2;
	socket_address.sll_addr[3] = MY_SRC_MAC3;
	socket_address.sll_addr[4] = MY_SRC_MAC4;
	socket_address.sll_addr[5] = MY_SRC_MAC5;

	printf("\n");
        printf("Ethernet Header\n");
        printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
                 eh->h_dest[0], eh->h_dest[1], eh->h_dest[2],
                 eh->h_dest[3], eh->h_dest[4], eh->h_dest[5]);
        printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
                 eh->h_source[0], eh->h_source[1], eh->h_source[2],
                 eh->h_source[3], eh->h_source[4], eh->h_source[5]);
        printf("   |-Protocol            : %u \n",(unsigned short)eh->h_proto);

	printf("IP Header\n");
        printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
        printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",
                        (unsigned int)iph->ihl,((unsigned int)iph->ihl*4));
        printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",
                                                        ntohs(iph->tot_len));
        printf("   |-TTL               : %d\n",(unsigned int)iph->ttl);
        printf("   |-Protocol          : %d\n",(unsigned int)iph->protocol);
        printf("   |-Checksum          : %d\n", ntohs(iph->check));

	memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = iph->saddr;

        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = iph->daddr;

        printf("   |-Source IP         : %s\n", inet_ntoa(source.sin_addr));
        printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));

	printf("\nUDP Header\n");
        printf("   |-Source Port      : %d\n", ntohs(udph->source));
        printf("   |-Destination Port : %d\n", ntohs(udph->dest));
        printf("   |-UDP Length       : %d\n", ntohs(udph->len));
        printf("   |-UDP Checksum     : %d\n", ntohs(udph->check));
	printf("\n");



	n_sent = sendto(sk, eth_snd_buf, 256, 0,
		(struct sockaddr*) &socket_address, sizeof(socket_address));

	if (n_sent<0) {
		perror("Problem sending data");
		exit(1);
	}

	memset(buf, 0, 40);

	while(1){
		memset(send_buf, 0, 256);

		n_read = recvfrom(sk,send_buf,256,0,NULL,NULL);
		if (n_read<0) {
			perror("Problem in recvfrom");
			exit(1);
		}
		eh = (struct ethhdr *) send_buf;
		iph = (struct iphdr*) (send_buf + sizeof(struct ethhdr));
		udph = (struct udphdr*) (send_buf + iph->ihl * 4 + sizeof(struct ethhdr));
		if(iph->protocol == 17){
			if(ntohs(udph->dest) == MY_PORT)
				 break;
		}
	}

	memcpy(buf, send_buf + sizeof(struct udphdr) + iph->ihl*4 + sizeof(struct ethhdr), 40);

	printf("\n");
        printf("Ethernet Header\n");
        printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
                 eh->h_dest[0], eh->h_dest[1], eh->h_dest[2],
                 eh->h_dest[3], eh->h_dest[4], eh->h_dest[5]);
        printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
                 eh->h_source[0], eh->h_source[1], eh->h_source[2],
                 eh->h_source[3], eh->h_source[4], eh->h_source[5]);
        printf("   |-Protocol            : %u \n",(unsigned short)eh->h_proto);

	printf("IP Header\n");
        printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
        printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",
                        (unsigned int)iph->ihl,((unsigned int)iph->ihl*4));
        printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",
                                                        ntohs(iph->tot_len));
        printf("   |-TTL               : %d\n",(unsigned int)iph->ttl);
        printf("   |-Protocol          : %d\n",(unsigned int)iph->protocol);
        printf("   |-Checksum          : %d\n", ntohs(iph->check));
	memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = iph->saddr;

        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = iph->daddr;

        printf("   |-Source IP         : %s\n", inet_ntoa(source.sin_addr));
        printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
	printf("\nUDP Header\n");
        printf("   |-Source Port      : %d\n", ntohs(udph->source));
        printf("   |-Destination Port : %d\n", ntohs(udph->dest));
        printf("   |-UDP Length       : %d\n", ntohs(udph->len));
        printf("   |-UDP Checksum     : %d\n", ntohs(udph->check));


	printf("Got back %d bytes\n",n_read);
	printf("%s\n", buf);

	return 0;
}
