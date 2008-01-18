/**************************************************************************
 *
 * stocks.cpp - An example program using STL's  money_punct, num_put
 *              and time_put locale facets.
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

#include <clocale>
#include <iostream>
#include <locale>

#include <stocks.h>
#include <examples.h>


// definition of the required facet id
std::locale::id StockXchange::id;


StockXchange::~StockXchange ()
{
    database::const_iterator begin = companyDatabase.begin ();
    database::const_iterator end   = companyDatabase.end ();

    while (begin < end)
        delete *begin++;
}


void StockXchange::localTime (std::ostream& os) const
{
    const char pat[] = "%c";

    typedef std::time_put<char, iter_type> TimePut;
    const TimePut& tp = std::use_facet<TimePut>(os.getloc ());
    iter_type begin (os);

    os << "\t [";
    tp.put (begin, os, ' ', &tmb, &pat [0], pat + sizeof pat - 1);
    os << "]\n";
}


void StockXchange::add (const std::string& name, double initPrice)
{
    companyDatabase.push_front (new Company (name, initPrice));     
}


bool StockXchange::put (std::ostream& os) const
{
    std::locale loc = os.getloc ();
    localTime (os); //display the local time

    typedef std::moneypunct<char, false> MoneyPunct;
    const MoneyPunct &mpunct = std::use_facet<MoneyPunct>(loc);

    typedef std::num_put<char, iter_type> NumPut;
    const NumPut &np = std::use_facet<NumPut>(loc);
    
    os << "\nCompany" << "\t\t\t" << "Initial Price" << "\t"
       << "Current Price" << "\t" << "Volume\n"
       << "-------" << "\t\t\t" << "------------" << "\t"
       << "----------" << "\t" << "______\n\n";
    
    iter_type itbegin (os);

    database::const_iterator begin = companyDatabase.begin ();
    database::const_iterator end   = companyDatabase.end ();
    
    while (begin < end) {
        
        Company *info = *begin++;
        info->updateStock ();
        
        os << info->companyName << "\t\t";
        os << mpunct.curr_symbol ();
        np.put (itbegin, os, ' ', info->offerPrice);
        os << "\t\t";
        os << mpunct.curr_symbol ();
        np.put (itbegin, os, ' ', info->stockPrice);
        os << "\t\t";
        long volume = stock_change[info->changeIndex ()]*1000L;
        if (volume < 0)
            volume = -volume;
        np.put (itbegin, os, ' ', volume);
        os << '\n';
    }

    return true;
}


std::ostream & operator<< (std::ostream& os, const StockXchange&)
{
    const std::locale loc = os.getloc ();
    const StockXchange &se_facet = std::use_facet<StockXchange>(loc);
    se_facet.put (os);
    return os;
}


int main ()
{
    typedef std::pair<StockXchange*, std::locale> sl_pair;
    typedef std::deque<sl_pair, std::allocator<sl_pair> > Xchange;
    Xchange sXchange;
    
    std::ostream os (std::cout.rdbuf ());
    const std::locale loc (os.getloc ());
    
    const char *p = std::setlocale (LC_ALL, US_LOCALE);
    if (!p)
        std::cerr << "\nNot a valid locale: " << US_LOCALE << '\n';
    else {
        NewYorkStockXchange *nse = new NewYorkStockXchange;
        nse->add ("Hyper Software", 20.50);
        nse->add ("Florida Fish", 15.10);
        nse->add ("Inka Inc", 9.50);
        nse->add ("Emory Chemicals", 11.00);
   
        os.imbue (std::locale (std::locale (US_LOCALE), nse));
        sXchange.push_front (sl_pair (nse, os.getloc ()));
        os << *nse;
    }
    
    p = std::setlocale (LC_ALL, GERMAN_LOCALE);
    if (!p)
        std::cerr<< "\nNot a valid locale: " << GERMAN_LOCALE << '\n';
    else {
        FrankfurtStockXchange *fse = new FrankfurtStockXchange;   
        fse->add ("B\166rsen-Software", 9.75);
        fse->add ("M\174nchner R\174ck", 19.75);
    
        os.imbue (std::locale (std::locale (GERMAN_LOCALE), fse));
        sXchange.push_front (sl_pair (fse, os.getloc ()));
        os << *fse;
    }
    
    p = std::setlocale (LC_ALL, FRENCH_LOCALE);
    if (!p)
        std::cerr << "\nNot a valid locale: " << FRENCH_LOCALE << '\n';
    else {
        ParisStockXchange *pse = new ParisStockXchange;   
        pse->add ("Wines Inc.", 11.50);
        pse->add ("Eiffel Co.", 11.50);
    
        os.imbue (std::locale (std::locale (FRENCH_LOCALE), pse));
        sXchange.push_front (sl_pair (pse, os.getloc ()));
        os << *pse;
    }
    
    p = std::setlocale (LC_ALL, JAPANESE_LOCALE);
    if (!p)
        std::cerr << "\nNot a valid locale: " << JAPANESE_LOCALE << '\n';
    else {
        TokyoStockXchange *tse = new TokyoStockXchange;
        tse->add ("Akiro Electronics", 12.30);
    
        os.imbue (std::locale (std::locale (JAPANESE_LOCALE), tse));
        sXchange.push_front (sl_pair (tse, os.getloc ()));
        os << *tse;
    }
    
    for (char q = '\0'; ; ) {
        std::cout << "\nWant to see another quote [enter 'q' to quit] ?"; 
        std::cin >> q;
        if (q != 'q') {
            Xchange::const_iterator it_begin = sXchange.begin ();
            Xchange::const_iterator it_end   = sXchange.end ();
            while (it_begin < it_end) {
                os.imbue ((*it_begin).second);
                os << (*(*it_begin).first);
                it_begin++;
            }
        }
        else
            break;
    }

    os << '\n';

    os.imbue (loc);

    return 0;
}
