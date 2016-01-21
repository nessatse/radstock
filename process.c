/*
 * $Id: process.c,v 1.7 2000/10/26 14:43:42 withnail Exp $
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
#include <arpa/inet.h>
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
#include <pcap/bpf.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "radstock.h"
#include "y.tab.h"

static char rcsid[] = "$Id: process.c,v 1.7 2000/10/26 14:43:42 withnail Exp $";
int t_watchptr = 0;
int t_radfilters = 0;

#define FIL_UNDEF -4

size_t strlcat(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0')
       d++;
    dlen = d - dst;
    n = siz - dlen;

    if (n == 0)
       return(dlen + strlen(s));
    while (*s != '\0') {
       if (n != 1) {
           *d++ = *s;
           n--;
       }
       s++;
    }
    *d = '\0';

    return(dlen + (s - src));	/* count does not include NUL */
}

int parse_packet(int curpos)
{
  int lresult = 0, rresult = 0, result = 0;

  if (debug & 16)
  fprintf(stderr, 
	  "parse_packet: curpos=%d lside=%d rside=%d  op=%d\n",
	  curpos, tree.elem[curpos].lside, tree.elem[curpos].rside,
	  tree.elem[curpos].operator);

  if (tree.elem[curpos].operator == LEX_TERM)
  {
    int attrib, abc;

    abc = tree.elem[curpos].lside;
    attrib = filters[abc].vala;
    result = filters[abc].value;
    if (result == FIL_UNDEF)
    {
      if (filters[abc].op == LEX_NEXIST)
	result = 1;
      else 
	result = 0;
    }
    if (debug & 16)
      fprintf(stderr, 
	      "parse_packet: curpos=%d op=%lu term=%d attrib=%d value=%d\n",
	      curpos, filters[abc].op, tree.elem[curpos].lside, 
	      attrib, result);
  }
  else {
    if (tree.elem[curpos].lside != -1) 
      lresult = parse_packet(tree.elem[curpos].lside);
    if (tree.elem[curpos].rside != -1)
      rresult = parse_packet(tree.elem[curpos].rside);

    if (lresult == FIL_UNDEF || rresult == FIL_UNDEF)
      result = FIL_UNDEF;
    else 
    switch (tree.elem[curpos].operator)
    {
    case LEX_AND:
      result = (lresult & rresult);
      break;
      
    case LEX_OR:
      result = (lresult | rresult);
      break;
    case LEX_NOT:
      result = !lresult;
      break;
    default:
      fprintf(stderr, "parse_packet: error with operator %d\n",
	      tree.elem[curpos].operator);
    }
  }
  
  if (debug & 16)
  fprintf(stderr, 
	  "parse_packet: curpos=%d lresult=%d rresult=%d  result=%d\n",
	  curpos, lresult, rresult, result);
  //showpkt = (t_radfilters == 0) || (t_radfilters - filter_matches() == 0); 
  return result;
}


inline int watchlist_check(char pkt_type, unsigned char ident, 
			   int saddr, int sport)
{
  int lp, startpos;

  if (t_radfilters == 0) return 0;
  // If it is a request packet it cannot really match on the watchlist
  if (pkt_type == RADPKT_AUTH_REQUEST || pkt_type == RADPKT_ACCT_REQUEST)
    return 0;

  startpos = ((unsigned int) saddr + ident) % WATCHMAX;

  if (debug)
    fprintf(stderr, "watchlist_check: checking %d (%d) startpos = %d\n",
	      saddr, sport, startpos);
  lp = (startpos + 1) % WATCHMAX;
  for (; lp != startpos ; lp = (lp + 1) % WATCHMAX)
  {
    if (watchlist[lp].sport != (unsigned long)sport) continue;
    if (watchlist[lp].saddr != (unsigned long)saddr) continue;
    if (watchlist[lp].ident != ident) continue;

    if (debug)
      fprintf(stderr, "watchlist_check: found at pos = %d\n", lp);
    /* We have a match - zero the port so it wont match anything else */
    watchlist[lp].sport = 0;
    return 1;
  }
  return 0;
}

int watchlist_add(unsigned char ident, int saddr, int sport)
{
  int timeout, lp, startpos;

  if (debug)
    fprintf(stderr, "watchlist_add: adding %d (%d)\n",
	      saddr, sport);
  // Find the hashed position in the watchlist - better hash plz
  timeout = time(NULL);
  startpos = ((unsigned int) saddr + ident) % WATCHMAX;
  lp = (startpos + 1) % WATCHMAX;
  while (startpos != lp) 
  {
    if (watchlist[lp].sport   == 0)       break;
    if (watchlist[lp].timeout <= (unsigned long)timeout) break;

    //printf(" t_watchptr %d lp %d\n", t_watchptr, lp);
    if (debug)
      fprintf(stderr, "watchlist_add: startpos = %d curpos = %d  max = %d\n",
	      startpos, lp, WATCHMAX);
    lp = (lp + 1) % WATCHMAX;
  }
  if (debug)
  {
    fprintf(stderr, "watchlist_add: storing pos = %d port = %lu timeout = %lu\n",
	    lp, watchlist[lp].sport, watchlist[lp].timeout);
  }
  watchlist[lp].ident = ident;
  watchlist[lp].saddr = saddr;
  watchlist[lp].sport = sport;
  watchlist[lp].timeout = timeout + WATCHLIST_TIMEOUT;

  return 0;
}

inline void reset_filters()
{
  int lp;

  for (lp = 0; lp < t_radfilters; lp++)
    filters[lp].value = FIL_UNDEF;
}

inline int filter_matches()
{
  int lp, count = 0, v;

  if (!t_radfilters) return -1;

  for (lp = 0; lp < t_radfilters; lp++)
  {
    v = filters[lp].value;
    if (v == FIL_UNDEF && filters[lp].op == LEX_NEXIST)
    {
      count++;
      continue;
    }
    if (v != 0 && v != FIL_UNDEF)
      count++;
  }
  return count;
}

void match_attr(int attrib, int attr_type, void *data)
{
  int lp, value, result;

  for (lp = 0; lp < t_radfilters; lp++)
  {
    if (filters[lp].vala != (unsigned long)attrib) continue;
    if (debug > 5) printf("match_attr: Matched attrib %ld\n", 
			  filters[lp].vala);
    if (filters[lp].op == LEX_EXIST) 
    {
      filters[lp].value = 1;
      continue;
    }
    else if (filters[lp].op == LEX_NEXIST)
    {
      filters[lp].value = 0;
      continue;
    }
    /*
     * Now the type specific bit
     */
    switch (attr_type)
    {
    case RADAT_INT:
      value = *((int *)data);
      switch (filters[lp].op)
      {
      case LEX_EQ:   result = (filters[lp].valb == (unsigned long)value); break;
      case LEX_NEQ:  result = (filters[lp].valb != (unsigned long)value); break;
      case LEX_LT:   result = (filters[lp].valb > (unsigned long)value);  break;
      case LEX_GT:   result = (filters[lp].valb < (unsigned long)value);  break;
      case LEX_LTE:  result = (filters[lp].valb >= (unsigned long)value); break;
      case LEX_GTE:  result = (filters[lp].valb <= (unsigned long)value); break;
      }
      filters[lp].value = result;
      break;
    case RADAT_IP:
    case RADAT_STR:
      result = regexec((regex_t *) filters[lp].valb, (char *) data, (size_t) 0, NULL, 0);
      filters[lp].value = ((result == 0 && filters[lp].op == LEX_EQ) ||
			   (result != 0 && filters[lp].op == LEX_NEQ));
      break;
     
    }
  }
  
}

void process(u_char *data1, struct pcap_pkthdr* h, u_char *p) {
  struct ip* ip_packet = (struct ip *)(p + link_offset);

  struct in_addr a;
  unsigned ip_hl = ip_packet->ip_hl*4;
  unsigned ip_off = ntohs(ip_packet->ip_off);
  unsigned fragmented = ip_off & (IP_MF | IP_OFFMASK);
  //  unsigned frag_offset = fragmented?(ip_off & IP_OFFMASK) * 8:0;
  static char tmpbuf[LBUFSIZE], longbuf[LBUFSIZE], hdrbuf[HDRBUFSIZE];
  struct attribs *tmpdata, *tmpdatav;
  struct udphdr* udp;
  unsigned udphdr_offset;
  int attrib, length,vendorattr;
  unsigned char *atdata, *data, ident, pkttype;
  unsigned int lp, len;
  char showpkt, replypkt = 0, reverse = 0;
  char srchost[24], dsthost[24];
  char *ptr, *timestamp;
  unsigned long vendorid;
  int dataoffset;
  if (ip_packet->ip_p != IPPROTO_UDP) 
    return;

	   
  reset_filters();
  *longbuf = 0; *hdrbuf = 0;

  udp = (struct udphdr *) (((char *)ip_packet) + ip_hl);
  udphdr_offset = (fragmented)?0:sizeof(struct udphdr); 
    
  data = ((char*)udp) + udphdr_offset;
  len = ntohs(ip_packet->ip_len) - ip_hl - udphdr_offset;

  pkttype = *data;
  ident   = *(data+1);

  switch (pkttype) {
  case RADPKT_AUTH_REQUEST:
    showpkt = show_auth & show_req;
    snprintf(tmpbuf, sizeof(tmpbuf), "Request ");
    break;
  case RADPKT_AUTH_ACCEPT:
    showpkt = show_auth & show_resp;
    reverse = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Accept  ");
    break;
  case RADPKT_AUTH_REJECT: 
    showpkt = show_auth & show_resp;
    reverse = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Reject  ");
    break;
  case RADPKT_ACCT_REQUEST: 
    showpkt = show_acct & show_req;
    snprintf(tmpbuf, sizeof(tmpbuf), "Acc-Req ");
    break;
  case RADPKT_ACCT_RESPONSE: 
    showpkt = show_acct & show_resp;
    reverse = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Acc-Ack ");
    break;
  case RADPKT_DISC_REQ:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Dis-Req ");
    break;
  case RADPKT_DISC_ACK:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Dis-ACK ");
    break;
  case RADPKT_DISC_NAK:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Dis-NAK ");
    break;
  case RADPKT_COA_REQ:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "CoA-Req ");
    break;
  case RADPKT_COA_ACK:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "CoA-ACK ");
    break;
  case RADPKT_COA_NAK:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "CoA-NAK ");
    break;
  case RADPKT_ACCT_CHALLENGE:
    // Not sure how to handle the rest so we'll just set showpkt to 1
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Ac-Chal ");
    break;
  case RADPKT_STATUS_SERVER:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Stat-Se ");
    break;
  case RADPKT_STATUS_CLIENT:
    showpkt = 1;
    snprintf(tmpbuf, sizeof(tmpbuf), "Stat-Cl ");
    break;
  default:
    showpkt = show_invalid;
    /* Since we have no idea what this packet is,  skip it */
    snprintf(tmpbuf, sizeof(tmpbuf),"Packet %3d (skip)", pkttype);
    replypkt = 1;
    break;
  }

  if (debug)
    fprintf(stderr, "Src: %s   Dst: %s\n", 
	   inet_ntoa(ip_packet->ip_src),
	   inet_ntoa(ip_packet->ip_dst));

  snprintf(srchost, sizeof(srchost), "%s:%d",
	  inet_ntoa(ip_packet->ip_src), ntohs(udp->uh_sport) );

  snprintf(dsthost, sizeof(dsthost), "%s:%d",
	  inet_ntoa(ip_packet->ip_dst), ntohs(udp->uh_dport) );

  if (print_time_fn)
    timestamp = print_time_fn(h);
  else 
    timestamp = "";

  if (reverse)
  {
    snprintf(hdrbuf,sizeof(hdrbuf), "%s(%2x) - %s <- %s (L%d) %s\n", 
	    tmpbuf, ident, dsthost, srchost, len, timestamp);
  }
  else {
    snprintf(hdrbuf,sizeof(hdrbuf), "%s(%2x) - %s -> %s (L%d) %s\n", 
	    tmpbuf, ident, srchost, dsthost, len, timestamp);
  }
  /* If replypkt is set this early - run away - bad packet */ 
  if (replypkt) goto LABEL;
  replypkt = watchlist_check(*data, ident,
			     (ip_packet->ip_dst).s_addr, udp->uh_dport);

  /* If we are in quiet mode, we only want to print the header
     so we can leave now */
  if (t_radfilters == 0 && quiet)
    goto LABEL;

  atdata = data + 20;
  while (atdata < data + len)
  {
    vendorid = 0;
    dataoffset = 0;
    //printf("atdata %d data+len %d\n", atdata, data+len);
    attrib = (unsigned char) *atdata;
    length = (unsigned char) *(atdata+1);

    /*
     * Ok - some of the length attributes are broken on old versions
     * of radius - lets just ignore the rest of the packet 
     */
    if (length == 0) {
      if (debug) printf("Ignoring malformed extra data\n");
      goto LABEL;
    }
    tmpdata = find_radattr_by_value(attrib);
    if (tmpdata)
    {
       /* Hack(tm) for VSA's */
      if (attrib == 26) {
          vendorid = ntohl(*(uint32_t *)(atdata+2));
	  vendorattr = (unsigned char)*(atdata+6);
	  dataoffset = 6;
      }
      
      // Lets now have a go at parsing the value...
      switch (tmpdata->valb)
      {
      case RADAT_INT:
	{ 
	  uint32_t value;

	  memcpy(&a.s_addr, atdata+2, 4);
	  value = ntohl(a.s_addr);
	  
	  if (!replypkt)
	    match_attr(attrib, tmpdata->valb, (void *) &value);

	  // Ok - now we need to check whether this is an enum type
	  tmpdatav = find_radvals_by_value(attrib, value);
	  if (tmpdatav) {
	    snprintf(tmpbuf, sizeof(tmpbuf), "  %-23s\tLen %2d \t%s\n", 
		    tmpdata->name, length, tmpdatav->name);
	    strlcat(longbuf, tmpbuf,sizeof(longbuf));
	  }
	  else {
	    snprintf(tmpbuf, sizeof(tmpbuf), "  %-23s\tLen %2d \t%u\n", 
		    tmpdata->name, length, value);
	    strlcat(longbuf, tmpbuf, sizeof(longbuf));
	  }
	}
	break;
      case RADAT_IP:
	memcpy(&a.s_addr, atdata+2, 4);

	if (!replypkt) 
	  match_attr(attrib, tmpdata->valb, (void *) inet_ntoa(a));

	snprintf(tmpbuf, sizeof(tmpbuf), "  %-23s\tLen %2d \t%s\n", 
		tmpdata->name, length, inet_ntoa(a));
	strlcat(longbuf, tmpbuf, sizeof(longbuf));
	break;
      case RADAT_STR:
	{
	  if (attrib != 26)
	    snprintf(tmpbuf, sizeof(tmpbuf), "  %-23s\tLen %2d \t\"", tmpdata->name, length);
	  else 
	    snprintf(tmpbuf, sizeof(tmpbuf), "  %s(%3lu,%3d)\tLen %2d \t\"",tmpdata->name,vendorid,vendorattr, length-dataoffset);

	  strlcat(longbuf, tmpbuf, sizeof(longbuf));

	  for (ptr=tmpbuf,lp = 2; lp < length-dataoffset; lp++)
	  {
	    if ((*(atdata+lp+dataoffset) > 31) && (*(atdata+lp+dataoffset) < 128))
	      *ptr++ = *(atdata+lp+dataoffset);
	    else {
	      sprintf(ptr,"\\%03o",*(atdata+lp+dataoffset));
	      ptr += 4;
	    }
	  }
	  *ptr = '\0';

	  // Check for filter matches
	  if (!replypkt)
	    match_attr(attrib, tmpdata->valb, (void *) tmpbuf);

	  /* Need to add this onto the end to make it useable */
          strlcat(tmpbuf,"\"\n", sizeof(tmpbuf));
	  strlcat(longbuf, tmpbuf, sizeof(longbuf));
	}
      }
    }
    else {
      snprintf(tmpbuf, sizeof(tmpbuf), "  Attrib-%3u\t\t\tLen %2d\t\"", 
	      (char) attrib & 0xff, (char)length & 0xff);
      strlcat(longbuf, tmpbuf, sizeof(longbuf));
      for (ptr=tmpbuf,lp = 2; lp < length; lp++)
      {
	      if ((*(atdata+lp) > 31) && (*(atdata+lp) < 128))
		      *ptr++ = *(atdata+lp);
	      else {
		      sprintf(ptr,"\\%03o",*(atdata+lp));
		      ptr += 4;
	      } 
      } 
      *ptr = '\0';
      strlcat(tmpbuf,"\"\n", sizeof(tmpbuf));
      strlcat(longbuf, tmpbuf, sizeof(longbuf));
    }
    atdata += length;
  }
 LABEL:
  /*
   * We show the packet if there are no filters in action
   * or if all the filters have matched
   */
  //showpkt = (t_radfilters == 0) || (t_radfilters - filter_matches() == 0); 
  //showpkt &= (t_radfilters == 0) || (parse_packet(tree.root) == 1);
  //showpkt = parse_packet(tree.root);
  //  printf("Rules1 left %d replypkt %d\n", showpkt, replypkt);
  if (t_radfilters)
  {
    switch (*data)
    {
    case 1:
    case 4:
      if (parse_packet(tree.root) == 1) 
      {
	/*
	 * There are filters in action and we have matched all of them
	 * If this is a request packet we need to add the ip address 
	 * and port number to the watch list so that we can pick up the 
	 * reply packet
	 */
	watchlist_add(ident, (ip_packet->ip_src).s_addr, udp->uh_sport);
	showpkt &= 1;
      }
      else showpkt = 0;
      break;
    default:
      // We only want to see it if its a reply packet
      showpkt &= replypkt;
      break;
    }
  }
  if (debug & 1)
    fprintf(stderr, "Packet: total filters %d - showpkt %d\n",
	    t_radfilters, showpkt);
  //  printf("Rules2 left %d replypkt %d\n", showpkt, replypkt);
  if (showpkt)
  {
    printf("%s", hdrbuf);
    if (!quiet) printf("%s", longbuf);
    fflush(stdout);		/* needed for redirecting to file */
  }
}

