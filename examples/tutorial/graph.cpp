/**************************************************************************
 *
 * graph.cpp - Example of a Graph built from STL's map.
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
 * Copyright 1994-2006, 2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <string>

#include <examples.h>

typedef std::map<std::string, int>       Distances;
typedef std::map<std::string, Distances> Cities;
typedef std::pair<unsigned, std::string> DistancePair;

bool operator< (const DistancePair& lhs, const DistancePair& rhs) {
    
    return lhs.first < rhs.first;
}


bool operator> (const DistancePair& lhs, const DistancePair& rhs) {
    
    return lhs.first > rhs.first;
}


static void
shortestDistance (Cities            &city_map,
                  const std::string &start_city,
                  Distances         &dist)
{
    
    // Process a priority queue of distances to nodes.
    std::priority_queue<DistancePair,
                        std::vector<DistancePair,
                                    std::allocator<DistancePair> >,
                        std::greater<DistancePair> > que;

    que.push (DistancePair (0, start_city));
    
    while (! que.empty ()) {
        
        // Pull nearest city from queue.
        int distance = que.top ().first;
        std::string city = que.top ().second;
        que.pop ();

        // If we haven't seen it already, process it.
        if (0 == dist.count (city))
        {
            // Then add it to shortest distance map.
            dist[city] = distance;

            // And put values into queue.
            const Distances& cities = city_map[city];
            Distances::const_iterator start = cities.begin ();
            Distances::const_iterator stop  = cities.end ();

            for (; start != stop; ++start) 
                que.push (DistancePair (distance + (*start).second,
                                      (*start).first));
        }
    }
}


int main () {
    
    std::cout << "Graph example program "
              << " - find shortest path in a directed graph." 
              << std::endl;

    static const char pendleton[]  = "Pendleton";
    static const char pensacola[]  = "Pensacola";
    static const char peoria[]     = "Peoria";
    static const char phoenix[]    = "Phoenix";
    static const char pierre[]     = "Pierre";
    static const char pittsburgh[] = "Pittsburgh";
    static const char princeton[]  = "Princeton";
    static const char pueblo[]     = "Pueblo";

    Cities cityMap;

    cityMap[pendleton][phoenix]    = 4;
    cityMap[pendleton][pueblo]     = 8;
    cityMap[pensacola][phoenix]    = 5;
    cityMap[peoria][pittsburgh]    = 5;
    cityMap[peoria][pueblo]        = 3;
    cityMap[phoenix][peoria]       = 4;
    cityMap[phoenix][pittsburgh]   = 10;
    cityMap[phoenix][pueblo]       = 3;
    cityMap[pierre][pendleton]     = 2;
    cityMap[pittsburgh][pensacola] = 4;
    cityMap[princeton][pittsburgh] = 2;
    cityMap[pueblo][pierre]        = 3;
    
    Distances dist;
    
    shortestDistance (cityMap, pierre, dist);
    Distances::iterator where;

    std::cout << "Find the shortest path from : " 
              << pierre << '\n';

    for (where = dist.begin (); where != dist.end (); ++where)
        std::cout << "  Distance to: " << (*where).first << ":"
                  <<  (*where).second << '\n';
        
    std::cout << "End of graph example program" << '\n';

    return 0;
}
