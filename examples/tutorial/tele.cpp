/**************************************************************************
 *
 * tele.cpp - Telephone directory sample program.
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

#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
   
#include <examples.h>

typedef std::map<std::string, long, std::less<std::string>,
            std::allocator<std::pair<const std::string, long> > > friendMap;
typedef std::map<long, std::string, std::less<long>,
            std::allocator<std::pair<const long, std::string> > > sortedMap;

typedef friendMap::value_type entry_type;
typedef sortedMap::value_type sorted_entry_type;


void printEntry (const entry_type & entry) {
    
    std::cout << entry.first << ":" << entry.second << std::endl;
}


void printSortedEntry (const sorted_entry_type & entry) {
    
    std::cout << entry.first << ":" << entry.second << std::endl;
}


long prefix (const entry_type& entry) {
    return entry.second / 10000;
}


bool prefixCompare (const entry_type & a, const entry_type & b) {

    return prefix (a) < prefix (b);
}


class checkPrefix {
public:
    checkPrefix (long p) : testPrefix (p)
        { }
    long testPrefix;
    bool operator () (const entry_type& entry) {
        return prefix (entry)==testPrefix;
    }
};
            
class telephoneDirectory {
public:
    void addEntry (std::string name, long number) {
        database[name] = number;
    }
    void remove (std::string name) {
        database.erase (name);
    }
    void update (std::string name, long number) {
        remove (name);addEntry (name, number);
    }
    void displayDatabase () {
        std::for_each (database.begin (), database.end (), printEntry);
    }
    void displayPrefix (int);
    void displayByPrefix (); 
private:
    friendMap database;
};


void telephoneDirectory::displayPrefix (int pfx) {
    
    std::cout << "Listing for prefix " << pfx << std::endl;

    friendMap::iterator
        where = std::find_if (database.begin (), database.end (),
                         checkPrefix (pfx));
    while (where != database.end ()) {
        printEntry (*where);
        where = std::find_if (++where, database.end (), checkPrefix (pfx));
    }

    std::cout << "end of prefix listing" << std::endl;
}


void telephoneDirectory::displayByPrefix () {
    
    std::cout << "Display by prefix" << std::endl;

    sortedMap sortedData;
    for (friendMap::iterator i = database.begin (); i != database.end (); i++)
        sortedData.insert (sortedMap::value_type ((*i).second, (*i).first));
    std::for_each (sortedData.begin (), sortedData.end (), printSortedEntry);

    std::cout << "end display by prefix" << std::endl;
}


int main () {
    
    std::cout << "Telephone Directory sample program" << std::endl;

    telephoneDirectory friends;
    friends.addEntry ("Samantha", 6342343);
    friends.addEntry ("Brenda", 5436546);
    friends.addEntry ("Fred", 7435423);
    friends.addEntry ("Allen", 6348723);
    friends.displayDatabase ();
    friends.displayPrefix (634);
    friends.displayByPrefix ();
    
    std::cout << "End of telephone directory sample program" << std::endl;

    return 0;
}
