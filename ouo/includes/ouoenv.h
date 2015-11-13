//
//  ouoenv.hpp
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#ifndef OUOENV_H
#define OUOENV_H

#include <stdio.h>
#include "ouolang.h"

/**
 *  @brief  Forward Declarations
 */
struct ouoval;
struct ouoenv;
typedef ouoval * (*ouobuiltin)(ouoenv *, ouoval *);

typedef struct ouoenv {
    ouoenv * par;
    int count;
    char ** syms;
    ouoval ** vals;
} ouoenv;

ouoenv * ouoenv_new(void);

void ouoenv_def(ouoenv * e, ouoval * k, ouoval * v);

void ouoenv_add_builtin(ouoenv * e, const char* name, ouobuiltin func);

void ouoenv_put(ouoenv * e, ouoval * k, ouoval * v);

ouoval * ouoenv_get(ouoenv * e, ouoval * k);

ouoenv * ouoenv_copy(ouoenv * e);

void ouoenv_del(ouoenv * e);

#endif /* OUOENV_H */
