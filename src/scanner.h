//
//  scanner.h
//  software5_re
//
//  Created by KitayamaHideya on 2016/02/06.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#ifndef scanner_h
#define scanner_h

#include "compiler.h"

#define MAX_STR_SIZE  1024
#define MAX_KEYWORD_SIZE 10  /* max string length of keyword */

#define NUM_OF_TOKEN	48  /* all token */
#define NUM_OF_KEYWORD	27  /* only keyword */

#define MAX_INT   32767

struct KEY {
    char * keyword;
    int tokenCode;
};

typedef struct T_CODE_STR {
    int lineNum;
    int tokenCode;
    char *tokenStr;
} tokenCodeStr;

extern void error_scanner(tokenCodeStr *eVal, char *msg);

extern tokenCodeStr * scan_token(FILE *mplFp);
extern int get_lineNumInMpl(void);

#endif /* scanner_h */
