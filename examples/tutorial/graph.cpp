/**************************************************************************
 *
 * graph.cpp - Example of a Graph built from STL's map.
 *
 * $Id: //stdlib/dev/examples/stdlib/tutorial/graph.cpp#12 $
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

#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <string>

#include <examples.h>

typedef
    std::map<std::string, int, std::less<std::string>,
             std::allocator<std::pair<const std::string, int> > > stringVector;

typedef
    std::map<std::string, stringVector, std::less<std::string>,
             std::allocator<std::pair<const std::string,
                                           stringVector > > >     graph;

typedef std::pair<unsigned,std::string> DistancePair;

bool operator< (const DistancePair& lhs, const DistancePair& rhs) {
    
    return lhs.first < rhs.first;
}


bool operator> (const DistancePair& lhs, const DistancePair& rhs) {
    
    return lhs.first > rhs.first;
}


std::string pendleton ("Pendleton");
std::string pensacola ("Pensacola");
std::string peoria ("Peoria");
std::string phoenix ("Phoenix");
std::string pierre ("Pierre");
std::string pittsburgh ("Pittsburgh");
std::string princeton ("Princeton");
std::string pueblo ("Pueblo");

void shortestDistance (graph& city_map, std::string& start_city,
                       stringVector& distances) {
    
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
        if (0 == distances.count (city))
        {
            // Then add it to shortest distance map.
            distances[city] = distance;

            // And put values into queue.
            const stringVector& cities = city_map[city];
            stringVector::const_iterator start = cities.begin ();
            stringVector::const_iterator stop  = cities.end ();

            for (; start != stop; ++start) 
                que.push (DistancePair (distance + (*start).second,
                                      (*start).first));
        }
    }
}


// Global city map
graph cityMap;

int main () {
    
    std::cout << "Graph example program "
              << " - find shortest path in a directed graph." 
              << std::endl;

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
    
    stringVector distances;
    
    shortestDistance (cityMap, pierre, distances);
    stringVector::iterator where;

    std::cout << "Find the shortest path from : " 
              << pierre.c_str () << std::endl;

    for (where = distances.begin (); where != distances.end (); ++where)
        std::cout << "  Distance to: " << (*where).first << ":"
                  <<  (*where).second << std::endl;
        
    std::cout << "End of graph example program" << std::endl;

    return 0;
}




