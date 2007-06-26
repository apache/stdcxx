/**************************************************************************
 *
 * widwork.cpp - Widget works inventory example.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <algorithm>
#include <examples.h>
#include <widwork.h>

std::ostream& operator<< ( std::ostream& out, const Widget& w )
{
        return out << "Widget " << w.id;
}

bool operator== (const Widget& lhs, const Widget& rhs) {
    return lhs.id == rhs.id;
}


bool operator< (const Widget& lhs, const Widget& rhs) {
    return lhs.id < rhs.id;
}


bool WidgetTester::operator () (const Widget & wid, int testid) const 
{
    return wid.id == testid;
}


void inventory::order (int wid) {
    
    std::cout << "Received order for widget type " << wid << std::endl;

    widgetList::iterator
        wehave = std::find_if (on_hand.begin (), on_hand.end (), 
                               std::bind2nd (WidgetTester (), wid));
    if (wehave != on_hand.end ()) {
        std::cout << "Ship " << *wehave << std::endl;
        on_hand.erase (wehave);
    }
    else {
        std::cout << "Back order widget of type "  << wid  << std::endl;
        on_order.push_front (wid);
    }
}


void inventory::receive (int wid) {

    std::cout << "Received shipment of widget type " << wid << std::endl;

    idList::iterator weneed = std::find (on_order.begin (),
                                         on_order.end (), wid);
    if (weneed != on_order.end ()) {
        std::cout << "Ship " << Widget (wid) << " to fill back order"
                  << std::endl;
        on_order.erase (weneed);
    }
    else
        on_hand.push_front (Widget (wid));
}


int main ()
{
    std::cout << "Widget Works test program" << std::endl;

    inventory stock;
    stock.receive (1);
    stock.receive (2);
    stock.receive (3);
    stock.order (2);
    stock.order (2);
    stock.order (3);
    stock.receive (2);
    
    std::cout << "End of widget words test program" << std::endl;

    return 0;
}














