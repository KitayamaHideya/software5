//
//  compiler.h
//  software5_re
//
//  Created by KitayamaHideya on 2016/01/18.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#ifndef compiler_h
#define compiler_h

#include "struct.h"
#include "const.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

/* scanner.c */
extern tokenList * scanner(const char * mplFileName);
extern void free_tokenList(tokenList *headp);

/* parser.c */
extern struct PARSE_TREE * parser(tokenList *headp);

/* parser_util.c */
extern void free_parse_tree(struct PARSE_TREE *elementp);

///////

extern void dump_tokenList(tokenList *headp);
extern void dump_parse_tree(struct PARSE_TREE *elementp);

#endif /* compiler_h */
