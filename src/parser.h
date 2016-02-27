//
//  parser.h
//  software5_re
//
//  Created by KitayamaHideya on 2016/02/11.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#ifndef parser_h
#define parser_h

#include "compiler.h"

#define NUM_OF_PARSE_FUNC 28

/* parser_util.c */
extern parseNode * malloc_parseNode(void);
extern parseLeaf * malloc_parseLeaf(void);
extern parseObmit * malloc_parseObmit(void);

extern void set_evalTypep(struct PARSE_TREE *elementp, varType *typep);
extern parseNode * link_parseNode(parseNode *edgep, int structNum);
extern parseNode * link_parseNode_as_list(parseNode *edgep, int structNum);
extern parseLeaf * link_parseLeaf(parseNode *edgep, tokenList *tokenp);
extern struct PARSE_TREE * check_and_link_parseLeaf(
        int checkedNum, parseNode *edgep, tokenList *tokenp, ... /* checked (int )tokenCode */);
extern parseObmit * link_parseObmit(parseNode *edgep);

extern parseNode * back_to_parent(parseNode *nodep);

extern int search_empty_child(parseNode *nodep);
extern int search_child_by_structNum(parseNode *nodep, int structNum);
extern int search_child_by_structNum_on_list(parseNode *nodeListTailp, int structNum);
extern int search_my_child_num(struct PARSE_TREE *elementp);

extern _bool is_parseNode(struct PARSE_TREE *elementp);
extern _bool is_parse_list(struct PARSE_TREE *elementp);
extern _bool is_parseLeaf(struct PARSE_TREE *elementp);
extern _bool is_parseObmit(struct PARSE_TREE *elementp);

extern struct PARSE_TREE * error_parser(struct PARSE_TREE *elementp, const char *msgFormat, ...);

#endif /* parser_h */
