/**************************************************************************
 *
 * widwork.h - header for widget works, from Chapter 6.3
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

#ifndef WIDWORK_H_INCLUDED
#define WIDWORK_H_INCLUDED

#include <list>
#include <functional>
#include <iostream>
    
class  Widget {
public:
    Widget (int a = 0) : id (a)
        { }

    void operator= (const Widget& rhs) {
        id = rhs.id;
    }

    friend std::ostream& operator<< ( std::ostream&, const Widget& );
    int id;
};

typedef std::list<Widget, std::allocator<Widget> > widgetList;
typedef std::list<int, std::allocator<int> >       idList;

struct WidgetTester : std::binary_function<Widget, int, bool> 
{
public:
    bool operator () (const Widget & wid, int testid) const;
};

class inventory 
{
public:
    void order (int wid);   // Process order for widget type wid.
    void receive (int wid); // Receive widget of type wid in shipment.

private:
    widgetList on_hand;
    idList     on_order;
};


#endif   // WIDWORK_H_INCLUDED
