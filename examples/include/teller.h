/**************************************************************************
 *
 * teller.h - Header for bank teller sample program.
 *
 * $Id: //stdlib/dev/examples/stdlib/include/teller.h#7 $
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


#ifndef TELLER_H_INCLUDED
#define TELLER_H_INCLUDED

inline unsigned long int lcg_rand (int n);

class Customer
{
public:

    int arrivalTime;
    int processTime;

    Customer (int at = 0)
        : arrivalTime (at),
          processTime (2 + lcg_rand (6)) {}

    bool done () {   // are we done with our transaction?
        return --processTime < 0;
    }

    bool operator< (const Customer& c) const {   // order by arrival time
        return arrivalTime < c.arrivalTime;
    }
};


class Teller
{
public:
    Teller (): free (true) { }

    bool isFree () {   // are we free to service new customer?
        if (free)
            return true;
        if (customer.done())
            free = true;
        return free;
    }

    void addCustomer (const Customer &c) {   // start serving new customer
        customer = c;
        free = false;
    }

private:

    bool     free;
    Customer customer;
};

#endif   // TELLER_H_INCLUDED
