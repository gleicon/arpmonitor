/* 
 * arpmonitor	-	log any arp [requests|reply] that comes to the 
 *			local ethernet 
 * 			primary use: debugging.
 * gleicon - 98
 *
 */

#include "main.h"

int rec; /* global fd */
char if_name[256]; /* global iface name, 256 should be enough */
int outputlog=0;

int main (int argc, char **argv) {
	
	int 	len, rsflags, c, promisc_flag=0;
	socklen_t from_len;
	struct 	ifreq		if_data;
	struct 	sockaddr	from;
	char temp_buf[1024];
	char temp_buf2[1024];
	char temp_buf3[1024];

	
	u_int8_t		myMAC[6];
	u_int32_t		ip, sip;



	signal (SIGINT, recvsignal);


	memset (&if_name, 0 , sizeof(if_name));
	memset (&if_data, 0, sizeof (struct ifreq));
	
	while ((c = getopt (argc, argv, OPTSTR)) != EOF) {
	 	switch (c) {
	        	case 'i':
			if (strlen(optarg) < 1) usage();
			strncpy(if_name, optarg, strlen(optarg));
			break;
	        	
			case 's':
			outputlog=0;
			break;
	        	
			case 'e':
			outputlog=1;
			break;
		
	        	case 'p':
			promisc_flag=1;
			break;
			
			case 'h':
			default:
			fprintf (stderr,"This is ArpMonitor\n");
			usage();
			break;
		}									 
	 }
	
	
	
	
	if (getuid ()) {
		fprintf (stderr,"This program must run as root\n");
		exit (0);
		}
	
	if (*(if_name) == 0) {
		fprintf (stderr," -i ????\n");
		usage();
		}
		
	//if ((rec = socket (PF_INET, SOCK_PACKET, htons (ETH_P_ARP))) < 0) {
	if ((rec = socket (PF_INET, SOCK_RAW, htons (ETH_P_ARP))) < 0) {
		syslog(LOG_INFO,"Socket. Aborting.\n");
	        perror("socket");
	        exit (0);
		}
	
	fprintf(stderr, "Iface: %s fd: %d\n", if_name, rec);
	
	strncpy(if_data.ifr_name , if_name, strlen(if_name));

	if (!outputlog)  openlog ("arpmon",LOG_PID, LOG_AUTHPRIV);
	
	logme("Initializing...");
	        

	/* added 09/28/2001 - set promisc mode */

	if (promisc_flag)  promisc_mode(rec, SET, if_name);	

	/* Lazy Ass inc, sorry about the delay :D */


	if (ioctl (rec, SIOCGIFHWADDR, &if_data) < 0) {
		fprintf (stderr,"can't get hardware address\n");
		exit(1);
		}
 	
 	memcpy (myMAC, if_data.ifr_hwaddr.sa_data, 6);
 	
 	sprintf(temp_buf3, "HW address: %s",  hwaddr (myMAC));
	logme (temp_buf3);

 	
 	if (ioctl (rec, SIOCGIFADDR, &if_data) < 0) {
 		fprintf (stderr,"can't get IP address\n");
 		exit(1);
 		}
 	
	memcpy ((void *) &ip, (void *) &if_data.ifr_addr.sa_data + 2, 4);

 	sprintf(temp_buf3, "IP address: %s",  inetaddr(ip));
	logme (temp_buf3);
	
	
	
	if (ioctl (rec, SIOCGIFNETMASK, &if_data) < 0) {
		fprintf (stderr,"can't get NETMASK\n");
		}
	memcpy ((void *) &ip, (void *) &if_data.ifr_netmask.sa_data + 2, 4);

 	sprintf(temp_buf3, "Netmask : %s",  inetaddr(ip));
	logme (temp_buf3);

	if (ioctl (rec, SIOCGIFBRDADDR, &if_data) < 0) {
		fprintf (stderr,"can't get BROADCAST\n");
		}
	
	memcpy ((void *) &ip, (void *) &if_data.ifr_broadaddr.sa_data + 2, 4);
 	
	sprintf(temp_buf3, "Broadcast : %s",  inetaddr(ip));
	logme (temp_buf3);
        
	
	if ((rsflags = fcntl (rec, F_GETFL)) == -1) {
		logme("Fcntl(F_GETFL) . Aborting.");
	        perror ("fcntl F_GETFL");
	        exit (1);
	        }
	                                                                
	if (fcntl (rec, F_SETFL, rsflags | O_NONBLOCK) == -1) {
		logme("Fcntl(F_SETFL) . Aborting.");
	        perror ("fcntl F_SETFL");
	        exit (1);
         	}
	
	        strncpy(from.sa_data, if_name, strlen(if_name));
	        from.sa_family = 1;
	
	for (;;) {
	
		bzero(full_packet, MAX_PACK_LEN);
		
	        // dat magic timeout
		usleep(50);
	        
	        len = recvfrom (rec, full_packet, MAX_PACK_LEN, 0, &from, &from_len);
	       	
		if (len == 0) {
			// waits a bit longer, nothing came from the network
			usleep(100);
			continue;
		} 
	        
	        if (ntohs (arp_pkt.arp_op) == ARPREPLY) {
	        		
	        	memcpy (&ip, arp_pkt.target_ip, 4);
	        	memcpy (&sip, arp_pkt.sender_ip, 4);
	               	sprintf (temp_buf, "[Reply] From: %s [%s] ",inetaddr (sip),hwaddr (arp_pkt.sender_eth));
	        	sprintf (temp_buf2, "To: %s [%s]\n",inetaddr (ip), hwaddr (arp_pkt.target_eth)); 
	        	
			
			if (outputlog) fprintf (stderr,"%s%s\n",temp_buf, temp_buf2);
	        	else syslog (LOG_INFO,"%s%s", temp_buf, temp_buf2);
	        	
	        	}
	        if (ntohs (arp_pkt.arp_op) == ARPREQUEST) {
	        	
	        	memcpy (&ip, arp_pkt.target_ip, 4);
	        	memcpy (&sip, arp_pkt.sender_ip, 4);
	               	sprintf (temp_buf,"[Request] From: %s [%s] ", inetaddr (sip), hwaddr (arp_pkt.sender_eth));
	        	sprintf (temp_buf2,"To: %s [%s]",  inetaddr (ip),  hwaddr (arp_pkt.target_eth));
	        	
			
			if (outputlog) fprintf (stderr,"%s%s\n",temp_buf, temp_buf2);
	        	else syslog (LOG_INFO, "%s%s",temp_buf, temp_buf2);
	        	}
	                                   
	}

	if (promisc_flag) promisc_mode(rec, UNSET, if_name);	
	
	close (rec); 
    	if (!outputlog) closelog();
}


