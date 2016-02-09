//
//  ouoval.h
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#ifndef OUOVAL_H
#define OUOVAL_H

#include <stdio.h>
#include "ouolang.h"

/**
 *  @brief  Forward Declarations
 */
struct ouoval;
struct ouoenv;
struct mpc_ast_t;
typedef ouoval * (*ouobuiltin)(ouoenv *, ouoval *);

typedef struct ouoval {
    int type;
    
    /* Basic
     * Error and Symbol types have some string data
     */
    double num;
    char * err;
    char * sym;
    char * str;
    char * value;
    
    /* Function */
    ouobuiltin builtin;
    ouoenv * env;
    ouoval * formals;
    ouoval * body;
    
    /* Expression
     * Count and Pointer to a list of "ouoval*"
     */
    int count;
    ouoval ** cell;
} ouoval;

/**
 Create Enumeration of Possible ouoval Types
 */
enum { OuOVAL_ERR, OuOVAL_NUM, OuOVAL_VALUE, OuOVAL_SYM, OuOVAL_STR, OuOVAL_SEXPR, OuOVAL_QEXPR, OuOVAL_FUNC };

/**
 Create Enumeration of Possible Error Types
 */
enum { OuOERR_DIV_ZERO, OuOERR_BAD_OP, OuOERR_BAD_NUM };

const char * ouotype_name(int t);

/**
 *  @brief  Create a new error type ouoval with message
 *
 *  @param x error message
 *
 *  @return ouoval
 */
ouoval * ouoval_err(const char* fmt, ...);

/**
 *  @brief  Create a new number type ouoval
 *
 *  @param x a number
 *
 *  @return ouoval
 */
ouoval * ouoval_num(double x);

/**
 *  @brief  Construct a pointer to a new Symbol ouoval
 *
 *  @param s symbol
 *
 *  @return ouoval
 */
ouoval * ouoval_sym(const char * s);

/**
 *  @brief  Construct a pointer to a new string ouoval
 *
 *  @param s string
 *
 *  @return ouoval
 */
ouoval * ouoval_str(const char * s);

/**
 *  @brief  A pointer to a new empty Sexpr ouoval
 *
 *  @return ouoval
 */
ouoval * ouoval_sexpr(void);

/**
 *  @brief  A pointer to a new empty Qexpr ouoval
 *
 *  @return ouoval
 */
ouoval * ouoval_qexpr(void);

/**
 *  @brief  Read expression
 *
 *  @param t ast
 *
 *  @return ouoval
 */
ouoval * ouoval_read(mpc_ast_t * t);

/**
 *  @brief  Read number type ouoval expression
 *
 *  @param t ast
 *
 *  @return ouoval
 */
ouoval * ouoval_read_num(mpc_ast_t * t);

/**
 *  @brief  Read string type ouoval expression
 *
 *  @param t ast
 *
 *  @return ouoval
 */
ouoval * ouoval_read_str(mpc_ast_t * t);

ouoval * ouoval_lambda(ouoval * formals, ouoval * body);

ouoval * ouoval_builtin(ouobuiltin func);

ouoval * ouoval_add(ouoval * v, ouoval * x);

/**
 *  @brief  Print an "ouoval"
 *
 *  @param v an ouoval to print
 */
void ouoval_print(ouoval * v);

/**
 *  @brief  Print an "ouoval" followed by a newline
 *
 *  @param v an ouoval to print
 */
void ouoval_println(ouoval * v);

/**
 *  @brief  Print a string type "ouoval"
 *
 *  @param v an ouoval to print
 */
void ouoval_print_str(ouoval * v);

/**
 *  @brief  Print the Sexpr/Qexpr type "ouoval"
 *
 *  @param v an ouoval to print
 */
void ouoval_expr_print(ouoval * v, char open, char close);

ouoval * ouoval_eval(ouoenv * e, ouoval * v);

ouoval * ouoval_eval_sexpr(ouoenv * e, ouoval * v);

ouoval * ouoval_copy(ouoval * v);

ouoval * ouoval_pop(ouoval * v, int i);

ouoval * ouoval_take(ouoval * v, int i);

ouoval * ouoval_join(ouoval * x, ouoval * y);

ouoval * ouoval_call(ouoenv* e, ouoval * f, ouoval * a);

void ouoval_del(ouoval * v);

int ouoval_eq(ouoval * x, ouoval * y);

#endif /* OUOVAL_H */
