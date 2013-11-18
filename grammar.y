/* Reverse polish notation calculator. */
     
%{
//#define YYSTYPE char*
//#define YYSTYPE double
#include <math.h>
#include <ctype.h>
#include <stdio.h>

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
     
%token LEX_ATTR
%token LEX_NUM
%token LEX_NULL
%token LEX_STR
%token LEX_IP
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
	      if (debug) fprintf(stderr,"Found test %d\n", $1); 
	      $$ = $1;
	    }
        | exp lop test   
            { 
	      if (debug) fprintf(stderr,"Found test lop test\n");  
	      $$ = yacc_add_node((int) $1, (int) $2, (int)$3);
	    }

        ;

test:   test1     { $$ = $1; }
      | uop test1 { $$ = yacc_add_node((int) $2, (int) $1, -1); }


test1:  LEX_LPAR exp1 LEX_RPAR     
          { $$ = $2; if (debug) fprintf(stderr,"Found (exp)\n");          }
      | attribute bop value 
          { $$ = yacc_add_node(yacc_parse_test($1, $2, $3, tmpbuf),
			       LEX_TERM, -1);
          }
      | exist_op attribute
          { $$ = yacc_add_node(yacc_parse_test($2, $1, 0, ""),
			       LEX_TERM, -1);
	  }

	  ;
	       
exp1:     test            
            { 
              $$ = $1; 
              if (debug) fprintf(stderr,"Found test (exp)\n");  
	    }
        | exp lop test   
            { 
	      if (debug) fprintf(stderr,"Found test lop test\n");  
	      $$ = yacc_add_node((int) $1, (int) $2, (int)$3);
	    }

	| LEX_NULL
        ;

exist_op: LEX_EXIST     { $$ = LEX_EXIST;  }
        | LEX_NEXIST    { $$ = LEX_NEXIST; }
        ;


/* Unary Operators */
uop:      LEX_NOT { $$ = LEX_NOT; }
        ;


/* Logical Operators */
lop:      LEX_AND { $$ = LEX_AND; }
        | LEX_OR  { $$ = LEX_OR;  }
        ;

/* Binary operators */
bop:      LEX_EQ      { $$ = LEX_EQ;   }
        | LEX_NEQ     { $$ = LEX_NEQ;  }
        | LEX_GT      { $$ = LEX_GT;   }
        | LEX_LT      { $$ = LEX_LT;   }
        | LEX_GTE     { $$ = LEX_GTE;  }
        | LEX_LTE     { $$ = LEX_LTE;  }
        ;

attribute:  LEX_STR  { $$ = (int) yacc_find_attr((char *) yylval); }

value:    LEX_NUM   
            { 
              $$ = LEX_NUM;
	      strcpy((char *) tmpbuf, (char *) yylval); 
            }
        | LEX_STR    
           {
              $$ = LEX_STR;
              strcpy((char *) tmpbuf, (char *) yylval); 
           }
        ;
%%


/* Lexical analyzer returns a double floating point
number on the stack and the token NUM, or the ASCII
read if not a number.  Skips all blanks
and tabs, returns 0 for EOF. */
     
yyerror (char *s)  /* Called by yyparse on error */
{
  fprintf (stderr, "Error in Radius Filter\n");
  clean_exit(-2);
}

     
