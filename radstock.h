/*
 * $Id: radstock.h,v 1.4 2000/10/24 18:02:35 withnail Exp $
 *
 */

#define ETHHDR_SIZE 14 
#define PPPHDR_SIZE 4 
#define SLIPHDR_SIZE 16
#define RAWHDR_SIZE 0
#define LOOPHDR_SIZE 4 
#define FDDIHDR_SIZE 21

#ifndef IP_OFFMASK
#define IP_OFFMASK 0x1fff
#endif

#define WORD_REGEX "((^%s\\W)|(\\W%s$)|(\\W%s\\W))"
#define IP_ONLY "udp and ( %s)"

/* Random variables used by data structures */
#define LBUFSIZE 16384
#define BUFSIZE  120
#define HDRBUFSIZE 256
#define ATTRSIZE 1024
#define WATCHMAX 67
/* Watchlist timeout in seconds */
#define WATCHLIST_TIMEOUT  20

#define RADAT_INT  1
#define RADAT_IP   2
#define RADAT_STR  3

#define RADPKT_AUTH_REQUEST   1
#define RADPKT_AUTH_ACCEPT    2
#define RADPKT_AUTH_REJECT    3
#define RADPKT_ACCT_REQUEST   4
#define RADPKT_ACCT_RESPONSE  5
#define RADPKT_ACCT_CHALLENGE 11
#define RADPKT_STATUS_SERVER  12
#define RADPKT_STATUS_CLIENT  13
#define RADPKT_DISC_REQ       40
#define RADPKT_DISC_ACK       41
#define RADPKT_DISC_NAK       42

struct tree_elem
{
  int operator;
  int lside, rside;
};

struct s_tree
{
  int root;
  struct tree_elem elem[100];
};


struct attribs 
{
  char name[BUFSIZE];
  unsigned long int op, value;
  unsigned long int vala;
  union {
     unsigned long int  valb;
     regex_t *valbp;
  };
}; 

struct t_watchlist
{
  unsigned long int saddr, sport;
  unsigned char ident;
  unsigned long int timeout;
};

struct t_watchlist watchlist[WATCHMAX];
struct attribs filters[16];
struct attribs radattrs[ATTRSIZE];
struct attribs radvals[ATTRSIZE];

extern int t_radfilters;
extern struct in_addr net, mask;

extern int show_auth, show_acct, show_req, show_resp, show_invalid;

extern int link_offset;
extern int quiet;
extern int debug;
extern char *(*print_time_fn)();
extern struct s_tree tree;

char *get_filter(char **);
void process(u_char *, struct pcap_pkthdr*, u_char *);
void dump(char *, int); 
void clean_exit(int);
void usage(int);
void version(void);

int re_match_func(char *, int); 
int bin_match_func(char *, int);
int blank_match_func(char *, int); 

int strishex(char *);

char *print_time(struct pcap_pkthdr *);
char *print_time_date(struct pcap_pkthdr *);
void print_time_diff(struct pcap_pkthdr *, char *);

void dump_delay(struct pcap_pkthdr *);

/* Found in init.c */
int cmp_attrs(const void *a, const void *b);
int readdict(char *dictfile);
void get_radfilter(char *arg);
struct attribs *find_radvals_by_value(int attrib, int lookupval);
struct attribs *find_radattr_by_value(int lookupval);
