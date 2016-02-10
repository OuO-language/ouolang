//
//  ouoString.cpp
//  ouo
//
//  Created by Meirtz on 15/11/16.
//  Copyright © 2015年 0xBBC. All rights reserved.
//

#include "ouoString.h"
#include <string.h>
#include <stdio.h>
#include "ouoval.h"


ouoval * len (ouoenv * e, ouoval * a) {
    OuOASSERT(a, a->count == 1, "Too many arguments have passed when I just need ONE!");
    
    return ouoval_num(strlen(a->cell[0]->str));
}
ouoval * cmp (ouoenv * e, ouoval * a) {
    OuOASSERT(a, a->count >= 2, "What the fuck you want me to compare when I only have one argument?");
    OuOASSERT(a, a->count <= 2, "Too many arguments have passed when I just need TWO!");
    
    return ouoval_num(strcmp(a->cell[0]->str, a->cell[1]->str));
}
ouoval * strn (ouoenv * e, ouoval * a) {
    OuOASSERT(a, a->count, "Tow many or only one argument to fuck up!");
    OuOASSERT(a, a->cell[0]->type == OuOVAL_STR /* && the type of value */, "Wrong argument type You DICKSUCKINGCONDOMSTER!!!");
    
    
    char tmp = a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)];
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = '\0';
    ouoval *res = ouoval_str(&a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)]);
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = tmp;
    return ouoval_str(res->str);
}

ouoval * hash (ouoenv * e, ouoval * a) {
    uint64_t hash = 0;
    uint64_t i;
    
    for (i = 0; i < strlen(a->cell[0]->str) ; i++) {
        hash += (uint64_t)a->cell[0]->str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    //printf("Debug:: %lld\n", hash);
    return ouoval_num(hash);
}
