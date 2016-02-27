//
//  parser.c
//  software5_re
//
//  Created by KitayamaHideya on 2016/02/11.
//  Copyright © 2016年 KitayamaHideya. All rights reserved.
//

#include "parser.h"

/* private functions */
parseNode * parse_program(parseNode *nodep, tokenList *tokenp);
parseNode * parse_block(parseNode *nodep, tokenList *tokenp);
parseNode * parse_var_dec(parseNode *nodep, tokenList *tokenp);
parseNode * parse_type(parseNode *nodep, tokenList *tokenp);
parseNode * parse_procedure(parseNode *nodep, tokenList *tokenp);
parseNode * parse_dummy_arg(parseNode *nodep, tokenList *tokenp);
parseNode * parse_compound(parseNode *nodep, tokenList *tokenp);

parseNode * parse_var(parseNode *nodep, tokenList *tokenp);
parseNode * parse_var_list(parseNode *nodep, tokenList *tokenp);
parseNode * parse_exp(parseNode *nodep, tokenList *tokenp);
parseNode * parse_exp_list(parseNode *nodep, tokenList *tokenp);

parseNode * parse_simple_exp(parseNode *nodep, tokenList *tokenp);
parseNode * parse_term(parseNode *nodep, tokenList *tokenp);
parseNode * parse_factor(parseNode *nodep, tokenList *tokenp);

parseNode * parse_output(parseNode *nodep, tokenList *tokenp);

parseNode * parse_statement(parseNode *nodep, tokenList *tokenp);
parseNode * parse_assignment(parseNode *nodep, tokenList *tokenp);
parseNode * parse_if(parseNode *nodep, tokenList *tokenp);
parseNode * parse_while(parseNode *nodep, tokenList *tokenp);
parseNode * parse_call(parseNode *nodep, tokenList *tokenp);
parseNode * parse_return(parseNode *nodep, tokenList *tokenp);
parseNode * parse_read(parseNode *nodep, tokenList *tokenp);
parseNode * parse_write(parseNode *nodep, tokenList *tokenp);

struct PARSE_TREE * parser(tokenList *headp) {
    parseNode * (* parseFunc[NUM_OF_PARSE_FUNC])(parseNode *, tokenList *) = {
        NULL/* root */, parse_program, parse_block, parse_var_dec, parse_type, parse_procedure,
        parse_dummy_arg, parse_compound, parse_var, parse_var_list, parse_exp, parse_exp_list,
        parse_simple_exp, parse_term, parse_factor, parse_output,
        NULL/* token */, NULL/* obmit */, NULL/* error */, NULL/* none */, parse_statement,
        parse_assignment, parse_if, parse_while, parse_call, parse_return,
        parse_read, parse_write};
    
    tokenList *tokenIterp;
    parseNode *rootp = NULL;
    parseNode *parseIterp;
    parseNode *parsingp;
    
    if (headp == NULL) {
        return NULL;
    }
    
    /* root */
    rootp = malloc_parseNode();
    rootp->structNum = P_ROOT;
    rootp->depth = 0;
    
    /* iterator initialize */
    parseIterp = rootp;
    tokenIterp = headp->nextp;
    
    /* parse */
    while (tokenIterp != NULL) {
        parsingp = parseIterp;
        
        if (parseFunc[parsingp->structNum] != NULL) {
            /* structNum == P_PROGRAM ~ P_OUTPUT, P_STATEMENT ~ P_WRITE */
            parseIterp = (*parseFunc[parsingp->structNum])(parseIterp, tokenIterp);
            
        }else{
            switch (parsingp->structNum) {
                case P_ROOT:
                    if (parseIterp->child[0] == NULL) {
                        if (tokenIterp->tokenCode == T_PROGRAM) {
                            parseIterp = link_parseNode(rootp, P_PROGRAM);
                        }else{
                            parseIterp = error_parser(parseIterp, "Not found: 'program' at line_%d", tokenIterp->lineNum);
                        }
                    }else{
                        if (tokenIterp->nextp != NULL) {
                            fprintf(stderr, "Too many program part.\n");
                            rootp->structNum = P_ERROR;
                        }
                        goto PARSE_OUT;
                    }
                    break;
                    
                case P_OBMIT:
                    parseIterp = back_to_parent(parseIterp);
                    break;
                    
                case P_ERROR:
                    goto PARSE_OUT;
                    
                default:
                    /* never reached */
                    printf("error??");
                    break;
            }
        }
        
        if (parseIterp == parsingp) {
            tokenIterp = tokenIterp->nextp;
        }
    }
    
PARSE_OUT:
    if ((rootp->structNum != P_ERROR) && (parseIterp->structNum != P_ROOT)) {
        fprintf(stderr, "Illegal termination of MPPL file.\n");
        rootp->structNum = P_ERROR;
    }
    
    return rootp;
}

/* private functions */
parseNode * parse_program(parseNode *nodep, tokenList *tokenp) {
    parseNode *retp = NULL;
    
    /* keyword: 'program' */
    if (nodep->child[0] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_PROGRAM);
    }
    
    /* ^^^ 1st Access ^^^ */
    
    /* name: program's name */
    if (nodep->child[1] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NAME);
    }
    
    /* ^^^ 2nd Access ^^^ */
    
    /* symbol: ';' */
    if (nodep->child[2] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_SEMI);
    }
    
    /* ^^^ 3rd Access ^^^ */
    
    /* part: P_BLOCK */
    if (nodep->child[3] == NULL) {
        return link_parseNode(nodep, P_BLOCK);
    }
    
    /* ^^^ 4th Access ^^^ */
    
    /* symbol: '.' */
    if (nodep->child[4] == NULL) {
        retp = check_and_link_parseLeaf(1, nodep, tokenp, T_DOT);
        if (retp == nodep) {
            return back_to_parent(nodep);
        }else{
            return retp;
        }
    }
    
    /* ^^^ 5th Access ^^^ */
    
    /* never reached */
    return back_to_parent(nodep);
}

parseNode * parse_block(parseNode *nodep, tokenList *tokenp) {
    int empty;
    
    empty = search_empty_child(nodep);
    
    if (empty == 6) {
        /* extend a list, excepting the case that child[6]=P_COMPOUND. */
        if (tokenp->tokenCode != T_BEGIN) {
            return link_parseNode_as_list(nodep, P_BLOCK);
            
        }else{
            /* if child[6]=P_COMPOUND, link parse_node(P_COMPOUND) to child[6]. */
            if (search_child_by_structNum_on_list(nodep, P_COMPOUND) == P_NOT_FOUND) {
                return link_parseNode(nodep, P_COMPOUND);
            }else{
                return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
            }
        }
        
    }else{  /* others */
        if (search_child_by_structNum_on_list(nodep, P_COMPOUND) == P_NOT_FOUND) {
            switch (tokenp->tokenCode) {
                    /* P_COMPOUND ONLY ONE TIME */
                case T_VAR:  /* variable declaration */
                    return link_parseNode(nodep, P_VAR_DEC);
                    
                case T_PROCEDURE:  /* procedure declaration */
                    return link_parseNode(nodep, P_PROCEDURE);
                    
                case T_BEGIN:  /* compound */
                    return link_parseNode(nodep, P_COMPOUND);
                    
                default:
                    return error_parser(nodep,
                                        "Incorrect token: '%s' at line_%d\n"
                                        "(Not found: compound_part[begin ... end])",
                                        tokenp->tokenStr, tokenp->lineNum);
            }
            
        }else{
            return back_to_parent(nodep);
        }
    }
    
    /* never reached */
    return back_to_parent(nodep);
}

parseNode * parse_var_dec(parseNode *nodep, tokenList *tokenp) {
    /* keyword: 'var' or part: P_OBMIT */
    if (nodep->child[0] == NULL) {
        if (is_parse_list(nodep)) {
            return link_parseObmit(nodep);
        }else{
            return check_and_link_parseLeaf(1, nodep, tokenp, T_VAR);
        }
    }
    
    /* ^^^ 1st Access ^^^ */
    
    /* part: P_VAR_LIST */
    if (nodep->child[1] == NULL) {
        return link_parseNode(nodep, P_VAR_LIST);
    }
    
    /* ^^^ 2nd Access ^^^ */
    
    /* keyword: ':' */
    if (nodep->child[2] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_COLON);
    }
    
    /* ^^^ 3rd Access ^^^ */
    
    /* part: P_TYPE */
    if (nodep->child[3] == NULL) {
        return link_parseNode(nodep, P_TYPE);
    }
    
    /* ^^^ 4th Access ^^^ */
    
    /* keyword: ';' */
    if (nodep->child[4] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_SEMI);
    }
    
    /* ^^^ 5th Access ^^^ */
    
    /* extend a list or end */
    if (tokenp->tokenCode == T_NAME) {
        return link_parseNode_as_list(nodep, P_VAR_DEC);
    }
    
    /* The end of P_VAR_DEC */
    return back_to_parent(nodep);
}

parseNode * parse_type(parseNode *nodep, tokenList *tokenp) {
    /* keyword: 'integer', 'boolean', 'char', 'array' */
    if (nodep->child[0] == NULL) {
        return check_and_link_parseLeaf(4, nodep, tokenp, T_INTEGER, T_BOOLEAN, T_CHAR, T_ARRAY);
    }
    
    /* ^^^ 1st Access ^^^ */
    
    /* keyword: '[' */
    if (nodep->child[1] == NULL) {
        if (nodep->child[0]->tokenp->tokenCode == T_ARRAY) {
            return check_and_link_parseLeaf(1, nodep, tokenp, T_LSQPAREN);
        }else{
            /* The end of P_TYPE if type is standard. */
            return back_to_parent(nodep);
        }
    }
    
    /* ^^^ 2nd Access ^^^ */
    
    /* keyword: unsigned integer */
    if (nodep->child[2] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NUMBER);
    }
    
    /* ^^^ 3rd Access ^^^ */
    
    /* keyword: ']' */
    if (nodep->child[3] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_RSQPAREN);
    }
    
    /* ^^^ 4th Access ^^^ */
    
    /* keyword: 'of' */
    if (nodep->child[4] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_OF);
    }
    
    /* ^^^ 5th Access ^^^ */
    
    /* keyword: 'integer', 'boolean', 'char' */
    if (nodep->child[5] == NULL) {
        return check_and_link_parseLeaf(3, nodep, tokenp, T_INTEGER, T_BOOLEAN, T_CHAR);
    }
    
    /* ^^^ 6th Access ^^^ */
    
    /* The end of P_TYPE */
    return back_to_parent(nodep);
}

parseNode * parse_procedure(parseNode *nodep, tokenList *tokenp) {
    /* keyword: 'procedure' */
    if (nodep->child[0] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_PROCEDURE);
    }
    
    /* ^^^ 1st Access ^^^ */
    
    /* name: procedure's name */
    if (nodep->child[1] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NAME);
    }
    
    /* ^^^ 2nd Access ^^^ */
    
    /* part: P_DUMMY_ARG or P_OBMIT */
    if (nodep->child[2] == NULL) {
        if (tokenp->tokenCode == T_LPAREN) {
            return link_parseNode(nodep, P_DUMMY_ARG);
        }else{
            return link_parseObmit(nodep);
        }
    }
    
    /* ^^^ 3rd Access ^^^ */
    
    /* keyword: ';' */
    if (nodep->child[3] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_SEMI);
    }
    
    /* ^^^ 4th Access ^^^ */
    
    /* part: P_VAR_DEC or P_OBMIT */
    if (nodep->child[4] == NULL) {
        if (tokenp->tokenCode == T_VAR) {
            return link_parseNode(nodep, P_VAR_DEC);
        }else{
            return link_parseObmit(nodep);
        }
    }
    
    /* ^^^ 5th Access ^^^ */
    
    /* part: P_COMPOUND */
    if (nodep->child[5] == NULL) {
        return link_parseNode(nodep, P_COMPOUND);
    }
    
    /* ^^^ 6th Access ^^^ */
    
    /* keyword: ';' */
    if (nodep->child[6] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_SEMI);
    }
    
    /* ^^^ 7th Access ^^^ */
    
    /* The end of P_PROCEDURE */
    return back_to_parent(nodep);
}

parseNode * parse_dummy_arg(parseNode *nodep, tokenList *tokenp) {
    // ( 変数名の並び : 型 [; 変数名の並び : 型 ] or )
    /* keyword: ';'(on list) or '('(on top) */
    if (nodep->child[0] == NULL) {
        if (is_parse_list(nodep)) {
            return check_and_link_parseLeaf(1, nodep, tokenp, T_SEMI);
        }else{
            return check_and_link_parseLeaf(1, nodep, tokenp, T_LPAREN);
        }
    }
    
    /* ^^^ 1st Access ^^^ */
    
    /* part: P_VAR_LIST */
    if (nodep->child[1] == NULL) {
        return link_parseNode(nodep, P_VAR_LIST);
    }
    
    /* ^^^ 2nd Access ^^^ */
    
    /* keyword: ':' */
    if (nodep->child[2] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_COLON);
    }
    
    /* ^^^ 3rd Access ^^^ */
    
    /* part: P_TYPE */
    if (nodep->child[3] == NULL) {
        return link_parseNode(nodep, P_TYPE);
    }
    
    /* ^^^ 4th Access ^^^ */
    
    /* keyword: ')'(on top) or part: P_DUMMY_ARG(on list) */
    if (nodep->child[4] == NULL) {
        if (tokenp->tokenCode == T_RPAREN) {  /* The end of P_DUMMY_ARG */
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{
            if (tokenp->tokenCode == T_SEMI) {  /* extend a list */
                return link_parseNode_as_list(nodep, P_DUMMY_ARG);
            }else{
                return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
            }
        }
    }
    
    /* ^^^ 5th Access ^^^ */
    
    /* The end of P_DUMMY_ARG */
    return back_to_parent(nodep);
}

parseNode * parse_compound(parseNode *nodep, tokenList *tokenp) {
    int empty;
    
    // (begin 文 [; 文 ; 文 ->) (; 文 ; 文 ; 文 ->) ... (... 文 end)]
    //空文はparse_statementにて処理
    
    empty = search_empty_child(nodep);
    
    /* check the end of compound */
    if ((empty != 0 && nodep->child[empty-1]->structNum == P_TOKEN &&
         nodep->child[empty-1]->tokenp->tokenCode == T_END) || (empty == P_NOT_FOUND)) {
        return back_to_parent(nodep);
    }
    
    /* compound is began with 'begin' token */
    if ((!is_parse_list(nodep)) && empty == 0) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_BEGIN);
    }
    
    /* statement is need at least one */
    if ((!is_parse_list(nodep)) && empty == 1) {
        return link_parseNode(nodep, P_STATEMENT);
    }
    
    if (empty == 6) {
        if (tokenp->tokenCode == T_END) {
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{
            return link_parseNode_as_list(nodep, P_COMPOUND);
        }
        
    }else{
        if ((empty % 2) == 0) {  /* empty == 0, 2, 4 */
            /* keyword: ';' or 'end' */
            return check_and_link_parseLeaf(2, nodep, tokenp, T_SEMI, T_END);
            
        }else{  /* empty == 1, 3, 5 */
            /* part: P_STATEMENT */
            return link_parseNode(nodep, P_STATEMENT);
        }
    }
    
    /* never reached */
    return back_to_parent(nodep);
}


parseNode * parse_var(parseNode *nodep, tokenList *tokenp) {
    //変数名
    if (nodep->child[0] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NAME);
    }
    
    /* ^^^ 1st Access ^^^ */
    
    //式(省略可)
    if (nodep->child[1] == NULL) {
        if (tokenp->tokenCode == T_LSQPAREN) {  //式あり
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{  //式省略
            return back_to_parent(nodep);  //終端の省略->親ノードへ戻る
        }
    }
    
    /* ^^^ 2nd Access ^^^ */
    
    //以下、式が省略されていない場合
    if (nodep->child[2] == NULL) {  //式
        return link_parseNode(nodep, P_EXP);
    }
    
    /* ^^^ 3rd Access ^^^ */
    
    
    if (nodep->child[3] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_RSQPAREN);
    }
    
    /* ^^^ 4th Access ^^^ */
    
    //変数終端
    return back_to_parent(nodep);
}

parseNode * parse_var_list(parseNode *nodep, tokenList *tokenp) {
    int empty;
    
    // 0->変数, 1->カンマ, 2->変数, 3->カンマ, 4->変数, [5->カンマ, 6->次]
    empty = search_empty_child(nodep);
    
    //少なくとも1つは変数がある
    if ((!is_parse_list(nodep)) && (empty == 0)) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NAME);
    }
    
    if ((empty % 2) == 0 && empty != 6) {  //変数(empty = 0, 2, 4)
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NAME);
    }
    
    if ((empty % 2) == 1) {  //デリミタ(カンマ) (empty = 1, 3, 5)
        if (tokenp->tokenCode == T_COMMA) {
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }
//        return check_and_link_parseLeaf(1, nodep, tokenp, T_COMMA);
    }
    
    if (empty == 6) {  //リスト延長(empty = 6)
        if (tokenp->tokenCode == T_NAME) {
            return link_parseNode_as_list(nodep, P_VAR_LIST);
        }else{
            return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
        }
    }
    
    //変数リスト終端
    return back_to_parent(nodep);
}

parseNode * parse_exp(parseNode *nodep, tokenList *tokenp) {
    // 左結合に注意
    // (単純式 [関係演算子 単純式] ->) ( 省略 [関係演算子 単純式] ->)
    
    if (nodep->child[0] == NULL) {
        if (is_parse_list(nodep)) {  //リスト時は省略
            return link_parseObmit(nodep);
        }else{  //単純式は少なくとも1つは必須
            return link_parseNode(nodep, P_SIMPLE_EXP);
        }
    }
    
    if (nodep->child[1] == NULL) {
        switch (tokenp->tokenCode) {
            case T_EQUAL:
            case T_NOTEQ:
            case T_LE:
            case T_LEEQ:
            case T_GR:
            case T_GREQ:  //関係演算子
                link_parseLeaf(nodep, tokenp);
                return nodep;
                
            default:  //式終端
                return back_to_parent(nodep);
        }
    }
    
    if (nodep->child[2] == NULL) {  //単純式
        return link_parseNode(nodep, P_SIMPLE_EXP);
    }
    
    if (nodep->child[3] == NULL) {
        switch (tokenp->tokenCode) {
            case T_EQUAL:
            case T_NOTEQ:
            case T_LE:
            case T_LEEQ:
            case T_GR:
            case T_GREQ:  //関係演算子 -> リスト延長
                return link_parseNode_as_list(nodep, P_EXP);
                
            default:  //式終端
                return back_to_parent(nodep);
        }
    }
    
    //式終端(入れ子状態)
    return back_to_parent(nodep);
}

parseNode * parse_exp_list(parseNode *nodep, tokenList *tokenp) {
    int empty;
    
    // 0->式, 1->カンマ, 2->式, 3->カンマ, 4->式, [5->カンマ, 6->次]
    empty = search_empty_child(nodep);
    
    //少なくとも1つは式がある
    if ((!is_parse_list(nodep)) && (empty == 0)) {
        return link_parseNode(nodep, P_EXP);
    }
    
    if ((empty % 2) == 0 && empty != 6) {  //式(empty = 0, 2, 4)
        return link_parseNode(nodep, P_EXP);
    }
    
    if ((empty % 2) == 1) {  //デリミタ(カンマ) (empty = 1, 3, 5)
        if (tokenp->tokenCode == T_COMMA) {
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{
            return back_to_parent(nodep);  //式の並び終端
        }
    }
    
    if (empty == 6) {  //リスト延長(empty = 6) オーバヘッド考慮なし
        return link_parseNode_as_list(nodep, P_EXP_LIST);
    }
    
    //式の並び終端 通常通過しない
    return back_to_parent(nodep);
}


parseNode * parse_simple_exp(parseNode *nodep, tokenList *tokenp) {
    // 左結合に注意
    // (['+'or'-'](省略可) 項 [->]) (加法演算子 項 ->) ...
    
    if (nodep->child[0] == NULL) {
        switch (tokenp->tokenCode) {
                //加法演算子
            case T_PLUS:
            case T_MINUS:
                link_parseLeaf(nodep, tokenp);
                return nodep;
                
            case T_OR:
                if (!is_parse_list(nodep)) {  //先頭に'or'は不可
                    return error_parser(nodep, "'or' can not be a top of simple-exp at line_%d",
                                        tokenp->tokenStr, tokenp->lineNum);
                }else{
                    link_parseLeaf(nodep, tokenp);
                    return nodep;
                }
                
            default:
                if (!is_parse_list(nodep)) {  //先頭に限り省略可
                    return link_parseObmit(nodep);
                }else{
                    return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
                }
        }
    }
    
    if (nodep->child[1] == NULL) {
        //項
        return link_parseNode(nodep, P_TERM);
    }
    
    if (nodep->child[2] == NULL) {
        switch (tokenp->tokenCode) {
            case T_PLUS:
            case T_MINUS:
            case T_OR:  //加法演算子->リスト延長
                return link_parseNode_as_list(nodep, P_SIMPLE_EXP);
                
            default:  //単純式終端
                return back_to_parent(nodep);
        }
    }
    
    //単純式(入れ子状態)の終端
    return back_to_parent(nodep);
}

parseNode * parse_term(parseNode *nodep, tokenList *tokenp) {
    // 左結合に注意
    // (因子 [乗法演算子 因子 ->]) ((省略) 乗法演算子 因子 ->) ...
    
    if (nodep->child[0] == NULL) {
        if (is_parse_list(nodep)) {  //リスト時は省略
            return link_parseObmit(nodep);
        }else{  //因子は少なくとも1つは必要
            return link_parseNode(nodep, P_FACTOR);
        }
    }
    
    if (nodep->child[1] == NULL) {
        switch (tokenp->tokenCode) {
            case T_STAR:
            case T_DIV:
            case T_AND:  //乗法演算子
                link_parseLeaf(nodep, tokenp);
                return nodep;
                
            default:  //項終端
                return back_to_parent(nodep);
        }
    }
    
    if (nodep->child[2] == NULL) {  //因子
        return link_parseNode(nodep, P_FACTOR);
    }
    
    if (nodep->child[3] == NULL) {
        switch (tokenp->tokenCode) {
            case T_STAR:
            case T_DIV:
            case T_AND:  //乗法演算子->リスト延長
                return link_parseNode_as_list(nodep, P_TERM);
                
            default:  //項終端
                return back_to_parent(nodep);
        }
    }
    
    /* never reached */
    return back_to_parent(nodep);
}

parseNode * parse_factor(parseNode *nodep, tokenList *tokenp) {
    if (nodep->child[0] == NULL) {
        switch (tokenp->tokenCode) {
            case T_NAME:  //変数 A
                return link_parseNode(nodep, P_VAR);
                
            case T_NUMBER:
            case T_FALSE:
            case T_TRUE:
            case T_STRING:  //定数 B
                
            case T_LPAREN:  // ( 式 ) C
                
            case T_NOT:  // not 因子 D
                
            case T_INTEGER:
            case T_BOOLEAN:
            case T_CHAR:  // 標準型 ( 式 ) E
                link_parseLeaf(nodep, tokenp);
                return nodep;
                
            default:
                return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
        }
    }
    
    //C, D, Eなら、以下続く
    if (nodep->child[1] == NULL) {
        if (is_parseNode(nodep->child[0])) {  // A 因子の終端
            return back_to_parent(nodep);
        }else{
            // p_factor->child[0]はparse_leaf
            switch (nodep->child[0]->tokenp->tokenCode) {
                case T_LPAREN:  // C
                    return link_parseNode(nodep, P_EXP);
                    
                case T_NOT:  // D
                    return link_parseNode(nodep, P_FACTOR);
                    
                case T_INTEGER:
                case T_BOOLEAN:
                case T_CHAR:  // E
                    return check_and_link_parseLeaf(1, nodep, tokenp, T_LPAREN);
                    
                default:  // B 因子の終端
                    return back_to_parent(nodep);
            }
        }
    }
    
    if (nodep->child[2] == NULL) {
        switch (nodep->child[1]->structNum) {
            case P_EXP:  // C
                return check_and_link_parseLeaf(1, nodep, tokenp, T_RPAREN);
                
            case P_FACTOR:  // D
                // 因子の終端
                return back_to_parent(nodep);
                
            case P_TOKEN:  // E
                return link_parseNode(nodep, P_EXP);
                
            default:
                return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
        }
    }
    
    if (nodep->child[3] == NULL) {
        if (nodep->child[2]->structNum == P_EXP) {  // E
            return check_and_link_parseLeaf(1, nodep, tokenp, T_RPAREN);
        }else{  // C 因子の終端
            return back_to_parent(nodep);
        }
    }
    
    // E 因子の終端
    return back_to_parent(nodep);
}


parseNode * parse_output(parseNode *nodep, tokenList *tokenp) {
    if (nodep->child[0] == NULL) {
        if ((tokenp->tokenCode == T_STRING) && (tokenp->tokenLen != 1)) {  //文字列
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{  //式
            return link_parseNode(nodep, P_EXP);
        }
    }
    
    if (nodep->child[1] == NULL) {
        if (nodep->child[0]->structNum == P_TOKEN) {  //出力指定が文字列の場合 -> 出力指定終端
            return back_to_parent(nodep);
        }else{  //出力指定が式の場合
            switch (tokenp->tokenCode) {
                case T_RPAREN:
                case T_COMMA:  //符号なし整数指定なし -> 出力指定終端
                    return back_to_parent(nodep);
                    
                case T_COLON:  //符号なし整数指定あり
                    link_parseLeaf(nodep, tokenp);
                    return nodep;
                    
                default:
                    return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
            }
        }
    }
    
    //符号なし整数指定ありのみ
    if (nodep->child[2] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NUMBER);
    }
    
    //出力指定終端
    return back_to_parent(nodep);
}


parseNode * parse_statement(parseNode *nodep, tokenList *tokenp) {
    //先頭の子で下のキーワード出現なら文の開始
    //各文の解析はそれぞれの関数で行う。ここでは文の終端の判別は行わない。
    if (nodep->child[0] == NULL) {  //文の開始
        switch (tokenp->tokenCode) {
                //ここで値P_STATEMENTを持つノードが消滅する(各文の値に上書きされる)
            case T_NAME:  //代入文->変数のノードを接続
                nodep->structNum = P_ASSIGNMENT;
                return link_parseNode(nodep, P_VAR);
                
            case T_IF: //分岐文
                nodep->structNum = P_IF;
                break;
                
            case T_WHILE:  //繰り返し文
                nodep->structNum = P_WHILE;
                break;
                
            case T_CALL: //手続き呼び出し文
                nodep->structNum = P_CALL;
                break;
                
            case T_RETURN:   //戻り文
                nodep->structNum = P_RETURN;
                break;
                
            case T_READ:
            case T_READLN:  //入力文
                nodep->structNum = P_READ;
                break;
                
            case T_WRITE:
            case T_WRITELN:  //出力文
                nodep->structNum = P_WRITE;
                break;
                
            case T_BEGIN:  //複合文
                nodep->structNum = P_COMPOUND;
                break;
                
            default: //空文
                nodep->structNum = P_OBMIT;
                return back_to_parent(nodep);
        }
    
        link_parseLeaf(nodep, tokenp);
        return nodep;
    }
    
    /* ^^^ ONLY 1st Access ^^^ */
    
    /* never reached */
    return back_to_parent(nodep);
}

parseNode * parse_assignment(parseNode *nodep, tokenList *tokenp) {
    
    if (nodep->child[0] == NULL) {
        return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
    }
    
    if (nodep->child[1] == NULL) {
        return check_and_link_parseLeaf(1, nodep, tokenp, T_ASSIGN);
    }
    
    if (nodep->child[2] == NULL) {  //式
        return link_parseNode(nodep, P_EXP);
    }
    
    //代入文終端
    return back_to_parent(nodep);
}

parseNode * parse_if(parseNode *nodep, tokenList *tokenp) {
    if (nodep->child[0] == NULL) {
        return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
    }
    
    if (nodep->child[1] == NULL) {  //式
        return link_parseNode(nodep, P_EXP);
    }
    
    if (nodep->child[2] == NULL) {  //then
        return check_and_link_parseLeaf(1, nodep, tokenp, T_THEN);
    }
    
    if (nodep->child[3] == NULL) {  //文
        return link_parseNode(nodep, P_STATEMENT);
    }
    
    if (nodep->child[4] == NULL) {
        if (tokenp->tokenCode == T_ELSE) {  //else節あり
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{  //else節なし -> 分岐文終端
            return back_to_parent(nodep);
        }
    }
    
    //else節ありの場合のみ
    if (nodep->child[5] == NULL) {  //文
        return link_parseNode(nodep, P_STATEMENT);
    }
    
    //分岐文終端
    return back_to_parent(nodep);
}

parseNode * parse_while(parseNode *nodep, tokenList *tokenp) {
    if (nodep->child[0] == NULL) {
        return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
    }
    
    if (nodep->child[1] == NULL) {  //式
        return link_parseNode(nodep, P_EXP);
    }
    
    if (nodep->child[2] == NULL) {  //do
        return check_and_link_parseLeaf(1, nodep, tokenp, T_DO);
    }
    
    if (nodep->child[3] == NULL) {  //文
        return link_parseNode(nodep, P_STATEMENT);
    }
    
    //繰り返し文終端
    return back_to_parent(nodep);
}

parseNode * parse_call(parseNode *nodep, tokenList *tokenp) {
    if (nodep->child[0] == NULL) {
        return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
    }
    
    if (nodep->child[1] == NULL) {  //手続き名
        return check_and_link_parseLeaf(1, nodep, tokenp, T_NAME);
    }
    
    if (nodep->child[2] == NULL) {
        if (tokenp->tokenCode == T_LPAREN) {  //式の並びあり -> (
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{  //式の並び省略 -> 手続き呼び出し文終端
            return back_to_parent(nodep);
        }
    }
    
    //式の並びあり
    if (nodep->child[3] == NULL) {
        //式の並び
        return link_parseNode(nodep, P_EXP_LIST);
    }
    
    if (nodep->child[4] == NULL) {  // )
        return check_and_link_parseLeaf(1, nodep, tokenp, T_RPAREN);
    }
    
    //手続き呼び出し文終端
    return back_to_parent(nodep);
}

parseNode * parse_return(parseNode *nodep, tokenList *tokenp) {
    if (nodep->child[0] == NULL) {
        return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
    }
    
    return back_to_parent(nodep);
}

parseNode * parse_read(parseNode *nodep, tokenList *tokenp) {
    int empty;
    
    // ( read/readln ( 変数 , 変数 ->or) ) ( (省略) , 変数 , 変数 ->or) )
    
    empty = search_empty_child(nodep);
    
    //先頭はparse_statementで挿入済み -> リスト延長部の場合、省略
    if (empty == 0) {
        return link_parseObmit(nodep);
    }
    
    if (empty == 1) {
        if (!is_parse_list(nodep)) {  //リスト先頭ノードの場合
            if (tokenp->tokenCode == T_LPAREN) {  // 変数指定あり
                link_parseLeaf(nodep, tokenp);
                return nodep;
            }else{  //変数指定なし->入力文終端
                return back_to_parent(nodep);
            }
        }else{  //リスト延長部の場合
            return check_and_link_parseLeaf(1, nodep, tokenp, T_COMMA);
        }
    }
    
    if ((empty == 2) && (!is_parse_list(nodep))) {  //変数指定ありの場合は少なくとも1つは必要
        return link_parseNode(nodep, P_VAR);  //変数指定
    }
    
    if (empty == 3) {  //変数指定デリミタ or 変数指定終端記号
        return check_and_link_parseLeaf(2, nodep, tokenp, T_RPAREN, T_COMMA);
    }
    
    if (empty == 5) {
        if (tokenp->tokenCode == T_RPAREN) {  // 変数指定終端記号
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{
            if (tokenp->tokenCode == T_COMMA) {  //リスト延長
                return link_parseNode_as_list(nodep, P_READ);
            }else{
                return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
            }
        }
    }
    
    if ((empty % 2) == 0 && empty != 6) {  //変数指定 or 入力文終端 empty=2, 4
        if (nodep->child[empty-1]->tokenp->tokenCode == T_RPAREN) {  //入力文終端
            return back_to_parent(nodep);
        }else{
            return link_parseNode(nodep, P_VAR);  //変数指定
        }
    }
    
    //入力文終端
    return back_to_parent(nodep);
}

parseNode * parse_write(parseNode *nodep, tokenList *tokenp) {
    int empty;
    
    // ( write/writeln ( 出力指定 , 出力指定 ->or) ) ( (省略) , 出力指定 , 出力指定 ->or) )
    
    empty = search_empty_child(nodep);
    
    //先頭はparse_statementで挿入済み -> リスト延長部の場合、省略
    if (empty == 0) {
        return link_parseObmit(nodep);
    }
    
    if (empty == 1) {
        if (!is_parse_list(nodep)) {  //リスト先頭ノードの場合
            if (tokenp->tokenCode == T_LPAREN) {  // 出力指定あり
                link_parseLeaf(nodep, tokenp);
                return nodep;
            }else{  //出力指定なし->出力文終端
                return back_to_parent(nodep);
            }
        }else{  //リスト延長部の場合
            return check_and_link_parseLeaf(1, nodep, tokenp, T_COMMA);
        }
        
    }
    
    if ((empty == 2) && (!is_parse_list(nodep))) {  //出力指定ありの場合は少なくとも1つは必要
        return link_parseNode(nodep, P_OUTPUT);  //出力指定
    }
    
    if (empty == 3) {  //出力指定デリミタ or 出力指定終端記号
        return check_and_link_parseLeaf(2, nodep, tokenp, T_RPAREN, T_COMMA);
    }
    
    if (empty == 5) {
        if (tokenp->tokenCode == T_RPAREN) {  // 出力指定終端記号
            link_parseLeaf(nodep, tokenp);
            return nodep;
        }else{
            if (tokenp->tokenCode == T_COMMA) {  //リスト延長
                return link_parseNode_as_list(nodep, P_WRITE);
            }else{
                return error_parser(nodep, "Incorrect token: '%s' at line_%d", tokenp->tokenStr, tokenp->lineNum);
            }
        }
    }
    
    if ((empty % 2) == 0 && empty != 6) {  //出力指定 or 出力文終端 empty=2, 4
        if (nodep->child[empty-1]->tokenp->tokenCode == T_RPAREN) {  //出力文終端
            return back_to_parent(nodep);
        }else{
            return link_parseNode(nodep, P_OUTPUT);  //出力指定
        }
    }
    
    //出力文終端
    return back_to_parent(nodep);
}

////

void dump_parse_tree(struct PARSE_TREE *elementp) {
    int empty;
    int i;
    
    if (is_parseLeaf(elementp)) {
        for (i=0; i<elementp->depth; i++) {
            printf(".");
        }
        printf("%s\n", elementp->tokenp->tokenStr);
    }
    
    if (is_parseObmit(elementp)) {
        for (i=0; i<elementp->depth; i++) {
            printf(".");
        }
        printf("**O**\n");
    }
    
    if (is_parseNode(elementp)) {
        if (elementp->structNum == P_ROOT || elementp->structNum == P_ERROR) {
            dump_parse_tree(elementp->child[0]);
        }else{
            empty = search_empty_child(elementp);
            
            for (i=0; i<empty; i++) {
                dump_parse_tree(elementp->child[i]);
            }
        }
    }
    
    return;
}