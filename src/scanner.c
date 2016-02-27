//
//  scanner.c
//  software5_re
//
//  Created by KitayamaHideya on 2016/02/06.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#include "scanner.h"

tokenList * malloc_tokenList(void);
tokenList * append_tokenList(tokenList *tailp, tokenCodeStr *retScanTokenp);

tokenList * scanner(const char * mplFileName) {
    FILE *mplFp;
    tokenList *tokenListHp = NULL;
    tokenList *tokenListIp = NULL;
    tokenCodeStr *retScanTokenp = NULL;
    
    if ((mplFp = fopen(mplFileName, "r")) == NULL) {
        fprintf(stderr, "mpl-file open is FAILED.\n");
        exit(EXIT_FAILURE);
    }
    
    tokenListHp = malloc_tokenList();
    tokenListIp = tokenListHp;
    
    if (tokenListHp != NULL) {
        while (((retScanTokenp = scan_token(mplFp)) != NULL) && (tokenListIp != NULL)) {
            if (retScanTokenp->tokenCode == T_ERROR) {
                break;
            }
            
            tokenListIp = append_tokenList(tokenListIp, retScanTokenp);
            free(retScanTokenp);
            retScanTokenp = NULL;
        }
    }
    
    fclose(mplFp);
    
    return tokenListHp;
}

tokenList * malloc_tokenList(void) {
    tokenList *newp = NULL;
    
    if((newp = (tokenList *)malloc(sizeof(tokenList))) == NULL) {
        fprintf(stderr, "malloc FAILED in malloc_tokenlist.\n");
        return NULL;
    }
    
    /* Initialization */
    newp->lineNum = 0;
    newp->tokenCode = -1;
    newp->tokenStr = NULL;
    newp->tokenInt = -1;
    newp->tokenLen = -1;
    newp->nextp = NULL;
    
    return newp;
}

tokenList * append_tokenList(tokenList *tailp, tokenCodeStr *retScanTokenp) {
    tokenList *newp = NULL;
    
    if((newp = malloc_tokenList()) == NULL) {
        return NULL;
    }
    
    newp->lineNum = retScanTokenp->lineNum;
    newp->tokenCode = retScanTokenp->tokenCode;
    newp->tokenStr = retScanTokenp->tokenStr;
    if (retScanTokenp->tokenCode == T_NUMBER) { newp->tokenInt = atoi(retScanTokenp->tokenStr); }
    newp->tokenLen = (int )strlen(retScanTokenp->tokenStr);
    
    tailp->nextp = newp;
    
    return newp;
}

void free_tokenList(tokenList *headp) {
    tokenList *freep, *tmpp;
    freep = headp;
    
    while (freep != NULL) {
        if (freep->tokenStr != NULL) {
            free(freep->tokenStr);
            freep->tokenStr = NULL;
        }
        tmpp = freep->nextp;
        free(freep);
        freep = NULL;
        
        freep = tmpp;
    }
    
    return;
}

void error_scanner(tokenCodeStr *eValp, char *msg) {
    fprintf(stderr, "%s, at line_%d\n", msg, get_lineNumInMpl());
    
    eValp->tokenCode = T_ERROR;
    return;
}

///////

void dump_tokenList(tokenList *head) {
    tokenList *iterp = head;
    
    while (iterp != NULL) {
        printf("----------\n");
        printf("lineNum:%d\n", iterp->lineNum);
        printf("tokenCode:%d\n", iterp->tokenCode);
        printf("tokenStr:%s\n", iterp->tokenStr);
        printf("tokenInt:%d\n", iterp->tokenInt);
        printf("tokenLen:%d\n", iterp->tokenLen);
        printf("----------\n");
        
        iterp = iterp->nextp;
    }
    
    return;
}
