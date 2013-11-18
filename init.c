/*
 * $Id: init.c,v 1.1.1.1 2000/10/13 12:23:11 withnail Exp $
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
#include "y.tab.h"
static char rcsid[] = "$Id: init.c,v 1.1.1.1 2000/10/13 12:23:11 withnail Exp $";

int t_radvals = 0;
int t_radattrs = 0;
int curnode = 0;

#define ATTR_NOTFOUND 0
#define ATTR_MATCH    1
#define ATTR_NOMATCH  2

#ifdef YYDEBUG
extern int yydebug;
#endif
char yacc_input_buff[1000];
char *yacc_input_pos;
int yyparse(void);



struct attr_listelem
{
  int attr_index;
  int attr_op;
  union {
    int ivalue;
    regex_t *rvalue;
  } data;
  int result;			/* ATTR_NOTFOUND etc */

};

struct attr_list
{
  struct attr_listelem elem[50];
  int t_elem;
};

struct s_tree tree;

int my_find_radattr(char *attrname, int *radindex)
{
  int lp;

  for (lp = 0; lp < t_radattrs; lp++)
  {
    if (strcasecmp(attrname, radattrs[lp].name) == 0) {
      *radindex = lp;
      return radattrs[lp].vala;
    }
  }
  return 0;
}

int my_find_radval(int radattr, char *attrname)
{
  int lp, tmpval;

  for (lp = 0; lp < t_radvals; lp++)
  {
    if (strcasecmp(attrname, radvals[lp].name) == 0) {
      tmpval = radvals[lp].vala >> 8;
      if (tmpval == radattr)
	
	return radvals[lp].vala - (tmpval << 8);
    }
  }
  return 0;
}


int my_find_attrs(const void *a, const void *b)
{
  struct attribs b1;
  unsigned int a1;

  a1 = (unsigned int) *((int *)a);

  b1 = *((struct attribs *) b);
  //  printf("Testing value %d against attribute table entry %d\n",
  //	 a1, b1.vala);
  
  return (a1 < b1.vala ? -1 : a1 > b1.vala ? 1 : 0);
}

struct attribs *find_radattr_by_value(int lookupval)
{
  return bsearch(&lookupval, 
		 radattrs, t_radattrs, 
		 sizeof(struct attribs), my_find_attrs);
}

struct attribs *find_radvals_by_value(int attrib, int lookupval)
{
  int tmp = (attrib << 8) +lookupval;
  return bsearch(&tmp, 
		 radvals, t_radvals, 
		 sizeof(struct attribs), my_find_attrs);
}


int cmp_attrs(const void *a, const void *b)
{
  struct attribs a1;
  struct attribs b1;

  b1 = *((struct attribs *) b);
  a1 = *((struct attribs *) a);

  return (a1.vala < b1.vala ? -1 : a1.vala > b1.vala ? 1 : 0);
}



int readdict(char *dictfile)
{
  char buffer[BUFSIZE+5];
  char arg1[BUFSIZE], arg2[BUFSIZE], arg3[BUFSIZE], arg4[BUFSIZE];
  int temp2 = 0;
  int lp;

  FILE *fd;

  if ((fd = fopen(dictfile, "r")) == NULL)
  {
    return 0;
  }
  t_radattrs = 0;
  t_radvals = 0;

  // Search for attributes
  while (feof(fd) == 0)
  {
    fgets(buffer, BUFSIZE, fd);
    if (!buffer || *buffer == '#' || *buffer == ' ' || *buffer == '\n' ) continue;
    if (debug & 2) fprintf(stderr, "readdict: buffer = :%s:\n", buffer);
    sscanf(buffer,"%s %s %s %s\n", arg1, arg2, arg3, arg4);

    // Ok - lets check for type of attrib
    if (strcasecmp(arg1, "ATTRIBUTE") == 0)
    {
      // Lets skip this attribute if we have read in our maximum
      if (t_radattrs == (ATTRSIZE - 2)) continue;
      radattrs[t_radattrs].vala = atoi(arg3);
      strcpy(radattrs[t_radattrs].name, arg2);
      if (strcasecmp(arg4, "integer") == 0) temp2 = RADAT_INT;
      else if (strcasecmp(arg4, "ipaddr") == 0) temp2 = RADAT_IP;
      else if (strcasecmp(arg4, "string") == 0) temp2 = RADAT_STR;
      if (debug & 2) 
	fprintf(stderr, "readdict: new attrib (%lu) = :%s:\n",
		radattrs[t_radattrs].vala,
		radattrs[t_radattrs].name);
      radattrs[t_radattrs++].valb = temp2;
    }
  }

  qsort((void *) radattrs, t_radattrs, sizeof(struct attribs), cmp_attrs);
  rewind(fd);

  while (feof(fd) == 0)
  {
    fgets(buffer, BUFSIZE, fd);
    if (!buffer || *buffer == '#' || *buffer == ' ' || *buffer == '\n' ) continue;
    //    printf("Buffer = :%s:\n", buffer);
    sscanf(buffer,"%s %s %s %s\n", arg1, arg2, arg3, arg4);


    // Ok - lets check for type of attrib
    if (strcasecmp(arg1, "VALUE") == 0)
    {

      // Lets skip this attribute if we have read in our maximum
      if (t_radvals == (ATTRSIZE - 2)) continue;

      // Search thru the whole list since it isnt sorted on name
      for (lp = 0; lp < t_radattrs; lp++)
	if (strcasecmp(radattrs[lp].name, arg2) == 0)
	{
	  radvals[t_radvals].vala = (radattrs[lp].vala << 8) + atoi(arg4);
	  strcpy(radvals[t_radvals].name, arg3);
	  t_radvals++;
	  break;
	}
    }
  }
  qsort((void *) radvals, t_radvals, sizeof(struct attribs), cmp_attrs);
  fclose(fd);
  return 1;
}


void get_radfilter(char *arg) 
{
  strcpy(yacc_input_buff, arg);
  yacc_input_pos = yacc_input_buff;

  if (debug) printf("get_radfilter: Parsing radius filter\n");
#ifdef YYDEBUG
  yydebug = 1;
#endif
  yyparse();
  tree.root = curnode - 1;
  if (debug) printf("get_radfilter: Passed radius filter\n");

}


#define FIND_ATTR 1
#define FIND_EQ   2
#define FIND_VAL  3

#if 0

void get_radfilter(char *arg) 
{
  char *achr, *bchr;
  char tmpstr[40], tmpstr2[50], tmpptr;
  int match;
  int radattr = 0, radval, attrindex;

  if (debug) printf("Parsing radius filter\n");
  //  if (!*arg)
  //  return NULL;

  t_radfilters = 0;
  match = FIND_ATTR;

  achr = arg;
    
    while (*achr) {
      if (debug) printf("here we go again\n");
      bchr = tmpstr;
      /* Remove leading space */
      while (isspace(*achr)) achr++;
      while ((*achr) && (*achr != '=') && !isspace(*achr) )
      {
	//printf("achr %c \n", *achr);
	*bchr++ = *achr++;
      }
      /* Remove trailing space */
      while (isspace(*achr)) achr++;
      *bchr = '\0';
      if (debug) printf("found string '%s' (achr '%s')\n", 
			tmpstr, achr);

      switch (match) {
      case FIND_ATTR:
	if ((radattr = my_find_radattr(tmpstr, &attrindex)) == 0) {
	  if (debug) printf("Attribute '%s' not known - exiting\n", tmpstr);
	  clean_exit(-1);
	}
	if (debug) printf("Found attrib (attrnum %d - index %d) %s\n", 
			  radattr, attrindex, tmpstr);
	match = FIND_EQ;
	break;
      case FIND_EQ:
	if (*achr == '=')
	{
	  if (debug) printf ("Found EQ\n");
	  achr++;
	  match = FIND_VAL;
	}
	else {
	  if (debug) printf("Equals not found\n");
	  exit(0);
	}
	break;
      case FIND_VAL:
	// Ok - now we need to know what type of attribute
	// we are looking at
	switch (radattrs[attrindex].valb) {
	case RADAT_INT:
	  if ((radval = my_find_radval(radattr, tmpstr)) == 0)
	    radval = atoi(tmpstr);
	  if (debug) printf("Found enum value (valnum %d) %s\n", radval, tmpstr);
	  filters[t_radfilters].vala = radattr;
	  filters[t_radfilters].valb = radval;
	  t_radfilters++;
	  break;

	case RADAT_IP:
	case RADAT_STR:
	  if (debug && radattrs[attrindex].valb == RADAT_STR) 
	    fprintf(stderr, "Found str value %s\n", tmpstr);
	  if (debug && radattrs[attrindex].valb == RADAT_IP) 
	    fprintf(stderr, "Found IP value %s\n", tmpstr);
	  
	  *tmpstr2 = '\0';
	  tmpptr = tmpstr;
	  if (*tmpptr == '\'') tmpptr++;
	  fprintf(stderr, "trailing '%c'\n", tmpstr[strlen(tmpstr)-1]);
	  
	  if (tmpstr[strlen(tmpstr)-1] == '\'')
	  {
	    fprintf(stderr, "trailing '\n");
	  }
	  if (*tmpptr != '^') strcat(tmpstr2, "^");
	  strcat(tmpstr2, tmpptr);
	  
	  if (*(tmpstr+strlen(tmpstr)-1) != '$') strcat(tmpstr2, "$");
	  if (debug) fprintf(stderr, "Adding regexp '%s'\n", tmpstr2);

	  filters[t_radfilters].vala = radattr;
	  filters[t_radfilters].valb = (int) malloc(sizeof(regex_t));
	  if (filters[t_radfilters].valb == (int) NULL) clean_exit(-2);
	  regcomp((regex_t *) filters[t_radfilters].valb,
		  tmpstr2, REG_EXTENDED|REG_NOSUB|REG_ICASE);
	  //strcpy(filters[t_radfilters].name, tmpstr);
	  t_radfilters++;
	  break;
	}
	match = FIND_ATTR;
	
	break;
      }
    }
  
}

#endif


int yacc_find_attr(char *str)
{
  int index, res;
  res = my_find_radattr(str,&index);
  if (res == 0)
  {
    fprintf(stderr," Attribute '%s' not found\n", str);
    clean_exit(-2);
  }
  //if (debug) fprintf(stderr, "Found attribute (%d) '%s'\n", 
  //res, str);
  return res;
}


int yacc_parse_test(int a1, int a2, int a3, char *value)
{
  struct attribs *myattr;
  int retval;

  myattr = find_radattr_by_value(a1);
  if (debug) 
    fprintf(stderr, "*** Test: attr (index %d) %s  op %d  value (type %d) '%s'\n",
	    a1, myattr->name, a2, a3, value);
  
  /*
   * What we do now is purely decided on the type of attribute
   */
  if (a2 == LEX_EXIST || a2 == LEX_NEXIST)
  {
    filters[t_radfilters].vala = a1;
    filters[t_radfilters].op   = a2;
    filters[t_radfilters].valb = (unsigned long)NULL;
    retval = t_radfilters++;
  }
  else 
  switch (myattr->valb)
  {
  case RADAT_INT:
    {
      int radval;

      if (a3 == LEX_NUM) 
      {
	radval = atoi(value);
      }
      else if ((radval = my_find_radval(a1, value)) == 0)
      {
	fprintf(stderr, "Error: Attribute %s - no value defined for %s\n",
		myattr->name, value);
	clean_exit(-2);
      }
      if (debug) fprintf(stderr, "Found integer %d\n", radval);
      filters[t_radfilters].vala = a1;
      filters[t_radfilters].op   = a2;
      filters[t_radfilters].valb = radval;
      retval = t_radfilters++;
    }
    break;
  case RADAT_IP:
  case RADAT_STR:
    {
      char tmpstr[50], *tmpptr;
      if (a2 != LEX_EQ && a2 != LEX_NEQ) 
      {
	fprintf(stderr, "Only = and != are valid for STR and IP types\n");
	clean_exit(-2);
      }
      *tmpstr = '\0';
      if (*value != '^') strcat(tmpstr, "^");

      tmpptr = value;
      if (*tmpptr == '\'') tmpptr++;
	  
      if (value[strlen(value)-1] == '\'')
	value[strlen(value)-1] = '\0';

      strcat(tmpstr, tmpptr);
      if (*(value+strlen(value)-1) != '$') strcat(tmpstr, "$");
      if (debug) fprintf(stderr, "Adding regexp '%s'\n", tmpstr);

      filters[t_radfilters].vala = a1;
      filters[t_radfilters].op   = a2;
      filters[t_radfilters].valbp = malloc(sizeof(regex_t));
      if (filters[t_radfilters].valbp ==  NULL) clean_exit(-2);
      regcomp((regex_t *) filters[t_radfilters].valbp,
	      tmpstr, REG_EXTENDED|REG_NOSUB|REG_ICASE);
      //strcpy(filters[t_radfilters].name, tmpstr);
      retval = t_radfilters++;
    }
    break;
  }
  return retval;
}


int yacc_add_node(int a1, int a2, int a3)
{
  int retval;

  retval = curnode++;
  if (debug) 
    fprintf(stderr, "Node %d: left %d  op %d  right %d\n",
	    retval, a1, a2, a3);
  tree.elem[retval].operator  = a2;
  tree.elem[retval].lside     = a1;
  tree.elem[retval].rside     = a3;

  return retval;
}
