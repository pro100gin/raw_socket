#define __USE_BSD
#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _POSIX_C_SOURCE
#define _XOPEN_SOURCE
#define _SVID_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE
#define _DEFAULT_SOURCE

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
#include <netinet/ip_icmp.h>
#include <netinet/in_systm.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netdb.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>

#define MAXBUF 40
#define SERVER_PORT 7373
#define MY_PORT 3333


void ping(struct sockaddr_in *, unsigned char *);

void rcv_pct();

void pr_pack(char *, int , struct sockaddr_in *);

unsigned short compute_checksum(unsigned short *, unsigned int );
