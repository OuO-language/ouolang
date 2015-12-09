//
//  ouoString.hpp
//  ouo
//
//  Created by Meirtz on 15/11/16.
//  Copyright © 2015年 OuOlang. All rights reserved.
//

#ifndef OUOSTRING_H
#define OUOSTRING_H

#include <stdio.h>
#include <string.h>
#include "ouolang.h"

ouoval * len (ouoenv * e, ouoval * a);
ouoval * cmp (ouoenv * e, ouoval * a);
ouoval * strn (ouoenv * e, ouoval * a);

#endif /* OUOSTRING_H */
