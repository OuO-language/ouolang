//
//  ouoassert.h
//  OuO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OuOlang. All rights reserved.
//

#ifndef OUOASSERT_H
#define OUOASSERT_H

#define OuOASSERT(args, cond, fmt, ...) \
    if (!(cond)) { ouoval* err = ouoval_err(fmt, ##__VA_ARGS__); ouoval_del(args); return err; }

#define OuOASSERT_TYPE(func, args, index, expect) \
    OuOASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ouotype_name(args->cell[index]->type), ouotype_name(expect))

#define OuOASSERT_NUM(func, args, num) \
    OuOASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define OuOASSERT_NOT_EMPTY(func, args, index) \
    OuOASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

#endif /* OUOASSERT_H */
