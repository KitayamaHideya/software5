//
//  const.h
//  software5_re
//
//  Created by KitayamaHideya on 2016/01/18.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#ifndef const_h
#define const_h

/* _bool */
#define TRUE  1
#define FALSE 0

/* token code */
#define T_ERROR  -1  /* error occured in scanning */
#define T_SKIP        0   /* scan skipped */

#define	T_NAME		1	/* name: A string of alphanumeric characters, begin with alphabetic character */
#define	T_PROGRAM	2	/* program: Keyword */
#define	T_VAR		3	/* var: Keyword */
#define	T_ARRAY	    4	/* array: Keyword */
#define	T_OF		    5	/* of: Keyword */
#define	T_BEGIN		6	/* begin: Keyword */
#define	T_END		7  	/* end: Keyword */
#define	T_IF		    8  	/* if: Keyword */
#define	T_THEN		9	/* then: Keyword */
#define	T_ELSE		10	/* else: Keyword */
#define	T_PROCEDURE	11	/* procedure: Keyword */
#define	T_RETURN	    12	/* return: Keyword */
#define	T_CALL	    13	/* call: Keyword */
#define	T_WHILE		14	/* while: Keyword */
#define	T_DO		    15 	/* do: Keyword */
#define	T_NOT		16	/* not: Keyword */
#define	T_OR		    17	/* or: Keyword */
#define	T_DIV		18 	/* div: Keyword */
#define	T_AND		19 	/* and: Keyword */
#define	T_CHAR		20	/* char: Keyword */
#define	T_INTEGER	21	/* integer: Keyword */
#define	T_BOOLEAN	22 	/* boolean: Keyword */
#define	T_READLN	    23	/* readln: Keyword */
#define	T_WRITELN	24	/* writeln: Keyword */
#define	T_TRUE		25	/* true: Keyword */
#define	T_FALSE		26	/* false: Keyword */
#define	T_NUMBER	    27	/* unsigned integer */
#define	T_STRING	    28	/* string */
#define	T_PLUS		29	/* +: Symbol */
#define	T_MINUS		30 	/* -: Symbol */
#define	T_STAR		31 	/* *: Symbol */
#define	T_EQUAL		32 	/* =: Symbol */
#define	T_NOTEQ		33 	/* <>: Symbol */
#define	T_LE		    34 	/* <: Symbol */
#define	T_LEEQ		35 	/* <=: Symbol */
#define	T_GR		    36	/* >: Symbol */
#define	T_GREQ		37	/* >=: Symbol */
#define	T_LPAREN	    38 	/* (: Symbol */
#define	T_RPAREN	    39 	/* ): Symbol */
#define	T_LSQPAREN	40	/* [: Symbol */
#define	T_RSQPAREN	41 	/* ]: Symbol */
#define	T_ASSIGN	    42	/* :=: Symbol */
#define	T_DOT		43 	/* .: Symbol */
#define	T_COMMA	    44	/* ,: Symbol */
#define	T_COLON		45	/* :: Symbol */
#define	T_SEMI		46	/* ;: Symbol */
#define	T_READ	    47	/* read: Keyword */
#define	T_WRITE	    48	/* write: Keyword */

/* parse tree */
#define NUM_OF_CHILD 7
#define P_NOT_FOUND 7

/* structure number */
#define P_ROOT 0
#define P_PROGRAM 1
#define P_BLOCK 2
#define P_VAR_DEC 3
#define P_TYPE 4
#define P_PROCEDURE 5
#define P_DUMMY_ARG 6
#define P_COMPOUND 7

#define P_VAR 8
#define P_VAR_LIST 9
#define P_EXP 10
#define P_EXP_LIST 11

#define P_SIMPLE_EXP 12
#define P_TERM 13
#define P_FACTOR 14

#define P_OUTPUT 15

#define P_STATEMENT 20
#define P_ASSIGNMENT 21
#define P_IF 22
#define P_WHILE 23
#define P_CALL 24
#define P_RETURN 25
#define P_READ 26
#define P_WRITE 27

#define P_TOKEN 16
#define P_OBMIT 17

#define P_ERROR 18

#endif /* const_h */
