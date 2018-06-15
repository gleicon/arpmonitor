/* main includes and defs */


#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <malloc.h>
#include <ctype.h>
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <netinet/tcp.h>
#include <net/if.h>   
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>  
#include <sys/time.h>

#define OPTSTR "hpsei:"

#include "functions.h"


#define ETH_P_ARP       0x0806
#define MAX_PACK_LEN    2000
#define ETHER_HEADER_LEN 14
#define ARPREQUEST      1
#define ARPREPLY        2
#define perr(s) fprintf(stderr,"%s", s)

struct arp_struct {
	u_char  dst_mac[6];
        u_char  src_mac[6];
        u_short pkt_type;  
        u_short hw_type;   
        u_short pro_type;  
        u_char  hw_len;    
        u_char  pro_len;   
        u_short arp_op;    
        u_char  sender_eth[6];
        u_char  sender_ip[4]; 
        u_char  target_eth[6];
        u_char  target_ip[4]; 
        };
        
                                                  
union {
    	u_char full_packet[MAX_PACK_LEN];
        struct arp_struct arp_pkt;
        } a;
          
#define full_packet a.full_packet
#define arp_pkt a.arp_pkt

#define SET	0
#define UNSET	1


