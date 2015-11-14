//
//  ouobuiltin.cpp
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#include "ouobuiltin.h"
#include <stdlib.h>
#include <string.h>
#include "mpc.h"

extern mpc_parser_t * ouo;

ouoval * builtin_var(ouoenv * e, ouoval * a, const char * func) {
    OuOASSERT_TYPE(func, a, 0, OuOVAL_QEXPR);
    
    ouoval * syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        OuOASSERT(a, (syms->cell[i]->type == OuOVAL_SYM),
                  "Function '%s' cannot define non-symbol. "
                  "Got %s, Expected %s.", func,
                  ouotype_name(syms->cell[i]->type),
                  ouotype_name(OuOVAL_SYM));
    }
    
    OuOASSERT(a, (syms->count == a->count-1),
              "Function '%s' passed too many arguments for symbols. "
              "Got %i, Expected %i.", func, syms->count, a->count-1);
    
    for (int i = 0; i < syms->count; i++) {
        /* If 'def' define in globally. If 'put' define in locally */
        if (strcmp(func, "def") == 0) {
            ouoenv_def(e, syms->cell[i], a->cell[i+1]);
        }
        
        if (strcmp(func, "=")   == 0) {
            ouoenv_put(e, syms->cell[i], a->cell[i+1]);
        }
    }
    
    ouoval_del(a);
    return ouoval_sexpr();
}

ouoval * builtin_load(ouoenv * e, ouoval * a) {
    OuOASSERT_NUM("load", a, 1);
    OuOASSERT_TYPE("load", a, 0, OuOVAL_STR);
    
    /* Parse File given by string name */
    mpc_result_t r;
    if (mpc_parse_contents(a->cell[0]->str, ouo, &r)) {
        
        /* Read contents */
        ouoval * expr = ouoval_read((mpc_ast_t *)r.output);
        mpc_ast_delete((mpc_ast_t *)r.output);
        
        /* Evaluate each Expression */
        while (expr->count) {
            ouoval * x = ouoval_eval(e, ouoval_pop(expr, 0));
            /* If Evaluation leads to error print it */
            if (x->type == OuOVAL_ERR) { ouoval_println(x); }
            ouoval_del(x);
        }
        
        /* Delete expressions and arguments */
        ouoval_del(expr);
        ouoval_del(a);
        
        /* Return empty list */
        return ouoval_sexpr();
        
    } else {
        /* Get Parse Error as String */
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        
        /* Create new error message using it */
        ouoval * err = ouoval_err("Could not load Library %s", err_msg);
        free(err_msg);
        ouoval_del(a);
        
        /* Cleanup and return error */
        return err;
    }
}

ouoval * builtin_error(ouoenv * e, ouoval * a) {
    OuOASSERT_NUM("error", a, 1);
    OuOASSERT_TYPE("error", a, 0, OuOVAL_STR);
    
    /* Construct Error from first argument */
    ouoval* err = ouoval_err(a->cell[0]->str);
    
    /* Delete arguments and return */
    ouoval_del(a);
    return err;
}

ouoval * builtin_print(ouoenv * e, ouoval * a) {
    
    /* Print each argument followed by a space */
    int i;
    for (i = 0; i < a->count - 1; i++) {
        ouoval_print(a->cell[i]); putchar(' ');
    }
    i++;
    ouoval_print(a->cell[i]);
    
    /* Print a newline and delete arguments */
    ouoval_del(a);
    
    return ouoval_sexpr();
}

ouoval * builtin_println(ouoenv * e, ouoval * a) {
    ouoval * r = builtin_print(e, a);
    putchar('\n');
    return r;
}

ouoval * builtin_lambda(ouoenv * e, ouoval * a) {
    /* Check Two arguments, each of which are Q-Expressions */
    OuOASSERT_NUM("\\", a, 2);
    OuOASSERT_TYPE("\\", a, 0, OuOVAL_QEXPR);
    OuOASSERT_TYPE("\\", a, 1, OuOVAL_QEXPR);
    
    /* Check first Q-Expression contains only Symbols */
    for (int i = 0; i < a->cell[0]->count; i++) {
        OuOASSERT(a, (a->cell[0]->cell[i]->type == OuOVAL_SYM),
                  "Cannot define non-symbol. Got %s, Expected %s.",
                  ouotype_name(a->cell[0]->cell[i]->type),ouotype_name(OuOVAL_SYM));
    }
    
    /* Pop first two arguments and pass them to lval_lambda */
    ouoval * formals = ouoval_pop(a, 0);
    ouoval * body = ouoval_pop(a, 0);
    ouoval_del(a);
    
    return ouoval_lambda(formals, body);
}

ouoval * builtin_def(ouoenv * e, ouoval * a) {
    return builtin_var(e, a, "def");
}
ouoval * builtin_put(ouoenv * e, ouoval * a) {
    return builtin_var(e, a, "=");
}

ouoval * builtin_if(ouoenv * e, ouoval* a) {
    OuOASSERT_NUM("if", a, 3);
    OuOASSERT_TYPE("if", a, 0, OuOVAL_NUM);
    OuOASSERT_TYPE("if", a, 1, OuOVAL_QEXPR);
    OuOASSERT_TYPE("if", a, 2, OuOVAL_QEXPR);
    
    /* Mark Both Expressions as evaluable */
    ouoval * x;
    a->cell[1]->type = OuOVAL_SEXPR;
    a->cell[2]->type = OuOVAL_SEXPR;
    
    if (a->cell[0]->num) {
        /* If condition is true evaluate first expression */
        x = ouoval_eval(e, ouoval_pop(a, 1));
    } else {
        /* Otherwise evaluate second expression */
        x = ouoval_eval(e, ouoval_pop(a, 2));
    }
    
    /* Delete argument list and return */
    ouoval_del(a);
    return x;
}

ouoval * builtin_eq(ouoenv * e, ouoval * a) {
    return builtin_cmp(e, a, "==");
}

ouoval * builtin_ne(ouoenv * e, ouoval * a) {
    return builtin_cmp(e, a, "!=");
}

ouoval * builtin_gt(ouoenv * e, ouoval * a) {
    return builtin_ord(e, a, ">");
}

ouoval * builtin_lt(ouoenv * e, ouoval * a) {
    return builtin_ord(e, a, "<");
}

ouoval * builtin_ge(ouoenv * e, ouoval * a) {
    return builtin_ord(e, a, ">=");
}

ouoval * builtin_le(ouoenv * e, ouoval * a) {
    return builtin_ord(e, a, "<=");
}

ouoval * builtin_cmp(ouoenv * e, ouoval * a, const char * op) {
    OuOASSERT_NUM(op, a, 2);
    
    int r = 0;
    if (strcmp(op, "==") == 0) {
        r =  ouoval_eq(a->cell[0], a->cell[1]);
    }
    if (strcmp(op, "!=") == 0) {
        r = !ouoval_eq(a->cell[0], a->cell[1]);
    }
    ouoval_del(a);
    return ouoval_num(r);
}

ouoval * builtin_ord(ouoenv * e, ouoval * a, const char* op) {
    OuOASSERT_NUM(op, a, 2);
    OuOASSERT_TYPE(op, a, 0, OuOVAL_NUM);
    OuOASSERT_TYPE(op, a, 1, OuOVAL_NUM);
    
    int r = 0;
    if (strcmp(op, ">")  == 0) {
        r = (a->cell[0]->num >  a->cell[1]->num);
    }
    if (strcmp(op, "<")  == 0) {
        r = (a->cell[0]->num <  a->cell[1]->num);
    }
    if (strcmp(op, ">=") == 0) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    }
    if (strcmp(op, "<=") == 0) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    }
    ouoval_del(a);
    return ouoval_num(r);
}

ouoval * builtin_op(ouoenv * e, ouoval * a, const char* op) {
    
    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        OuOASSERT_TYPE(op, a, i, OuOVAL_NUM);
    }
    
    /* Pop the first element */
    ouoval * x = ouoval_pop(a, 0);
    
    /* If no arguments and sub then perform unary negation */
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }
    
    /* While there are still elements remaining */
    while (a->count > 0) {
        
        /* Pop the next element */
        ouoval * y = ouoval_pop(a, 0);
        
        /* Perform operation */
        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                ouoval_del(x); ouoval_del(y);
                x = ouoval_err("Division By Zero.");
                break;
            }
            x->num /= y->num;
        }
        if (strcmp(op, "^") == 0) {
            x->num = pow(x->num, y->num);
        }
        
        /* Delete element now finished with */
        ouoval_del(y);
    }
    
    /* Delete input expression and return result */
    ouoval_del(a);
    return x;
}

ouoval * builtin_list(ouoenv * e, ouoval * a) {
    a->type = OuOVAL_QEXPR;
    return a;
}

ouoval * builtin_head(ouoenv * e, ouoval * a) {
    /* Check Error Conditions */
    OuOASSERT_NUM("head", a, 1);
    OuOASSERT_TYPE("head", a, 0, OuOVAL_QEXPR);
    OuOASSERT_NOT_EMPTY("head", a, 0);
    
    /* Otherwise take first argument */
    ouoval * v = ouoval_take(a, 0);
    
    /* Delete all elements that are not head and return */
    while (v->count > 1) { ouoval_del(ouoval_pop(v, 1)); }
    return v;
}

ouoval * builtin_tail(ouoenv * e, ouoval * a) {
    /* Check Error Conditions */
    OuOASSERT(a, a->count == 1,
              "Function 'tail' passed too many arguments!");
    OuOASSERT(a, a->cell[0]->type == OuOVAL_QEXPR,
              "Function 'tail' passed incorrect type!");
    OuOASSERT(a, a->cell[0]->count != 0,
              "Function 'tail' passed {}!");
    
    /* Take first argument */
    ouoval * v = ouoval_take(a, 0);
    
    /* Delete first element and return */
    ouoval_del(ouoval_pop(v, 0));
    return v;
}

ouoval * builtin_eval(ouoenv * e, ouoval * a) {
    OuOASSERT(a, a->count == 1,
              "Function 'eval' passed too many arguments!");
    OuOASSERT(a, a->cell[0]->type == OuOVAL_QEXPR,
              "Function 'eval' passed incorrect type!");
    
    ouoval * x = ouoval_take(a, 0);
    x->type = OuOVAL_SEXPR;
    return ouoval_eval(e, x);
}

ouoval * builtin_join(ouoenv *e, ouoval * a) {
    
    for (int i = 0; i < a->count; i++) {
        OuOASSERT_TYPE("join", a, i, OuOVAL_QEXPR);
    }
    
    ouoval* x = ouoval_pop(a, 0);
    
    while (a->count) {
        ouoval * y = ouoval_pop(a, 0);
        x = ouoval_join(x, y);
    }
    
    ouoval_del(a);
    return x;
}

ouoval * builtin_add(ouoenv * e, ouoval * a) {
    return builtin_op(e, a, "+");
}

ouoval * builtin_sub(ouoenv * e, ouoval * a) {
    return builtin_op(e, a, "-");
}

ouoval * builtin_mul(ouoenv * e, ouoval * a) {
    return builtin_op(e, a, "*");
}

ouoval * builtin_div(ouoenv * e, ouoval * a) {
    return builtin_op(e, a, "/");
}

ouoval * builtin_power(ouoenv * e, ouoval * a) {
    return builtin_op(e, a, "^");
}
