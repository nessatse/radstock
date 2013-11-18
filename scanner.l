

%{
#define YYSTYPE int
#include "y.tab.h"
extern char yacc_input_buff[1000];
extern char *yacc_input_pos;
extern int debug;
extern int yylval;

#ifndef lint
static const char rcsid[] =
    "$Id: scanner.l,v 1.5 2000/10/26 15:04:16 withnail Exp $";
#endif


#ifdef FLEX_SCANNER
#define YY_NO_UNPUT
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
if (*yacc_input_pos > 3) \
{ \
  int c = '*', n; \
  c = *(yacc_input_pos++);   \
  for ( n = 0; n < max_size &&  (c  != '\0' && c != EOF); n++ ) { \
    buf[n] = (char) c; \
    c = *(yacc_input_pos++);  \
  } \
  result = n; \
} \
else {\
  result = YY_NULL; \
}
#else
#undef getc
#define getc(fp) (*yacc_input_pos == 0 ? EOF : *yacc_input_pos++)
#endif

%}

N		([0-9]+|(0X|0x)[0-9A-Fa-f]+)
B		([0-9A-Fa-f][0-9A-Fa-f]?)
W		([0-9A-Fa-f][0-9A-Fa-f]?[0-9A-Fa-f]?[0-9A-Fa-f]?)
S               ('[^']*'|[^\t\n ]*)
%%

"exists"        return LEX_EXIST;
"nexists"       return LEX_NEXIST;

and|"&&"	{ if (debug) fprintf(stderr,"lex: AND\n");
                  return LEX_AND;  }
or|"||"		{ if (debug) fprintf(stderr, "\nlex: OR\n");  
                  return LEX_OR;   }
not|"!"		{ if (debug) fprintf(stderr, "\nlex: NOT\n");
                  return LEX_NOT;  }


"\0"            { return LEX_NULL; }
"("             { printf("\nlex: LPAR\n"); return LEX_LPAR; }
")"             { printf("\nlex: RPAR\n"); return LEX_RPAR; }

"="             {    return LEX_EQ;   }
">="            {    return LEX_GTE;  }
">"             {    return LEX_GT;   }
"<="            {    return LEX_LTE;  }
"<"             {    return LEX_LT;   }
"!="            {    return LEX_NEQ;  }

#{N}             { printf("\nlex: NUM %s\n", yytext);  return LEX_NUM;  }
{N}             {  yylval = yytext;  return LEX_NUM;  }
#{S}             { printf("\nlex: STR %s\n", yytext);  yylval = yytext; return LEX_STR;  }
{S}             { yylval = yytext; return LEX_STR;  }

%%

int
yywrap()
{
        return 1;
}
