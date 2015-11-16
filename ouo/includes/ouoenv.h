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
    /**
     *  @brief  Parent env
     */
    ouoenv * par;
    
    /**
     *  @brief  Variables count
     */
    int count;
    
    /**
     *  @brief  Variable symbols
     */
    char ** syms;
    
    /**
     *  @brief  Variables
     */
    ouoval ** vals;
} ouoenv;

/**
 *  @brief  Construct a pointer to a new ouoenv
 *
 *  @return ouoenv
 */
ouoenv * ouoenv_new(void);

/**
 *  @brief  Define a variable in e
 *
 *  @discussion  This variable would be defined at the root node of e
 *
 *  @return ouoenv
 */
void ouoenv_def(ouoenv * e, ouoval * k, ouoval * v);

/**
 *  @brief  Add a builtin function for a variable
 *
 *  @param e    env
 *  @param name symbol
 *  @param func builtin function
 */
void ouoenv_add_builtin(ouoenv * e, const char* name, ouobuiltin func);

/**
 *  @brief  Put a variable in e
 *
 *  @param e env
 *  @param k symbol anme
 *  @param v variable
 */
void ouoenv_put(ouoenv * e, ouoval * k, ouoval * v);

/**
 *  @brief  Get a variable in e
 *
 *  @discussion  If this symbol isn't in this env, we'll check the parent node of the given env
 *
 *  @param e env
 *  @param k symbol name
 *
 *  @return A copy of that variable
 */
ouoval * ouoenv_get(ouoenv * e, ouoval * k);

/**
 *  @brief  Create a copy of the given env
 *
 *  @param e env
 *
 *  @return A copy of the given env
 */
ouoenv * ouoenv_copy(ouoenv * e);

/**
 *  @brief  Delete the env
 *
 *  @param e env
 */
void ouoenv_del(ouoenv * e);

#endif /* OUOENV_H */
