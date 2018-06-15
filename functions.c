/* general functions */


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
#include <errno.h>

#include <syslog.h>
#include <sys/time.h>
 

#define SET     0
#define UNSET   1


	
extern int rec;
extern char if_name[256];
extern int outputlog;


char *inetaddr ( u_int32_t ip ) {
  	struct in_addr in;
    	in.s_addr = ip;   
      	return inet_ntoa(in);
      	}

char *hwaddr (u_char * s) {
   	static char buf[30];
     	sprintf (buf, "%02X:%02X:%02X:%02X:%02X:%02X", s[0], s[1], s[2], s[3], s[4],s[5]);
        return buf;
        }
       
void logme (char *heh) {
	
	if (outputlog) {
		fprintf (stderr,"ArpMonitor: ");
		fprintf (stderr,"%s\n", heh);
	} else {
        	syslog (LOG_INFO, "%s", heh);
	}
}
       
void perr_syslog (char *heh) {
	if (outputlog) {
		fprintf (stderr,"ArpMonitor:");
		fprintf (stderr,"%s\n", heh);
	} else {
        	syslog (LOG_INFO, "error:%s", heh);
    	}
}

void promisc_mode (int rec, int status, char *if_name) {
	
	struct ifreq if_data;
	
	strncpy (if_data.ifr_name, if_name, strlen(if_name));
	
	/* added 09/28/2001 - unset promisc mode */
	if (ioctl (rec, SIOCGIFFLAGS, &if_data) < 0) {
		fprintf (stderr, "couldn`t get flags: %d %s\n", errno, strerror(errno));
		return;
	}
	if (status == SET) if_data.ifr_flags |= IFF_PROMISC;
	if (status == UNSET) if_data.ifr_flags &= ~IFF_PROMISC;
	if (ioctl (rec, SIOCSIFFLAGS, &if_data) < 0) {
		fprintf (stderr, "couldn`t set/unset promisc mode in %d: %d - %s\n", __LINE__, errno, strerror(errno));
	}

}
/* Lazy Ass inc, sorry about the delay :D */


void recvsignal (int sig) {
	logme("received SIGINT, giving up\n");
	promisc_mode(rec, UNSET, if_name);
	if (!outputlog) closelog();
	close (rec);
	exit(0);
	}


void usage (void) {

	fprintf (stderr,"Usage mode:\n");

	fprintf (stderr,"./arpmonitor -i <interface> -s|e -p\n");

	fprintf (stderr,"-i <interface> , interface to check\n");
	fprintf (stderr,"-s or -e , output: <s>yslog or std<e>rr - default: syslog\n");
	fprintf (stderr,"-p setup promisc mode (auto clean on ctrl-C) - default: no promisc mode\n");
	
	exit(0);
}

