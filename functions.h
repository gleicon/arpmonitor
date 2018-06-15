/* prototipes */


void promisc_mode (int , int , char *);
char *inetaddr ( u_int32_t );
char *hwaddr (u_char * );
void logme (char *);
void perr_syslog (char *);
void warn_mail (char *, char *);
void recvsignal (int );
void usage(void);


