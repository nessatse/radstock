/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LEX_ATTR = 258,
     LEX_NUM = 259,
     LEX_NULL = 260,
     LEX_STR = 261,
     LEX_IP = 262,
     LEX_AND = 263,
     LEX_OR = 264,
     LEX_EQ = 265,
     LEX_NEQ = 266,
     LEX_NOT = 267,
     LEX_GT = 268,
     LEX_GTE = 269,
     LEX_LT = 270,
     LEX_LTE = 271,
     LEX_LPAR = 272,
     LEX_RPAR = 273,
     LEX_EXIST = 274,
     LEX_NEXIST = 275,
     LEX_TERM = 276
   };
#endif
/* Tokens.  */
#define LEX_ATTR 258
#define LEX_NUM 259
#define LEX_NULL 260
#define LEX_STR 261
#define LEX_IP 262
#define LEX_AND 263
#define LEX_OR 264
#define LEX_EQ 265
#define LEX_NEQ 266
#define LEX_NOT 267
#define LEX_GT 268
#define LEX_GTE 269
#define LEX_LT 270
#define LEX_LTE 271
#define LEX_LPAR 272
#define LEX_RPAR 273
#define LEX_EXIST 274
#define LEX_NEXIST 275
#define LEX_TERM 276




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 25 "grammar.y"

    char *string;
    int  number;



/* Line 1685 of yacc.c  */
#line 100 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


