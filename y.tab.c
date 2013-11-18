#include <stdlib.h>
#ifndef lint
#ifdef __unused
__unused
#endif
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.37 2003/02/12 18:03:55 davidc Exp $";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
#if defined(__cplusplus) || __STDC__
static int yygrowstack(void);
#else
static int yygrowstack();
#endif
#define YYPREFIX "yy"
#line 4 "grammar.y"
/*#define YYSTYPE char**/
/*#define YYSTYPE double*/
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
#line 41 "y.tab.c"
#define YYERRCODE 256
#define LEX_ATTR 257
#define LEX_NUM 258
#define LEX_NULL 259
#define LEX_STR 260
#define LEX_IP 261
#define LEX_AND 262
#define LEX_OR 263
#define LEX_EQ 264
#define LEX_NEQ 265
#define LEX_NOT 266
#define LEX_GT 267
#define LEX_GTE 268
#define LEX_LT 269
#define LEX_LTE 270
#define LEX_LPAR 271
#define LEX_RPAR 272
#define LEX_EXIST 273
#define LEX_NEXIST 274
#define LEX_TERM 275
const short yylhs[] = {                                        -1,
    0,    1,    1,    2,    2,    3,    3,    5,    5,    5,
    7,    7,    7,   11,   11,    6,    4,    4,    9,    9,
    9,    9,    9,    9,    8,   10,   10,
};
const short yylen[] = {                                         2,
    1,    1,    1,    1,    3,    1,    2,    3,    3,    2,
    1,    3,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,
};
const short yydefred[] = {                                      0,
    2,   25,   16,    0,   14,   15,    0,    1,    0,    4,
    6,    0,    0,    0,   13,    0,    0,    0,   17,   18,
    0,    7,   19,   20,   21,   23,   22,   24,    0,   10,
    0,    8,    5,   26,   27,    9,    0,
};
const short yydgoto[] = {                                       7,
    8,    9,   10,   21,   11,   12,   18,   13,   29,   36,
   14,
};
const short yysindex[] = {                                   -258,
    0,    0,    0, -248,    0,    0,    0,    0, -256,    0,
    0, -231, -220, -246,    0, -256,    0, -268,    0,    0,
 -238,    0,    0,    0,    0,    0,    0,    0, -255,    0,
 -238,    0,    0,    0,    0,    0,    0,
};
const short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,   24,    0,
    0,    0,    0,    0,    0,    0, -253,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -242,
};
const short yygindex[] = {                                      0,
    0,   27,   -4,   16,   22,    0,    0,   23,    0,    0,
    0,
};
#define YYTABLESIZE 50
const short yytable[] = {                                      17,
    1,    2,   34,   32,   35,   19,   20,    3,    4,    4,
   15,    2,    4,    2,    5,    6,   33,    3,   11,    5,
    5,    2,    4,    3,    5,    6,   37,    3,    2,   12,
   16,   31,    4,   22,    5,    6,   30,    0,    0,    4,
    0,    5,    6,   23,   24,    0,   25,   26,   27,   28,
};
const short yycheck[] = {                                       4,
  259,  260,  258,  272,  260,  262,  263,  266,  262,  263,
  259,  260,  271,  260,  273,  274,   21,  266,  272,  262,
  263,  260,  271,    0,  273,  274,   31,  266,  260,  272,
    4,   16,  271,   12,  273,  274,   14,   -1,   -1,  271,
   -1,  273,  274,  264,  265,   -1,  267,  268,  269,  270,
};
#define YYFINAL 7
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 275
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"LEX_ATTR","LEX_NUM","LEX_NULL",
"LEX_STR","LEX_IP","LEX_AND","LEX_OR","LEX_EQ","LEX_NEQ","LEX_NOT","LEX_GT",
"LEX_GTE","LEX_LT","LEX_LTE","LEX_LPAR","LEX_RPAR","LEX_EXIST","LEX_NEXIST",
"LEX_TERM",
};
const char * const yyrule[] = {
"$accept : input",
"input : line",
"line : LEX_NULL",
"line : exp",
"exp : test",
"exp : exp lop test",
"test : test1",
"test : uop test1",
"test1 : LEX_LPAR exp1 LEX_RPAR",
"test1 : attribute bop value",
"test1 : exist_op attribute",
"exp1 : test",
"exp1 : exp lop test",
"exp1 : LEX_NULL",
"exist_op : LEX_EXIST",
"exist_op : LEX_NEXIST",
"uop : LEX_NOT",
"lop : LEX_AND",
"lop : LEX_OR",
"bop : LEX_EQ",
"bop : LEX_NEQ",
"bop : LEX_GT",
"bop : LEX_LT",
"bop : LEX_GTE",
"bop : LEX_LTE",
"attribute : LEX_STR",
"value : LEX_NUM",
"value : LEX_STR",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
#line 123 "grammar.y"


/* Lexical analyzer returns a double floating point
number on the stack and the token NUM, or the ASCII
read if not a number.  Skips all blanks
and tabs, returns 0 for EOF. */
     
yyerror (char *s)  /* Called by yyparse on error */
{
  fprintf (stderr, "Error in Radius Filter\n");
  clean_exit(-2);
}

     
#line 207 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 2:
#line 37 "grammar.y"
{  if (debug)  fprintf(stderr,"syntax ok1\n"); }
break;
case 3:
#line 38 "grammar.y"
{ if (debug) fprintf (stderr, "syntax ok2\n"); }
break;
case 4:
#line 42 "grammar.y"
{ 
	      if (debug) fprintf(stderr,"Found test %d\n", yyvsp[0]); 
	      yyval = yyvsp[0];
	    }
break;
case 5:
#line 47 "grammar.y"
{ 
	      if (debug) fprintf(stderr,"Found test lop test\n");  
	      yyval = yacc_add_node((int) yyvsp[-2], (int) yyvsp[-1], (int)yyvsp[0]);
	    }
break;
case 6:
#line 54 "grammar.y"
{ yyval = yyvsp[0]; }
break;
case 7:
#line 55 "grammar.y"
{ yyval = yacc_add_node((int) yyvsp[0], (int) yyvsp[-1], -1); }
break;
case 8:
#line 59 "grammar.y"
{ yyval = yyvsp[-1]; if (debug) fprintf(stderr,"Found (exp)\n");          }
break;
case 9:
#line 61 "grammar.y"
{ yyval = yacc_add_node(yacc_parse_test(yyvsp[-2], yyvsp[-1], yyvsp[0], tmpbuf),
			       LEX_TERM, -1);
          }
break;
case 10:
#line 65 "grammar.y"
{ yyval = yacc_add_node(yacc_parse_test(yyvsp[0], yyvsp[-1], 0, ""),
			       LEX_TERM, -1);
	  }
break;
case 11:
#line 72 "grammar.y"
{ 
              yyval = yyvsp[0]; 
              if (debug) fprintf(stderr,"Found test (exp)\n");  
	    }
break;
case 12:
#line 77 "grammar.y"
{ 
	      if (debug) fprintf(stderr,"Found test lop test\n");  
	      yyval = yacc_add_node((int) yyvsp[-2], (int) yyvsp[-1], (int)yyvsp[0]);
	    }
break;
case 14:
#line 85 "grammar.y"
{ yyval = LEX_EXIST;  }
break;
case 15:
#line 86 "grammar.y"
{ yyval = LEX_NEXIST; }
break;
case 16:
#line 91 "grammar.y"
{ yyval = LEX_NOT; }
break;
case 17:
#line 96 "grammar.y"
{ yyval = LEX_AND; }
break;
case 18:
#line 97 "grammar.y"
{ yyval = LEX_OR;  }
break;
case 19:
#line 101 "grammar.y"
{ yyval = LEX_EQ;   }
break;
case 20:
#line 102 "grammar.y"
{ yyval = LEX_NEQ;  }
break;
case 21:
#line 103 "grammar.y"
{ yyval = LEX_GT;   }
break;
case 22:
#line 104 "grammar.y"
{ yyval = LEX_LT;   }
break;
case 23:
#line 105 "grammar.y"
{ yyval = LEX_GTE;  }
break;
case 24:
#line 106 "grammar.y"
{ yyval = LEX_LTE;  }
break;
case 25:
#line 109 "grammar.y"
{ yyval = (int) yacc_find_attr((char *) yylval); }
break;
case 26:
#line 112 "grammar.y"
{ 
              yyval = LEX_NUM;
	      strcpy((char *) tmpbuf, (char *) yylval); 
            }
break;
case 27:
#line 117 "grammar.y"
{
              yyval = LEX_STR;
              strcpy((char *) tmpbuf, (char *) yylval); 
           }
break;
#line 524 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
