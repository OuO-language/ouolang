//
//  ouoString.cpp
//  ouo
//
//  Created by Meirtz on 15/11/16.
//  Copyright © 2015年 OuOlang. All rights reserved.
//

#include "ouoval.h"
#include "ouoString.h"

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
    OuOASSERT(a, a->count, "Tow many or only one argument!");
    OuOASSERT(a, a->cell[0]->type == OuOVAL_STR /* && the type of value */, "Wrong argument type.");
    
    
    char tmp = a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)];
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = '\0';
    ouoval *res = ouoval_str(&a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)]);
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = tmp;
    return ouoval_str(res->str);
}
