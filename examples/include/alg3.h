/**************************************************************************
 *
 * alg3.h - Header for sample programs for STL generic algorihtms
 *          that modify their arguments in place.
 *
 * $Id: //stdlib/dev/examples/stdlib/include/alg3.h#7 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef ALG3_H_INCLUDED
#define ALG3_H_INCLUDED

#include <rw/_random.h>

class iotaGenerator
{
public:
    iotaGenerator (int iv) : current (iv)
        { }
    int operator () () {
        return current++;
    }
private:
    int current;
};


struct RandomInteger
{
    int operator () (int m) {
        // using a proprietary algorithm
        return _RW::__rw_random (m);
    }
};

#endif   // ALG3_H_INCLUDED
