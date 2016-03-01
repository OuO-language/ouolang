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
    OuOASSERT(a, a->count, "Two many or only one argument to fuck up!");
    OuOASSERT(a, a->cell[0]->type == OuOVAL_STR /* && the type of value */, "Wrong argument type You DICKSUCKINGCONDOMSTER!!!");
    
    
    char tmp = a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)];
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = '\0';
    ouoval *res = ouoval_str(&a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)]);
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = tmp;
    return ouoval_str(res->str);
}

ouoval * endswith(ouoenv * e, ouoval * a) {
    OuOASSERT(a, (a->count & 0x1) == 0, "endswith function needs even number arguments");
    
    ouoval * r = ouoval_qexpr();
    
    for (int i = 0; i < a->count; i += 2) {
        ouoval * arg1 = a->cell[i];
        OuOASSERT(a, a->cell[i + 1]->type == OuOVAL_STR /* && the type of value */, "Wrong argument type for argument at %d", i + 1);
        
        
        char * ending = a->cell[i + 1]->str;
        
        size_t ending_len = strlen(ending) - 1;
        
        if (arg1->type == OuOVAL_STR) {
            char * string = arg1->str;
            size_t string_len = strlen(string) - 1;
            
            if (string_len < ending_len) {
                r = ouoval_join(r, ouoval_num(0));
            } else {
                int found = 1;
                for (long long pos = 0; pos <= ending_len; pos++) {
                    if (ending[ending_len - pos] != string[string_len - pos]) {
                        found = 0;
                        break;
                    }
                }
                r = ouoval_add(r, ouoval_num(found));
            }
        } else if (arg1->type == OuOVAL_QEXPR) {
            for (int t = 0; t < arg1->count; t++) {
                if (arg1->cell[t]->type != OuOVAL_STR) {
                    ouoval_del(r);
                    return ouoval_err("Argument %d, index %d must be a string", i, t);
                }
                char * string = arg1->cell[t]->str;
                size_t string_len = strlen(string) - 1;
                
                if (string_len < ending_len) {
                    r = ouoval_join(r, ouoval_num(0));
                } else {
                    int found = 1;
                    for (long long pos = 0; pos <= ending_len; pos++) {
                        if (ending[ending_len - pos] != string[string_len - pos]) {
                            found = 0;
                            break;
                        }
                    }
                    r = ouoval_add(r, ouoval_num(found));
                }
            }
        }
    }
    return r;
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
