//
//  ouobuiltin.h
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#ifndef OUOBUILTIN_H
#define OUOBUILTIN_H

#include <stdio.h>
#include "ouolang.h"
#define MAX_HISTORY_COUNT 233

/**
 *  @brief  Forward Declarations
 */
struct ouoval;
struct ouoenv;
typedef ouoval * (*ouobuiltin)(ouoenv *, ouoval *);

/**
 *  @brief  <#Description#>
 *
 *  @param e    <#e description#>
 *  @param a    <#a description#>
 *  @param func <#func description#>
 *
 *  @return <#return value description#>
 */
ouoval * builtin_var(ouoenv * e, ouoval * a, const char * func);

/**
 *  @brief  Load a file
 *
 *  @param e env
 *  @param a An Sexpr with file path
 *
 *  @return Result of evaluation
 */

ouoval * builtin_load(ouoenv * e, ouoval * a);

ouoval * builtin_error(ouoenv * e, ouoval * a);

ouoval * builtin_print(ouoenv * e, ouoval * a);

ouoval * builtin_println(ouoenv * e, ouoval * a);

ouoval * builtin_lambda(ouoenv * e, ouoval * a);

ouoval * builtin_def(ouoenv * e, ouoval * a);

ouoval * builtin_put(ouoenv * e, ouoval * a);

ouoval * builtin_con(ouoenv *e, ouoval * a);

/* new
 */
//ouoval * builtin_new(ouoenv *e, ouoval * a);

ouoval * builtin_if(ouoenv * e, ouoval* a);

ouoval * builtin_eq(ouoenv * e, ouoval * a);

ouoval * builtin_ne(ouoenv * e, ouoval * a);

ouoval * builtin_gt(ouoenv * e, ouoval * a);

ouoval * builtin_lt(ouoenv * e, ouoval * a);

ouoval * builtin_ge(ouoenv * e, ouoval * a);

ouoval * builtin_le(ouoenv * e, ouoval * a);

ouoval * builtin_cmp(ouoenv * e, ouoval * a, const char * op);

ouoval * builtin_ord(ouoenv * e, ouoval * a, const char* op);

/* Use operator string to see which operation to perform */
ouoval * builtin_op(ouoenv * e, ouoval * a, const char * op);

ouoval * builtin_eval(ouoenv * e, ouoval * a);

ouoval * builtin_list(ouoenv * e, ouoval * a);

ouoval * builtin_head(ouoenv * e, ouoval * a);

ouoval * builtin_tail(ouoenv * e, ouoval * a);

ouoval * builtin_eval(ouoenv * e, ouoval * a);

ouoval * builtin_join(ouoenv * e, ouoval * a);

ouoval * builtin_add(ouoenv * e, ouoval * a);

ouoval * builtin_sub(ouoenv * e, ouoval * a);

ouoval * builtin_mul(ouoenv * e, ouoval * a);

ouoval * builtin_div(ouoenv * e, ouoval * a);

ouoval * builtin_power(ouoenv * e, ouoval * a);

ouoval * builtin_mod(ouoenv * e, ouoval * a);

ouoval * builtin_in(ouoenv * e, ouoval * a);

ouoval * builtin_history(ouoenv * e, ouoval * a);


#endif /* OUOBUILTIN_H */
