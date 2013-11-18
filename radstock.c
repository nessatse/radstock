/*
 * $Id: radstock.c,v 1.8 2000/10/26 15:16:42 withnail Exp $
 *
 * Please refer to the COPYRIGHT file for more information. 
 * 
 */

#if defined(BSD) || defined(SOLARIS)
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <net/if.h>
#include <regex.h>
#include <fcntl.h>
#endif

#if defined(OSF1)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <net/route.h>
#include <sys/mbuf.h>
#endif

#if defined(LINUX)
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <regex.h>
#endif


#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#include <pcap.h>
#include <net/bpf.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "radstock.h"
//#include "version.h"

static char rcsid[] = 
  "$Id: radstock.c,v 1.8 2000/10/26 15:16:42 withnail Exp $";


//#define DICTFILE "/usr/local/etc/raddb/dictionary"

char defpcapfilter[] = "udp and port 1645";

int snaplen = 65535, promisc = 1, to = 1000;
int show_empty = 0, show_hex = 0;
int match_after = 0, keep_matching = 0;
int invert_match = 0, bin_match = 0;
int matches = 0, max_matches = 0;
int live_read = 1, want_delay = 0;;

char pc_err[PCAP_ERRBUF_SIZE], *re_err;

int re_match_word = 0, re_ignore_case = 0;

char *match_data = NULL, *bin_data = NULL, *filter = NULL;
int match_len = 0;
int debug = 0, quiet = 0;
struct bpf_program pcapfilter;
struct in_addr net, mask;
pcap_t *pd = NULL;
char *dev = NULL;
int link_offset;

int show_auth = 1;
int show_acct = 1;
int show_req  = 1;
int show_resp = 1;

char *read_file = NULL, *dump_file = NULL, *dict_file = NULL;
pcap_dumper_t *pd_dump = NULL;

struct timeval prev_ts = {0, 0}, prev_delay_ts = {0,0};
void (*print_time_fn)() = NULL;



int main(int argc, char **argv) {
  int c;
  char *radfilterfile = NULL;
  char radfilterbuf[1000] = "\0";

  signal(SIGINT,  clean_exit);
  signal(SIGQUIT, clean_exit);
  signal(SIGABRT, clean_exit);
  signal(SIGPIPE, clean_exit);


  debug = 0; quiet = 0;
  /* 
   * Lets allocate a temporary buffer to store the expression in.
   * Then, we can fill this buffer from whatever means we like
   * STDIN, file, or optarg, and treat it exactly the same 
   */
  if (debug) printf("Parsing switches\n");
  while ((c = getopt(argc, argv, "KLMNpd:hXVi:c:wqe:vxlDtTn:i:A:I:O:f:")) != EOF) {
    switch (c) {
    case 'K':
      if (show_auth == 1) show_acct = 0;
      break;
    case 'L':
      if (show_acct == 1) show_auth = 0;
      break;
    case 'M':
      if (show_req == 1) show_resp = 0;
      break;
    case 'N':
      if (show_resp == 1) show_req = 0;
      break;
    case 'd':
      if (!isdigit(*optarg))
      {
	printf("error: switch -d requires an integer value\n");
	clean_exit(-4);
      }
      debug = atoi(optarg);
      break;
    case 'p':
      promisc = 0;
    case 'f':
      dict_file = optarg;
      break;
    case 'I':  
      read_file = optarg;
      break;
    case 'O':
      dump_file = optarg;
      break;
    case 'A': 
      match_after = atoi(optarg) + 1;
      break;
    case 'i': 
      dev = optarg;
      break;
    case 'n':
      max_matches = atoi(optarg);
      break;
    case 'T':
      print_time_fn = &print_time;
      break;
    case 'D':
      print_time_fn = &print_time_date;
      break;
    case 'c':
      radfilterfile = optarg;
      break;
    case 't':
      //print_time = &print_time_absolute;
      break;
    case 'l':
      setvbuf(stdout, NULL, _IOLBF, 0);
      break;
    case 'x':
      show_hex++;
      break;
    case 'v':
      invert_match++;
      break;
    case 'e':
      strcpy(radfilterbuf,optarg);
      /* Used to be --- show_empty++; */
      break;
    case 'q':
      quiet++;
      break;
    case 'w':
      re_match_word++;
      break;
    case 'V':
      version();
    case 'X':
      bin_match++;
      break;
    case 'h':
      usage(0);
    }
  }
  if (debug) printf("Reading dictionary\n");

  if (dict_file) 
  {
    if (readdict(dict_file) == 0)
    {
      printf("Cannot open dictionary file %s\n", dict_file);
      exit(0);
    }
  }
  else if (readdict(DICTFILE) == 0)
  {
    printf("Cannot open dictionary file %s\n", DICTFILE);
    exit(0);
  }

  /*
   * Ok - lets grab the expression now
   * (arbitrary decision - if expression on cmd line
   *  ignore the file argument)
   */
  if (radfilterfile)
  {
    int fd, nbytes = 1;
    char *bufptr, *envptr;

    /* We have a cmd filter */
    if (strcmp(radfilterfile, "-") == 0)
      fd = 0;
    else if (isalnum(*radfilterfile) && (envptr = getenv("RADSTOCK_PATH"))) {
      char tmpbuf[255];
      /* 
       * We'll check the RADSTOCK_PATH 
       */
      strcpy(radfilterbuf, envptr);
      bufptr = strtok(radfilterbuf, ":");
      while (bufptr && (fd < 0))
      {
	sprintf(tmpbuf, "%s/%s", bufptr, radfilterfile);
	if (debug) printf("Checking for file '%s'\n", tmpbuf);
	fd = open(tmpbuf, O_RDONLY);
	if (fd > 0)
	  printf("Config file found : %s\n", tmpbuf);
	else
	  bufptr = strtok(NULL, ":");
      }
    }
    else {
      fd = open(radfilterfile, O_RDONLY);
      if (fd > 0) printf("Config file found : %s\n", radfilterfile);
    }

    if (fd < 0)
    {
      printf("Cannot open config file\n");
      clean_exit(-1);
    }

    bufptr = radfilterbuf;
    while (nbytes)
    {
      nbytes =read(fd, (void *) bufptr, 1000);
      bufptr += nbytes;
    }
    *bufptr = '\0';
    if (fd) close(fd);
  }


  if (debug) printf("Handling radius filter '%s'\n", radfilterbuf);
  if (*radfilterbuf)
  {
    get_radfilter(radfilterbuf); 
    memset(watchlist, WATCHMAX * sizeof(struct t_watchlist), 0);
  }
  if (debug) printf("Done radius filter '%s'\n", radfilterbuf);

  //clean_exit(-2);

  if (read_file) {
    if (!(pd = pcap_open_offline(read_file, pc_err))) {
      perror(pc_err);
      clean_exit(-1);
    }

    live_read = 0;
    printf("input: %s\n", read_file);

  } else {
    if (!dev)
      if (!(dev = pcap_lookupdev(pc_err))) {
	perror(pc_err);
	clean_exit(-1);
      }
    
    if ((pd = pcap_open_live(dev, snaplen, promisc, to, pc_err)) == NULL) {
      perror(pc_err);
      clean_exit(-1);
    }

    if (pcap_lookupnet(dev, &net.s_addr, &mask.s_addr, pc_err) == -1) {
      perror(pc_err);
      memset(&net, 0, sizeof(net));
      memset(&mask, 0, sizeof(mask));
    } 

    if (!quiet) {
      printf("interface: %s", dev);
      if (net.s_addr && mask.s_addr) {
	printf(" (%s/", inet_ntoa(net));
	printf("%s)", inet_ntoa(mask)); 
      }
      printf("     ");
    }
  }


  if (argv) {
    filter = get_filter(&argv[optind]); 
    if (debug && !quiet) printf("filter1: %s\n", filter); 
    if (pcap_compile(pd, &pcapfilter, filter, 0, mask.s_addr)) {
      free(filter); 
      filter = get_filter(&argv[optind-1]); 
    if (debug && !quiet) printf("filter2: %s\n", filter); 

  //    pcap_restart();
      if (pcap_compile(pd, &pcapfilter, filter, 0, mask.s_addr)) {
	pcap_perror(pd, "pcap compile");
	clean_exit(-1);
      } else match_data = NULL;
    }

    if (!quiet) printf("filter: %s\n", filter); 
    
    if (pcap_setfilter(pd, &pcapfilter)) {
      pcap_perror(pd, "pcap set");
      clean_exit(-1);
    }
  }

  if (filter) free(filter);

  switch(pcap_datalink(pd)) {
  case DLT_EN10MB:
  case DLT_IEEE802:
    link_offset = ETHHDR_SIZE;
    break;

  case DLT_FDDI:
    link_offset = FDDIHDR_SIZE;
    break;

  case DLT_SLIP: 
    link_offset = SLIPHDR_SIZE;
    break;
    
  case DLT_PPP:
    link_offset = PPPHDR_SIZE;
    break;

  case DLT_RAW: 
    link_offset = RAWHDR_SIZE;
    break;

  case DLT_NULL:
    link_offset = LOOPHDR_SIZE;
    break;

  default:
    fprintf(stderr, "fatal: unsupported interface type\n");
    clean_exit(-1);
  }
  
  while (pcap_loop(pd, 0, (pcap_handler)process, 0));

  clean_exit(0);
  return 0;
}	


char *get_filter(char **argv) {
  char **arg = argv, *theirs, *mine;
  char *from, *to;
  int len = 0;

  if (!*arg)
  {
    len = strlen(defpcapfilter);
    if (!(mine = (char *) malloc(len+1)))
      return NULL;

    strcpy(mine, defpcapfilter);
    return mine;
  }

  while (*arg)
    len += strlen(*arg++) + 1;

  if (!(theirs = (char*)malloc(len + 1)) || 
      !(mine = (char*)malloc(len + sizeof(IP_ONLY))))
    return NULL;

  memset(theirs, 0, len + 1);
  memset(mine, 0, len + sizeof(IP_ONLY));

  arg = argv;
  to = theirs;
  while (*arg) 
  {
    from = *arg;
    while ((*to++ = *from++));
    *(to-1) = ' ';
    arg++;
  }

  sprintf(mine, IP_ONLY, theirs);

  free(theirs);
  return mine;
}


void clean_exit(int sig) {
  struct pcap_stat s;
  if (!quiet && sig >= 0) printf("exit\n");

  if (bin_data) free(bin_data);
  
  if (!quiet && sig >= 0 && !read_file &&
      pd && !pcap_stats(pd, &s)) 
    printf("%d received, %d dropped\n", s.ps_recv, s.ps_drop);

  if (pd) pcap_close(pd);

  exit(sig);
}


int strishex(char *str) {
  char *s;
  if ((s = strchr(str, 'x'))) 
    s++;
  else s = str;

  while (*s) 
    if (!isxdigit(*s++))
      return 0;

  return 1;
}



void print_time(struct pcap_pkthdr *h, char *buf) 
{
  struct tm *t = localtime((time_t *) &h->ts.tv_sec);

  sprintf(buf+79-10, "[%02d:%02d:%02d]\n",
	  t->tm_hour, t->tm_min, t->tm_sec);
}


void print_time_date(struct pcap_pkthdr *h, char *buf) 
{
  struct tm *t = localtime((time_t *) &h->ts.tv_sec);

  sprintf(buf+79-19, "[%02d/%02d/%02d %02d:%02d:%02d]\n",
         t->tm_mday, t->tm_mon, t->tm_year % 100, t->tm_hour,
         t->tm_min, t->tm_sec);
}


void print_time_diff(struct pcap_pkthdr *h, char *buf) { 
  unsigned secs, usecs;

  if (!prev_ts.tv_sec && !prev_ts.tv_usec) {// uninit
    prev_ts.tv_sec = h->ts.tv_sec;
    prev_ts.tv_usec = h->ts.tv_usec;
  }

  secs = h->ts.tv_sec - prev_ts.tv_sec;
  if (h->ts.tv_usec >= prev_ts.tv_usec)
    usecs = h->ts.tv_usec - prev_ts.tv_usec;
  else {
    secs--; 
    usecs = 1000000 - (prev_ts.tv_usec - h->ts.tv_usec);
  }

  printf("+%d.%06d ", secs, usecs);

  prev_ts.tv_sec = h->ts.tv_sec;
  prev_ts.tv_usec = h->ts.tv_usec;
}


void dump_delay(struct pcap_pkthdr *h) {
  unsigned secs, usecs;

  if (!prev_delay_ts.tv_sec && !prev_delay_ts.tv_usec) {
    prev_delay_ts.tv_sec = h->ts.tv_sec;
    prev_delay_ts.tv_usec = h->ts.tv_usec;
  }

  secs = h->ts.tv_sec - prev_delay_ts.tv_sec;
  if (h->ts.tv_usec >= prev_delay_ts.tv_usec)
    usecs = h->ts.tv_usec - prev_delay_ts.tv_usec;
  else {
    secs--; 
    usecs = 1000000 - (prev_delay_ts.tv_usec - h->ts.tv_usec);
  }

  sleep(secs);
  usleep(usecs);
  
  prev_delay_ts.tv_sec = h->ts.tv_sec;
  prev_delay_ts.tv_usec = h->ts.tv_usec;
}


void usage(int e) {
  printf("usage: radstock <options> <bpf filter>\n");
  printf("          -h      - display this page\n");
  printf("          -f file - define location of dictionary\n");
  printf("          -i intf - listen on given interface\n");
  printf("          -d lvl  - enable debugging output\n");
  printf("          -q      - quiet mode - do not show packet contents\n");
  printf("          -p      - run in non-promiscuous mode\n");
  printf("          -T      - print the time for each packet\n");
  printf("          -D      - print the date and time for each packet\n");
  printf("          -K      - only show authentication packets\n");
  printf("          -L      - only show accounting packets\n");
  printf("          -M      - only show request packets\n");
  printf("          -N      - only show response packets\n");
  printf("          -V      - show the version number\n");
  printf("          -e exp  - radius filter expression\n");
  printf("          -c file - read radius filter from file (- = stdin)\n");
  printf("\ni.e. radstock -e \"user-name = henry\" port 1645\n\n");

  exit(e);
}


void version(void) {
  printf("radstock: version %s\n", VERSIONSTR);
  exit(0);
}

