/**************************************************************************
 *
 * deque.cpp - Example program for deque class.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/deque.cpp#12 $
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

#include <deque>
#include <iostream>
#include <string>

#include <examples.h>

typedef std::deque<std::string, std::allocator<std::string> > Deque;

Deque deck_of_cards;
Deque current_hand;


void initialize_cards (Deque &cards)
{
    cards.push_front ("ace of spades");
    cards.push_front ("king of spades");
    cards.push_front ("queen of spades");
    cards.push_front ("jack of spades");
    cards.push_front ("ten of spades");
    //
    // etc.
    //
}


template <class It, class It2> 
void print_current_hand (It start, It2 end) 
{
    while (start < end) 
        std::cout << *start++ << std::endl;
}


template <class It, class It2>
void deal_cards (It, It2 end)
{
    for (int i = 0; i < 5; i++) {
        current_hand.insert (current_hand.begin (), *end);
        deck_of_cards.erase (end++);
    }
}


void play_poker ()
{
    initialize_cards (deck_of_cards);
    deal_cards (current_hand.begin (), deck_of_cards.begin ()); 
}


int main () 
{
    play_poker ();

    print_current_hand (current_hand.begin (), current_hand.end ());

    return 0;
}
