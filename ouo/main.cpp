//
//  main.cpp
//  OuO
//
//  Created by BlueCocoa on 15/11/11.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#include <iostream>
#include <string>
#include <editline/readline.h>
#include <errno.h>
#include "mpc.h"
#include "ouoval.h"
#include "ouoenv.h"
#include "ouobuiltin.h"

using namespace std;

#define ouo_prompt "OuO> "

mpc_parser_t * number;
mpc_parser_t * symbol;
mpc_parser_t * strings;
mpc_parser_t * comment;
mpc_parser_t * sexpr;
mpc_parser_t * qexpr;
mpc_parser_t * expression;
mpc_parser_t * ouo;

ouoval * builtin_exit(ouoenv *e, ouoval * a) {
    printf("（/>///<）/~~~~~~~~~~~~~~~~~~╧═╧ )>口<) \n");
    exit(0);
    return NULL;
}

void ouoenv_add_builtins(ouoenv * e) {
    ouoenv_add_builtin(e, "exit", builtin_exit);
    
    /* String Functions */
    ouoenv_add_builtin(e, "load",  builtin_load);
    ouoenv_add_builtin(e, "error", builtin_error);
    ouoenv_add_builtin(e, "print", builtin_print);
    ouoenv_add_builtin(e, "println", builtin_println);
    
    /* Variable Functions */
    ouoenv_add_builtin(e, "\\", builtin_lambda);
    ouoenv_add_builtin(e, "def", builtin_def);
    ouoenv_add_builtin(e, "=",   builtin_put);
    /* new variable functions
     */
    //ouoenv_add_builtin(e, "new", builtin_new);
    
    ouoenv_add_builtin(e, "if", builtin_if);
    ouoenv_add_builtin(e, "==", builtin_eq);
    ouoenv_add_builtin(e, "!=", builtin_ne);
    ouoenv_add_builtin(e, ">",  builtin_gt);
    ouoenv_add_builtin(e, "<",  builtin_lt);
    ouoenv_add_builtin(e, ">=", builtin_ge);
    ouoenv_add_builtin(e, "<=", builtin_le);
    
    /* List Functions */
    ouoenv_add_builtin(e, "list", builtin_list);
    ouoenv_add_builtin(e, "head", builtin_head);
    ouoenv_add_builtin(e, "tail", builtin_tail);
    ouoenv_add_builtin(e, "eval", builtin_eval);
    ouoenv_add_builtin(e, "join", builtin_join);
    
    
    /* Mathematical Functions */
    ouoenv_add_builtin(e, "+", builtin_add);
    ouoenv_add_builtin(e, "-", builtin_sub);
    ouoenv_add_builtin(e, "*", builtin_mul);
    ouoenv_add_builtin(e, "/", builtin_div);
}

int main(int argc, const char * argv[]) {
    number   = mpc_new("number");
    symbol = mpc_new("symbol");
    strings = mpc_new("string");
    comment = mpc_new("comment");
    sexpr = mpc_new("sexpr");
    qexpr = mpc_new("qexpr");
    expression = mpc_new("expression");
    ouo = mpc_new("ouo");
    
    mpca_lang(MPCA_LANG_DEFAULT,
              "                                                            \
              number     : /-?[0-9]+\\.?[0-9]*/ ;                          \
              symbol     : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%^]+/ ;              \
              string     : /\"(\\\\.|[^\"])*\"/ ;                          \
              comment    : /;[^\\r\\n]*/ ;                                 \
              sexpr      : '[' <expression>* ']' ;                         \
              qexpr      : '{' <expression>* '}' ;                         \
              expression : <number> | <symbol> | <string>                  \
                         | <comment> | <sexpr> | <qexpr> ;                 \
              ouo        : /^/ <expression>* /$/ ;                         \
              ",
              number, symbol, strings, comment, sexpr, qexpr, expression, ouo);
    
    ouoenv * e = ouoenv_new();
    ouoenv_add_builtins(e);
    
    /* Supplied with list of files */
    if (argc >= 2) {
        /* loop over each supplied filename (starting from 1) */
        for (int i = 1; i < argc; i++) {
            
            /* Argument list with a single argument, the filename */
            ouoval * args = ouoval_add(ouoval_sexpr(), ouoval_str(argv[i]));
            
            /* Pass to builtin load and get the result */
            ouoval * x = builtin_load(e, args);
            
            /* If the result is an error be sure to print it */
            if (x->type == OuOVAL_ERR) { ouoval_println(x); }
            ouoval_del(x);
        }
    } else {
        fprintf(stderr, "OuO Programming Language v0.0.1\nBuilt at %s compiled with gcc version %s\n", __DATE__, __VERSION__);
        while (1) {
            char* input = readline(ouo_prompt);
            mpc_result_t r;
            if (mpc_parse("<stdin>", input, ouo, &r)) {
                /* On Success Print the AST */
                ouoval * x = ouoval_eval(e, ouoval_read((mpc_ast_t *)r.output));
                ouoval_println(x);
                ouoval_del(x);
                mpc_ast_delete((mpc_ast_t *)r.output);
            } else {
                /* Otherwise Print the Error */
                mpc_err_print(r.error);
                mpc_err_delete(r.error);
            }
            free(input);
        }
    }
    
    ouoenv_del(e);
    mpc_cleanup(8, number, symbol, strings, comment, sexpr, qexpr, expression, ouo);
    return 0;
}
