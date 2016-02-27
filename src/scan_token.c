//
//  scan_token.c
//  software5_re
//
//  Created by KitayamaHideya on 2016/02/08.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#include "scanner.h"

static tokenCodeStr sRetVal;
static char sTokenStr[MAX_STR_SIZE];

static char sCharBuf = '$';
static unsigned int sLineNumInMpl = 1;

/* keyword list */
static struct KEY sKeyAry[NUM_OF_KEYWORD] = {
    {"and", 	T_AND	},
    {"array",	T_ARRAY	},
    {"begin",	T_BEGIN	},
    {"boolean",	T_BOOLEAN},
    {"call",	T_CALL	},
    {"char",	T_CHAR	},
    {"div",		T_DIV	},
    {"do",		T_DO		},
    {"else",	T_ELSE	},
    {"end",		T_END	},
    {"false",	T_FALSE	},
    {"if",		T_IF		},
    {"integer",	T_INTEGER},
    {"not",		T_NOT	},
    {"of",		T_OF		},
    {"or",		T_OR		},
    {"procedure", T_PROCEDURE},
    {"program",	T_PROGRAM},
    {"read",	T_READ	},
    {"readln",	T_READLN	},
    {"return", T_RETURN	},
    {"then",	T_THEN	},
    {"true",	T_TRUE	},
    {"var",		T_VAR	},
    {"while",	T_WHILE	},
    {"write",	T_WRITE  },
    {"writeln",	T_WRITELN}
};

void init_sRetVal(void);
tokenCodeStr * copy_sRetVal(void);

void search_tokenCode_alpha(FILE *mplFp);
void search_tokenCode_numeric(FILE *mplFp);
void search_tokenCode_symbol(FILE *mplFp);
void search_tokenCode_string(FILE *mplFp);
void skip_comment(FILE *mplFp);
void scan_crlf(FILE *mplFp);

tokenCodeStr * scan_token(FILE *mplFp) {
    tokenCodeStr *retValp = NULL;
    
    init_sRetVal();
    
    if (sCharBuf == '$') {
        sCharBuf = fgetc(mplFp);
    }
    
    while (sRetVal.tokenCode == T_SKIP) {
        switch (sCharBuf) {
            case 'A' ... 'Z':  /* A~Z */
            case 'a' ... 'z':  /* a~z */
                search_tokenCode_alpha(mplFp);
                break;
                
            case '0' ... '9':  /* 0~9 */
                search_tokenCode_numeric(mplFp);
                break;
                
            case '(' ... '.':
            case ':' ... '>':
            case '[':
            case ']':  /* Symbols */
                search_tokenCode_symbol(mplFp);
                break;
                
            case '\'':  /* String */
                search_tokenCode_string(mplFp);
                break;
                
            case '/':
            case '{':  /* Comment */
                skip_comment(mplFp);
                break;
                
            case '\n':  /* LF */
            case '\r':  /* CR */
                scan_crlf(mplFp);
                break;
                
            default:  /* if cbuf is space or tab, skip that cbuf.
                       and others are error. */
                if (sCharBuf == ' ' || sCharBuf == '\t') {
                    sCharBuf = fgetc(mplFp);
                    /* SKIP */
                    break;
                }else{
                    if (sCharBuf != EOF) {
                        error_scanner(&sRetVal, "Undefined token.");
                    }else{
                        return NULL;
                    }
                }
        }
    }
    sRetVal.lineNum = sLineNumInMpl;
    
    retValp = copy_sRetVal();
    
    return retValp;
}

int get_lineNumInMpl(void) {
    return sLineNumInMpl;
}

void init_sRetVal(void) {
    
    sRetVal.lineNum = 0;
    sRetVal.tokenCode = T_SKIP;
    sTokenStr[0] = '\0';
    sRetVal.tokenStr = sTokenStr;
    
    return;
}

tokenCodeStr * copy_sRetVal(void) {
    tokenCodeStr *newp = NULL;
    char *tmpTokenStrp = NULL;
    
    if ((newp = (tokenCodeStr *)malloc(sizeof(tokenCodeStr))) == NULL) {
        fprintf(stderr, "malloc(tokenCodeStr) FAILED in copy_sRetVal\n");
        return NULL;
    }
    
    if ((tmpTokenStrp = (char *)malloc(sizeof(char)*strlen(sRetVal.tokenStr) + 1)) == NULL) {
        fprintf(stderr, "malloc(tokenCodeStr->tokenStr) FAILED in copy_sRetVal\n");
        return NULL;
    }
    
    newp->lineNum = sRetVal.lineNum;
    newp->tokenCode = sRetVal.tokenCode;
    
    strncpy(tmpTokenStrp, sRetVal.tokenStr, strlen(sRetVal.tokenStr));
    newp->tokenStr = tmpTokenStrp;
    
    return newp;
}

void search_tokenCode_alpha(FILE *mplFp) {
    int strlen = 0;
    int i;
    
    while (isalnum(sCharBuf)) {
        if (strlen >= MAX_STR_SIZE-1) {
            error_scanner(&sRetVal, "Over max-length(1023) of token.");
            return;
        }
        strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
        sCharBuf = fgetc(mplFp);
        strlen++;
    }
    
    if (strlen <= MAX_KEYWORD_SIZE) {
        for (i=0; i<NUM_OF_KEYWORD; i++) {
            if(strncmp(sRetVal.tokenStr, sKeyAry[i].keyword, MAX_KEYWORD_SIZE) == 0){
                sRetVal.tokenCode = sKeyAry[i].tokenCode;
                return;
            }
        }
    }
    
    sRetVal.tokenCode = T_NAME;
    return;
}

void search_tokenCode_numeric(FILE *mplFp) {
    int numDigits = 0;
    int numVal = 0;
    
    while (isdigit(sCharBuf)) {
        if (numDigits > 5) {
            error_scanner(&sRetVal, "Over max-integer-value.");
            return;
        }
        
        strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
        sCharBuf = fgetc(mplFp);
        numDigits++;
    }
    
    numVal = atoi(sRetVal.tokenStr);
    
    if (numVal > MAX_INT) {
        error_scanner(&sRetVal, "Over max-integer-value.");
    }else{
        sRetVal.tokenCode = T_NUMBER;
    }
    
    return;
}

void search_tokenCode_symbol(FILE *mplFp) {
    switch (sCharBuf) {
        case '<':
            strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
            sCharBuf = fgetc(mplFp);
            switch (sCharBuf) {
                case '>': sRetVal.tokenCode = T_NOTEQ; break;
                case '=': sRetVal.tokenCode = T_LEEQ;  break;
                default:  sRetVal.tokenCode = T_LE;    break;
            }
            
            if (sRetVal.tokenCode != T_LE) {
                strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
                sCharBuf = fgetc(mplFp);
            }
            break;
            
        case '>':
            strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
            sCharBuf = fgetc(mplFp);
            if (sCharBuf == '=') {
                sRetVal.tokenCode = T_GREQ;
                strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
                sCharBuf = fgetc(mplFp);
            }else{
                sRetVal.tokenCode = T_GR;
            }
            break;
            
        case ':':
            strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
            sCharBuf = fgetc(mplFp);
            if (sCharBuf == '=') {
                sRetVal.tokenCode = T_ASSIGN;
                strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
                sCharBuf = fgetc(mplFp);
            }else{
                sRetVal.tokenCode = T_COLON;
            }
            break;
            
        default:
            strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
            switch (sCharBuf) {
                case '+': sRetVal.tokenCode = T_PLUS;     break;
                case '-': sRetVal.tokenCode = T_MINUS;    break;
                case '*': sRetVal.tokenCode = T_STAR;     break;
                case '=': sRetVal.tokenCode = T_EQUAL;    break;
                case '(': sRetVal.tokenCode = T_LPAREN;   break;
                case ')': sRetVal.tokenCode = T_RPAREN;   break;
                case '[': sRetVal.tokenCode = T_LSQPAREN; break;
                case ']': sRetVal.tokenCode = T_RSQPAREN; break;
                case '.': sRetVal.tokenCode = T_DOT;      break;
                case ',': sRetVal.tokenCode = T_COMMA;    break;
                case ';': sRetVal.tokenCode = T_SEMI;     break;
                default: /* never reached */ break;
            }
            sCharBuf = fgetc(mplFp);
            break;
    }
    
    return;
}

void search_tokenCode_string(FILE *mplFp) {  // TODO: fix strlen count when escape ''.
    char charBuf2;
    int strlen = 0;
    
    sCharBuf = fgetc(mplFp);
    charBuf2 = fgetc(mplFp);
    while (1) {
        if (sCharBuf == '\'') {
            if (charBuf2 == '\'') {
                /* escape '' */
                if (strlen >= MAX_STR_SIZE-1) {
                    error_scanner(&sRetVal, "Over max-length(1023) of string.");
                    return;
                }
                
                strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
                strncat(sRetVal.tokenStr, (char *)&charBuf2, 1);
                sCharBuf = charBuf2;
                charBuf2 = fgetc(mplFp);
                
                strlen++;
                
            }else{
                /* end of string */
                break;
            }
            
        }else if (sCharBuf == '\n' || sCharBuf == '\r' || sCharBuf == EOF || charBuf2 == EOF){  /* ERROR */
            error_scanner(&sRetVal, "Not found \"\'\".");
            return;
            
        }else{
            if (strlen >= MAX_STR_SIZE-1) {
                error_scanner(&sRetVal, "Over max-length(1023) of string.");
                return;
            }
            
            strncat(sRetVal.tokenStr, (char *)&sCharBuf, 1);
        }
        
        sCharBuf = charBuf2;
        charBuf2 = fgetc(mplFp);
        strlen++;
    }
    
    sCharBuf = charBuf2;
    
    sRetVal.tokenCode = T_STRING;
    return;
}

void skip_comment(FILE *mplFp) {
    char charBuf2;
    
    switch (sCharBuf) {
        case '/':
            sCharBuf = fgetc(mplFp);
            charBuf2 = fgetc(mplFp);
            
            if (sCharBuf == '*') {
                while (1) {
                    sCharBuf = charBuf2;
                    charBuf2 = fgetc(mplFp);
                    
                    if (sCharBuf == '*' && charBuf2 == '/') {
                        /* end of comment */
                        break;
                        
                    }else if (sCharBuf == '\r' || sCharBuf == '\n') {
                        if (sCharBuf == '\r' && charBuf2 == '\n') {  /* CR + LF */
                            sLineNumInMpl++;
                            sCharBuf = charBuf2;
                            charBuf2 = fgetc(mplFp);
                        }else{  /* others */
                            sLineNumInMpl++;
                        }
                        
                    }else if (sCharBuf == EOF || charBuf2 == EOF){  /* ERROR */
                        error_scanner(&sRetVal, "Not found \"*/\".");
                        return;
                    }
                }
            }else{  /* ERROR */
                return;
            }
            break;
            
        case '{':
            while (sCharBuf != '}') {  /* end of comment */
                sCharBuf = fgetc(mplFp);
                
                if (sCharBuf == EOF) {  /* ERROR */
                    error_scanner(&sRetVal, "Not found \"}\".");
                    return;
                }
            }
            break;
            
        default:
            /* never reached */
            break;
    }
    
    sCharBuf = fgetc(mplFp);
    
    sRetVal.tokenCode = T_SKIP;
    return;
}

void scan_crlf(FILE *mplFp) {
    switch (sCharBuf) {
        case '\n':  /* LF */
            sLineNumInMpl++;
            sCharBuf = fgetc(mplFp);
            break;
            
        case '\r':
            sCharBuf = fgetc(mplFp);
            if (sCharBuf == '\n') {  /* CR+LF */
                sLineNumInMpl++;
                sCharBuf = fgetc(mplFp);
            }else{               /* CR */
                sLineNumInMpl++;
            }
            break;
            
        default:
            /* never reached */
            break;
    }
    
    sRetVal.tokenCode = T_SKIP;
    return;
}
