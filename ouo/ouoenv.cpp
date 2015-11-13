//
//  ouoenv.cpp
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#include "ouoenv.h"
#include <stdlib.h>
#include <string.h>

ouoenv * ouoenv_new(void) {
    ouoenv * e = (ouoenv *)malloc(sizeof(ouoenv));
    e->par = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void ouoenv_def(ouoenv * e, ouoval * k, ouoval * v) {
    /* Iterate till e has no parent */
    while (e->par) { e = e->par; }
    /* Put value in e */
    ouoenv_put(e, k, v);
}

void ouoenv_add_builtin(ouoenv * e, const char* name, ouobuiltin func) {
    ouoval * k = ouoval_sym(name);
    ouoval * v = ouoval_builtin(func);
    ouoenv_put(e, k, v);
    ouoval_del(k); ouoval_del(v);
}

void ouoenv_put(ouoenv * e, ouoval * k, ouoval * v) {
    
    /* Iterate over all items in environment */
    /* This is to see if variable already exists */
    for (int i = 0; i < e->count; i++) {
        
        /* If variable is found delete item at that position */
        /* And replace with variable supplied by user */
        if (strcmp(e->syms[i], k->sym) == 0) {
            ouoval_del(e->vals[i]);
            e->vals[i] = ouoval_copy(v);
            return;
        }
    }
    
    /* If no existing entry found allocate space for new entry */
    e->count++;
    e->vals = (ouoval **)realloc(e->vals, sizeof(ouoval *) * e->count);
    e->syms = (char **)realloc(e->syms, sizeof(char *) * e->count);
    
    /* Copy contents of lval and symbol string into new location */
    e->vals[e->count-1] = ouoval_copy(v);
    e->syms[e->count-1] = (char *)malloc(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1], k->sym);
}

ouoval * ouoenv_get(ouoenv * e, ouoval * k) {
    
    /* Iterate over all items in environment */
    for (int i = 0; i < e->count; i++) {
        /* Check if the stored string matches the symbol string */
        /* If it does, return a copy of the value */
        if (strcmp(e->syms[i], k->sym) == 0) {
            return ouoval_copy(e->vals[i]);
        }
    }
    /* If no symbol check in parent otherwise error */
    if (e->par) {
        return ouoenv_get(e->par, k);
    }
    return ouoval_err("unbound symbol!");
}

ouoenv * ouoenv_copy(ouoenv * e) {
    ouoenv * n = (ouoenv *)malloc(sizeof(ouoenv));
    n->par = e->par;
    n->count = e->count;
    n->syms = (char **)malloc(sizeof(char *) * n->count);
    n->vals = (ouoval **)malloc(sizeof(ouoval *) * n->count);
    for (int i = 0; i < e->count; i++) {
        n->syms[i] = (char *)malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = ouoval_copy(e->vals[i]);
    }
    return n;
}

void ouoenv_del(ouoenv * e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        ouoval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}
