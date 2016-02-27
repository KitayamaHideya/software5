//
//  main.c
//  software5_re
//
//  Created by KitayamaHideya on 2016/01/18.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#include "compiler.h"

int main(int argc, const char * argv[]) {
    tokenList * tokenListHp = NULL;
    struct PARSE_TREE * parseTreeRp = NULL;
    
    if (argc < 2) {
        fprintf(stderr, "File name is not specified.\n");
        exit(EXIT_FAILURE);
    }
    
    /* make token list */
    tokenListHp = scanner(argv[1]);
//    dump_tokenList(tokenListHp);
    
    /* make parse tree */
    parseTreeRp = parser(tokenListHp);
    dump_parse_tree(parseTreeRp);
    
    /* check */
    
    /* generate CASLII code */
    
    /* release memory */
    free_tokenList(tokenListHp);
    
    return 0;
}
