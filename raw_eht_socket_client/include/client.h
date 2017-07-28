#include <stdio.h>      /* standard C i/o facilities */
#include <stdlib.h>     /* needed for atoi() */
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include <string.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>


#define MAXBUF 40
#define SERVER_PORT 7373
#define MY_PORT 3333
