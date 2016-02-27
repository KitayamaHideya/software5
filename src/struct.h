//
//  struct.h
//  software5_re
//
//  Created by KitayamaHideya on 2016/01/18.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#ifndef struct_h
#define struct_h

#include "const.h"

/* type definition */
typedef int _bool;  /* TRUE or FALSE */

/* token list */
typedef struct TOKEN_LIST {
    int lineNum;
    int tokenCode;
    char *tokenStr;
    int tokenInt;
    int tokenLen;
    struct TOKEN_LIST *nextp;
} tokenList;

/* parse tree */
typedef struct TYPE {
    int tokenType;
    int arySize;
    struct TYPE *elemTypep;
    struct TYPE *nextp;
} varType;

struct PARSE_TREE {
    int structNum;  /* structure number */
    int depth;
    int listNum;
    struct TYPE *evalTypep;
    struct PARSE_TREE *parentp;
    struct PARSE_TREE *child[NUM_OF_CHILD];
    tokenList *tokenp;
};

typedef struct PARSE_TREE parseNode;  /* DO NOT USE token */
typedef struct PARSE_TREE parseLeaf;  /* DO NOT USE child */
typedef struct PARSE_TREE parseObmit; /* DO NOT USE token AND child. THINK AS parse_node */

/* symbol table */
typedef struct LINE {
    int refLineNum;
    struct LINE *nextLinep;
} ref;

typedef struct ID {
    char *name;
    char *label;
    varType *idTypep;
    int isPara;
    int defLineNum;
    ref *idRefp;
    struct ID *nextp;
} varId;

typedef struct NAME_HASH_TABLE {
    int tbSize;
    int haveVar;
    varId **nameHashAry;
    struct NAME_HASH_TABLE *nextp;
    
    /* procedure information */
    char *procName;
    char *label;
    varType *paraTypep;
    int defLineNum;
    ref *idRefp;
} nameHashTb;

typedef struct PROC_HASH_TABLE {
    int tbSize;
    int haveProc;
    nameHashTb **procHashAry;
} procHashTb;

#endif /* struct_h */
