/* Reverse polish notation calculator. */
     
%{
#define YYDEBUG 0
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
//#include "radstock.h"
extern void clean_exit(int);
extern int yylex(void);

#ifndef lint
static const char rcsid[] =
    "$Id: grammar.y,v 1.7 2000/10/26 15:04:16 withnail Exp $";
#endif

extern int debug;
char tmpbuf[100];
extern int yacc_find_attr(char *);
extern int yacc_parse_test(int, int, int, char*);
extern int yacc_add_node(int,int,int); 
%}
     
%union {
    char *string;
    int  number;
}
%token  <string> LEX_ATTR
%token  <string> LEX_NUM
%token  LEX_NULL
%token  <string> LEX_STR
%token  <string> LEX_IP
%token LEX_AND LEX_OR 
%token LEX_EQ LEX_NEQ 
%token LEX_NOT LEX_GT LEX_GTE LEX_LT LEX_LTE
%token LEX_LPAR LEX_RPAR
%token LEX_EXIST LEX_NEXIST
%token LEX_TERM     
%% /* Grammar rules and actions follow */
input:    line
     ;
     
line:       LEX_NULL {  if (debug)  fprintf(stderr,"syntax ok1\n"); }
          | exp   { if (debug) fprintf (stderr, "syntax ok2\n"); }
          ;
     
exp:      test        
            { 
	      if (debug) fprintf(stderr,"Found test %d\n", $<string>1); 
	      $<string>$ = $<string>1;
	    }
        | exp lop test   
            { 
	      if (debug) fprintf(stderr,"Found test lop test\n");  
	      $<number>$ = yacc_add_node((int) $<number>1, (int) $<number>2, (int)$<number>3);
	    }

        ;

test:   test1     { $<string>$ = $<string>1; }
      | uop test1 { $<number>$ = yacc_add_node((int) $<number>2, (int) $<number>1, -1); }


test1:  LEX_LPAR exp1 LEX_RPAR     
          { $<string>$ = $<string>2; if (debug) fprintf(stderr,"Found (exp)\n");          }
      | attribute bop value 
          { $<number>$ = yacc_add_node(yacc_parse_test($<number>1, $<number>2, $<number>3, tmpbuf),
			       LEX_TERM, -1);
          }
      | exist_op attribute
          { $<number>$ = yacc_add_node(yacc_parse_test($<number>2, $<number>1, 0, ""),
			       LEX_TERM, -1);
	  }

	  ;
	       
exp1:     test            
            { 
              $<string>$ = $<string>1; 
              if (debug) fprintf(stderr,"Found test (exp)\n");  
	    }
        | exp lop test   
            { 
	      if (debug) fprintf(stderr,"Found test lop test\n");  
	      $<number>$ = yacc_add_node((int) $<number>1, (int) $<number>2, (int)$<number>3);
	    }

	| LEX_NULL
        ;

exist_op: LEX_EXIST     { $<number>$ = LEX_EXIST;  }
        | LEX_NEXIST    { $<number>$ = LEX_NEXIST; }
        ;


/* Unary Operators */
uop:      LEX_NOT { $<number>$ = LEX_NOT; }
        ;


/* Logical Operators */
lop:      LEX_AND { $<number>$ = LEX_AND; }
        | LEX_OR  { $<number>$ = LEX_OR;  }
        ;

/* Binary operators */
bop:      LEX_EQ      { $<number>$ = LEX_EQ;   }
        | LEX_NEQ     { $<number>$ = LEX_NEQ;  }
        | LEX_GT      { $<number>$ = LEX_GT;   }
        | LEX_LT      { $<number>$ = LEX_LT;   }
        | LEX_GTE     { $<number>$ = LEX_GTE;  }
        | LEX_LTE     { $<number>$ = LEX_LTE;  }
        ;

attribute:  LEX_STR  { $<number>$ = (int) yacc_find_attr((char *)$<string>1); }

value:    LEX_NUM   
            { 
              $<number>$ = LEX_NUM;
	      strcpy((char *) tmpbuf, (char *) $<string>1); 
            }
        | LEX_STR    
           {
              $<number>$ = LEX_STR;
              strcpy((char *) tmpbuf, (char *) $<string>1); 
           }
        ;
%%


/* Lexical analyzer returns a double floating point
number on the stack and the token NUM, or the ASCII
read if not a number.  Skips all blanks
and tabs, returns 0 for EOF. */
     
void yyerror (char *s)  /* Called by yyparse on error */
{
  fprintf (stderr, "Error in Radius Filter\n");
  clean_exit(-2);
}

     
