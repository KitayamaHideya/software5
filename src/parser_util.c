//
//  parser_util.c
//  software5_re
//
//  Created by KitayamaHideya on 2016/02/11.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#include "parser.h"

/* private function */
struct PARSE_TREE * malloc_parse_tree(void);

parseNode * malloc_parseNode(void) {
    parseNode *newp = NULL;
    
    if ((newp = malloc_parse_tree()) == NULL) {
        fprintf(stderr, "malloc FAILED in malloc_parseNode.\n");
//        exit(EXIT_FAILURE);
        return NULL;
    }
    
    return newp;
}

parseLeaf * malloc_parseLeaf(void) {
    parseLeaf *newp = NULL;
    
    if ((newp = malloc_parse_tree()) == NULL) {
        fprintf(stderr, "can not malloc in malloc_parseLeaf.");
//        exit(EXIT_FAILURE);
        return NULL;
    }
    
    newp->structNum = P_TOKEN;
    
    return newp;
}

parseObmit * malloc_parseObmit(void) {
    parseObmit *newp = NULL;
    
    if ((newp = malloc_parse_tree()) == NULL) {
        fprintf(stderr, "can not malloc in malloc_parseObmit.");
//        exit(EXIT_FAILURE);
        return NULL;
    }
    
    newp->structNum = P_OBMIT;
    
    return newp;
}

void set_evalTypep(struct PARSE_TREE *elementp, varType *typep) {
    if (elementp != NULL) {
        elementp->evalTypep = typep;
    }
    return;
}

parseNode * link_parseNode(parseNode *edgep, int structNum) {
    int linkedChildNum;
    parseNode *childp;
    
    linkedChildNum = search_empty_child(edgep);
    
    childp = malloc_parseNode();
    
    childp->structNum = structNum;
    childp->depth = edgep->depth + 1;
    childp->parentp = edgep;
    
    edgep->child[linkedChildNum] = childp;
    
    return childp;
}

parseNode * link_parseNode_as_list(parseNode *edgep, int structNum) {
    parseNode *nextNodep;
    
    nextNodep = link_parseNode(edgep, structNum);
    
    nextNodep->depth = edgep->depth;
    nextNodep->listNum = edgep->listNum + 1;
    
    return nextNodep;
}

parseLeaf * link_parseLeaf(parseNode *edgep, tokenList *tokenp) {
    int linkedChildNum;
    parseLeaf *childp;
    
    linkedChildNum = search_empty_child(edgep);
    
    childp = malloc_parseLeaf();
    childp->depth = edgep->depth + 1;
    childp->parentp = edgep;
    childp->tokenp = tokenp;
    
    edgep->child[linkedChildNum] = childp;
    
    return childp;
}

struct PARSE_TREE * check_and_link_parseLeaf(
        int checkedNum, parseNode *edgep, tokenList *tokenp, ... /* checked (int )tokenCode */) {
    va_list checkedTokenCodeArgs;
    int linkTokenCode = T_ERROR;
    int checkedTokenCode = T_ERROR;
    int i;
    
    va_start(checkedTokenCodeArgs, tokenp);
    for (i=0; i<checkedNum; i++) {
        checkedTokenCode = va_arg(checkedTokenCodeArgs, int);
        if (tokenp->tokenCode == checkedTokenCode) {
            linkTokenCode = checkedTokenCode;
            break;
        }
    }
    va_end(checkedTokenCodeArgs);
    
    if (linkTokenCode != T_ERROR) {
        link_parseLeaf(edgep, tokenp);
        return edgep;
    }else{
        return error_parser(edgep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
    }
    
    /* never reached */
    return NULL;
}

parseObmit * link_parseObmit(parseNode *edgep) {
    int linkedChildNum;
    parseObmit *childp;
    
    linkedChildNum = search_empty_child(edgep);
    
    childp = malloc_parseObmit();
    childp->depth = edgep->depth + 1;
    childp->parentp = edgep;
    
    edgep->child[linkedChildNum] = childp;
    
    return childp;
}

parseNode * back_to_parent(parseNode *nodep) {
    parseNode *parentNodep = nodep;
    
    if (parentNodep->parentp != NULL) {
        while (is_parse_list(parentNodep)) {
            parentNodep = parentNodep->parentp;
        }
        
        parentNodep = parentNodep->parentp;
    }
    
    return parentNodep;
}

int search_empty_child(parseNode *nodep) {
    int emptyChild = P_NOT_FOUND;
    int i;
    
    if (!is_parseNode(nodep)) {
        /* error("This is not a node. Can not search."); */
        return -1;
    }
    
    for (i = 0; i < NUM_OF_CHILD; i++) {
        if (nodep->child[i] == NULL) {
            emptyChild = i;
            return emptyChild;
        }
    }
    
    /* not found */
    return emptyChild;
}

int search_child_by_structNum(parseNode *nodep, int structNum) {
    int haveChild = P_NOT_FOUND;
    int empty;
    int i;
    
    if (!is_parseNode(nodep)) {
        /* error("This is not a node. Can not search."); */
        return -1;
    }
    
    empty = search_empty_child(nodep);
    
    for (i = 0; i < empty; i++) {
        if (nodep->child[i]->structNum == structNum) {
            haveChild = i;
            return haveChild;
        }
    }
    
    /* not found */
    return haveChild;
}

int search_child_by_structNum_on_list(parseNode *nodeListTailp, int structNum) {
    parseNode *nodeIterp = nodeListTailp;
    parseNode *searchingp = nodeListTailp;
    int haveChild = P_NOT_FOUND;
    
    if (!is_parseNode(nodeListTailp)) {
        /* error("This is not a node. Can not search."); */
        return -1;
    }
    
    while (nodeIterp->depth == searchingp->depth) {
        searchingp = nodeIterp;
        
        haveChild = search_child_by_structNum(searchingp, structNum);
        if (haveChild != P_NOT_FOUND) {
            haveChild = 10*(searchingp->listNum) + haveChild;
            return haveChild;
        }
        
        nodeIterp = searchingp->parentp;
    }
    
    /* not found */
    return haveChild;
}

int search_my_child_num(struct PARSE_TREE *elementp) {
    parseNode *parentp = NULL;
    int i;
    
    if (elementp->parentp == NULL) {
        return -1;
    }
    
    parentp = elementp->parentp;
    
    for (i=0; i<NUM_OF_CHILD; i++) {
        if (parentp->child[i] == elementp) {
            return i;
        }
    }
    
    /* never reached */
    /* not found */
    return -1;
}

_bool is_parseNode(struct PARSE_TREE *elementp) {
    if (elementp->tokenp == NULL) {
        return TRUE;
    }else{
        return FALSE;
    }
}

_bool is_parse_list(struct PARSE_TREE *elementp) {
    if (elementp->listNum != 0) {
        return TRUE;
    }else{
        return FALSE;
    }
}

_bool is_parseLeaf(struct PARSE_TREE *elementp) {
    if (elementp->structNum == P_TOKEN) {
        return TRUE;
    }else{
        return FALSE;
    }
}

_bool is_parseObmit(struct PARSE_TREE *elementp) {
    if (elementp->structNum == P_OBMIT) {
        return TRUE;
    }else{
        return FALSE;
    }
}

void free_parse_tree(struct PARSE_TREE *elementp) {
    int empty;
    int i;
    
    if (!(is_parseNode(elementp))) {
        free(elementp);
        return;
    }
    
    empty = search_empty_child(elementp);
    if (empty != 0) {
        for (i=0; i<empty; i++) {
            free_parse_tree(elementp->child[i]);
        }
    }
    free(elementp);
    return;
}

struct PARSE_TREE * error_parser(struct PARSE_TREE *elementp, const char *msgFormat, ...) {
    va_list msgArgs;
    
    /* output error message */
    va_start(msgArgs, msgFormat);
    vfprintf(stderr, msgFormat, msgArgs);
    va_end(msgArgs);
    
    /* error handling on parse tree */
    while (elementp->structNum != P_ROOT) {
        elementp = elementp->parentp;
    }
    elementp->structNum = P_ERROR;
    
    return elementp;
}


/* private function */
struct PARSE_TREE * malloc_parse_tree(void) {
    struct PARSE_TREE *newp = NULL;
    int i;
    
    if((newp = (struct PARSE_TREE *)malloc(sizeof(struct PARSE_TREE))) == NULL) {
        return NULL;
    }
    
    /* Initialization */
    newp->structNum = -1;
    newp->depth = -1;
    newp->listNum = 0;
    newp->evalTypep = NULL;
    newp->parentp = NULL;
    newp->tokenp = NULL;
    for (i = 0; i < NUM_OF_CHILD; i++) {
        newp->child[i] = NULL;
    }
    
    return newp;
}