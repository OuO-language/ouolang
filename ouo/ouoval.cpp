//
//  ouoval.cpp
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#include "ouoval.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <cmath>
#include "mpc.h"

const char * ouotype_name(int t) {
    switch(t) {
        case OuOVAL_FUNC: return "Function";
        case OuOVAL_NUM: return "Number";
        case OuOVAL_VALUE: return "MP Value";
        case OuOVAL_ERR: return "Error";
        case OuOVAL_SYM: return "Symbol";
        case OuOVAL_STR: return "String";
        case OuOVAL_SEXPR: return "S-Expression";
        case OuOVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}

ouoval * ouoval_err(const char * fmt, ...) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_ERR;
    
    /* Create a va list and initialize it */
    va_list va;
    va_start(va, fmt);
    
    /* Allocate 512 bytes of space */
    v->err = (char *)malloc(512);
    
    /* printf the error string with a maximum of 511 characters */
    vsnprintf(v->err, 511, fmt, va);
    
    /* Reallocate to number of bytes actually used */
    v->err = (char *)realloc(v->err, strlen(v->err)+1);
    
    /* Cleanup our va list */
    va_end(va);
    
    return v;
}

ouoval * ouoval_num(double x) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_NUM;
    v->num = x;
    return v;
}

ouoval * ouoval_sym(const char * s) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_SYM;
    v->sym = (char *)malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

ouoval * ouoval_str(const char * s) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_STR;
    v->str = (char *)malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

ouoval * ouoval_sexpr(void) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

ouoval * ouoval_qexpr(void) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

ouoval * ouoval_read(mpc_ast_t* t) {
    
    /* If Symbol or Number return conversion to that type */
    if (strstr(t->tag, "number")) { return ouoval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return ouoval_sym(t->contents); }
    if (strstr(t->tag, "string")) { return ouoval_read_str(t); }
    
    /* If root (>) or sexpr then create empty list */
    ouoval * x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = ouoval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = ouoval_sexpr(); }
    if (strstr(t->tag, "qexpr"))  { x = ouoval_qexpr(); }
    
    /* Fill this list with any valid expression contained within */
    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "[") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "]") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }
        x = ouoval_add(x, ouoval_read(t->children[i]));
    }
    
    return x;
}

ouoval * ouoval_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ?
    ouoval_num(x) : ouoval_err("invalid number");
}

ouoval * ouoval_read_str(mpc_ast_t* t) {
    /* Cut off the final quote character */
    t->contents[strlen(t->contents)-1] = '\0';
    /* Copy the string missing out the first quote character */
    char* unescaped = (char *)malloc(strlen(t->contents+1)+1);
    strcpy(unescaped, t->contents+1);
    
    /* Construct a new lval using the string */
    ouoval* str = ouoval_str(unescaped);
    /* Free the string and return */
    free(unescaped);
    return str;
}

ouoval * ouoval_lambda(ouoval * formals, ouoval * body) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_FUNC;
    
    /* Set Builtin to Null */
    v->builtin = NULL;
    
    /* Build new environment */
    v->env = ouoenv_new();
    
    /* Set Formals and Body */
    v->formals = formals;
    v->body = body;
    return v;
}

ouoval * ouoval_builtin(ouobuiltin func) {
    ouoval * v = (ouoval *)malloc(sizeof(ouoval));
    v->type = OuOVAL_FUNC;
    v->builtin = func;
    return v;
}

ouoval * ouoval_add(ouoval * v, ouoval * x) {
    v->count++;
    v->cell = (struct ouoval **)realloc(v->cell, sizeof(ouoval *) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

void ouoval_print(ouoval * v) {
    switch (v->type) {
            /* In the case the type is a number print it */
            /* Then 'break' out of the switch. */
        case OuOVAL_NUM: printf("%g", v->num); break;
        case OuOVAL_VALUE: {
            printf("%s", v->value);
            break;
        }
        case OuOVAL_FUNC:
            if (v->builtin) {
                printf("<function>");
            } else {
                printf("[\\ "); ouoval_print(v->formals);
                putchar(' '); ouoval_print(v->body); putchar(']');
            }
            break;
            
            /* In the case the type is an error */
        case OuOVAL_ERR:
            /* Check what type of error it is and print it */
            printf("Error: %s", v->err); break;
        case OuOVAL_SYM:   printf("%s", v->sym); break;
        case OuOVAL_STR:   ouoval_print_str(v); break;
        case OuOVAL_SEXPR: ouoval_expr_print(v, '[', ']'); break;
        case OuOVAL_QEXPR: ouoval_expr_print(v, '{', '}'); break;
    }
}

void ouoval_println(ouoval * v) {
    ouoval_print(v);
    putchar('\n');
}

void ouoval_print_str(ouoval * v) {
    /* Make a Copy of the string */
    char * escaped = (char *)malloc(strlen(v->str)+1);
    strcpy(escaped, v->str);
    /* Pass it through the unescape function */
    escaped = (char *)mpcf_unescape(escaped);
    /* Print it between " characters */
    printf("%s", escaped);
    /* free the copied string */
    free(escaped);
}

void ouoval_expr_print(ouoval * v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        
        /* Print Value contained within */
        ouoval_print(v->cell[i]);
        
        /* Don't print trailing space if last element */
        if (i != (v->count-1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

ouoval * ouoval_eval(ouoenv * e, ouoval * v) {
    if (v->type == OuOVAL_SYM) {
        ouoval * x = ouoenv_get(e, v);
        ouoval_del(v);
        return x;
    }
    /* Evaluate Sexpressions */
    if (v->type == OuOVAL_SEXPR) { return ouoval_eval_sexpr(e, v); }
    /* All other lval types remain the same */
    return v;
}

ouoval * ouoval_eval_sexpr(ouoenv * e, ouoval * v) {
    
    /* Evaluate Children */
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = ouoval_eval(e, v->cell[i]);
    }
    
    /* Error Checking */
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == OuOVAL_ERR) { return ouoval_take(v, i); }
    }
    
    /* Empty Expression */
    if (v->count == 0) { return v; }
    
    /* Single Expression */
    if (v->count == 1) { return ouoval_take(v, 0); }
    
    /* Ensure First Element is Symbol */
    ouoval * f = ouoval_pop(v, 0);
    if (f->type != OuOVAL_FUNC) {
        ouoval * err = ouoval_err(
                                  "S-Expression starts with incorrect type. "
                                  "Got %s, Expected %s.",
                                  ouotype_name(f->type), ouotype_name(OuOVAL_FUNC));
        ouoval_del(f); ouoval_del(v);
        return err;
    }
    
    /* Call builtin with operator */
    ouoval * result = ouoval_call(e, f, v);
    ouoval_del(f);
    return result;
}

ouoval * ouoval_copy(ouoval* v) {
    
    ouoval * x = (ouoval *)malloc(sizeof(ouoval));
    x->type = v->type;
    
    switch (v->type) {
            
            /* Copy Functions and Numbers Directly */
        case OuOVAL_FUNC:
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = ouoenv_copy(v->env);
                x->formals = ouoval_copy(v->formals);
                x->body = ouoval_copy(v->body);
            }
            break;
        case OuOVAL_NUM: x->num = v->num; break;
            
            /* Copy Strings using malloc and strcpy */
        case OuOVAL_ERR:
            x->err = (char *)malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err); break;
            
        case OuOVAL_SYM:
            x->sym = (char *)malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym); break;
        case OuOVAL_STR:
            x->str = (char *)malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str); break;
        case OuOVAL_VALUE:
            x->value = (char *)malloc(strlen(v->value) + 1);
            strcpy(x->value, v->value); break;
            
            /* Copy Lists by copying each sub-expression */
        case OuOVAL_SEXPR:
        case OuOVAL_QEXPR:
            x->count = v->count;
            x->cell = (ouoval **)malloc(sizeof(ouoval *) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = ouoval_copy(v->cell[i]);
            }
            break;
    }
    
    return x;
}

ouoval * ouoval_pop(ouoval * v, int i) {
    /* Find the item at "i" */
    ouoval * x = v->cell[i];
    
    /* Shift memory after the item at "i" over the top */
    memmove(&v->cell[i], &v->cell[i+1],
            sizeof(ouoval *) * (v->count-i-1));
    
    /* Decrease the count of items in the list */
    v->count--;
    
    /* Reallocate the memory used */
    v->cell = (ouoval **)realloc(v->cell, sizeof(ouoval *) * v->count);
    return x;
}

ouoval * ouoval_take(ouoval * v, int i) {
    ouoval * x = ouoval_pop(v, i);
    ouoval_del(v);
    return x;
}

ouoval * ouoval_join(ouoval * x, ouoval * y) {
    
    /* For each cell in 'y' add it to 'x' */
    while (y->count) {
        x = ouoval_add(x, ouoval_pop(y, 0));
    }
    
    /* Delete the empty 'y' and return 'x' */
    ouoval_del(y);
    return x;
}

ouoval * ouoval_call(ouoenv* e, ouoval * f, ouoval * a) {
    
    /* If Builtin then simply apply that */
    if (f->builtin) { return f->builtin(e, a); }
    
    /* Record Argument Counts */
    int given = a->count;
    int total = f->formals->count;
    
    /* While arguments still remain to be processed */
    while (a->count) {
        
        /* If we've ran out of formal arguments to bind */
        if (f->formals->count == 0) {
            ouoval_del(a); return ouoval_err(
                                             "Function passed too many arguments. "
                                             "Got %i, Expected %i.", given, total);
        }
        
        /* Pop the first symbol from the formals */
        ouoval * sym = ouoval_pop(f->formals, 0);
        
        /* Special Case to deal with '&' */
        if (strcmp(sym->sym, "&") == 0) {
            
            /* Ensure '&' is followed by another symbol */
            if (f->formals->count != 1) {
                ouoval_del(a);
                return ouoval_err("Function format invalid. "
                                  "Symbol '&' not followed by single symbol.");
            }
            
            /* Next formal should be bound to remaining arguments */
            ouoval * nsym = ouoval_pop(f->formals, 0);
            ouoenv_put(f->env, nsym, builtin_list(e, a));
            ouoval_del(sym); ouoval_del(nsym);
            break;
        }
        
        /* Pop the next argument from the list */
        ouoval * val = ouoval_pop(a, 0);
        
        /* Bind a copy into the function's environment */
        ouoenv_put(f->env, sym, val);
        
        /* Delete symbol and value */
        ouoval_del(sym); ouoval_del(val);
    }
    
    /* Argument list is now bound so can be cleaned up */
    ouoval_del(a);
    
    /* If '&' remains in formal list bind to empty list */
    if (f->formals->count > 0 &&
        strcmp(f->formals->cell[0]->sym, "&") == 0) {
        
        /* Check to ensure that & is not passed invalidly. */
        if (f->formals->count != 2) {
            return ouoval_err("Function format invalid. "
                              "Symbol '&' not followed by single symbol.");
        }
        
        /* Pop and delete '&' symbol */
        ouoval_del(ouoval_pop(f->formals, 0));
        
        /* Pop next symbol and create empty list */
        ouoval * sym = ouoval_pop(f->formals, 0);
        ouoval * val = ouoval_qexpr();
        
        /* Bind to environment and delete */
        ouoenv_put(f->env, sym, val);
        ouoval_del(sym); ouoval_del(val);
    }
    
    /* If all formals have been bound evaluate */
    if (f->formals->count == 0) {
        
        /* Set environment parent to evaluation environment */
        f->env->par = e;
        
        /* Evaluate and return */
        return builtin_eval(f->env, ouoval_add(ouoval_sexpr(), ouoval_copy(f->body)));
    } else {
        /* Otherwise return partially evaluated function */
        return ouoval_copy(f);
    }
}

void ouoval_del(ouoval * v) {
    
    switch (v->type) {
            /* Do nothing special for number type */
        case OuOVAL_NUM: break;
        case OuOVAL_FUNC:
            if (!v->builtin) {
                ouoenv_del(v->env);
                ouoval_del(v->formals);
                ouoval_del(v->body);
            }
            break;
            
            /* For Err or Sym free the string data */
        case OuOVAL_ERR: free(v->err); break;
        case OuOVAL_SYM: free(v->sym); break;
        case OuOVAL_STR: free(v->str); break;
        case OuOVAL_VALUE: free(v->value); break;
            
            /* If Sexpr|Qexpr then delete all elements inside */
        case OuOVAL_QEXPR:
        case OuOVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                ouoval_del(v->cell[i]);
            }
            /* Also free the memory allocated to contain the pointers */
            free(v->cell);
            break;
    }
    
    /* Free the memory allocated for the "lval" struct itself */
    free(v);
}

int ouoval_eq(ouoval * x, ouoval * y) {
    
    /* Different Types are always unequal */
    if (x->type != y->type) { return 0; }
    
    /* Compare Based upon type */
    switch (x->type) {
            /* Compare Number Value */
        case OuOVAL_NUM: return (x->num == y->num);
            
            /* Compare String Values */
        case OuOVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case OuOVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
        case OuOVAL_STR: return (strcmp(x->str, y->str) == 0);
            
            /* Compare GMP Values */
        case OuOVAL_VALUE: {
            mpz_t xv, yv;
            mpz_init_set_str(xv, x->value, 10);
            mpz_init_set_str(yv, y->value, 10);
            int result = mpz_cmp(xv, yv);
            mpz_clears(xv, yv);
            return result;
        }
            
            /* If builtin compare, otherwise compare formals and body */
        case OuOVAL_FUNC:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return ouoval_eq(x->formals, y->formals)
                && ouoval_eq(x->body, y->body);
            }
            
            /* If list compare every individual element */
        case OuOVAL_QEXPR:
        case OuOVAL_SEXPR:
            if (x->count != y->count) { return 0; }
            for (int i = 0; i < x->count; i++) {
                /* If any element not equal then whole list not equal */
                if (!ouoval_eq(x->cell[i], y->cell[i])) { return 0; }
            }
            /* Otherwise lists must be equal */
            return 1;
            break;
    }
    return 0;
}
