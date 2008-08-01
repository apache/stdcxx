/**************************************************************************
 *
 * stocks.h - Stocks program.
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

#ifndef STOCKS_H_INCLUDED
#define STOCKS_H_INCLUDED

#include <ctime>
#include <locale>
#include <string>
#include <deque>
#include <stdexcept>
#include <iterator>
#include <sstream>

#include <examples.h>


// The stock exchange example displays stock quotes at various exchanges
// distributed globally. 

// The objective of the example is to show monetary, numeric, and date
// and time values formated using standard and/or user-defined facets.


#if defined (_AIX)
#  define US_LOCALE             "en_US"
#  define GERMAN_LOCALE         "de_DE"
#  define FRENCH_LOCALE         "fr_FR"
#  define JAPANESE_LOCALE       "ja_JP"
#elif defined (__hpux)
#  define US_LOCALE             "en_US.iso88591"
#  define GERMAN_LOCALE         "de_DE.iso88591"
#  define FRENCH_LOCALE         "fr_FR.iso88591"
#  define JAPANESE_LOCALE       "ja_JP.eucJP"
#elif defined (__linux__)
#  define US_LOCALE             "en_US"
#  define GERMAN_LOCALE         "de_DE"
#  define FRENCH_LOCALE         "fr_FR"
#  define JAPANESE_LOCALE       "ja_JP"
#elif defined (__osf__)
#  define US_LOCALE             "en_US.ISO8859-1"
#  define GERMAN_LOCALE         "de_DE.ISO8859-1"
#  define FRENCH_LOCALE         "fr_FR.ISO8859-1"
#  define JAPANESE_LOCALE       "ja_JP"
#elif defined (SNI)
#  define US_LOCALE             "En"
#  define GERMAN_LOCALE         "De"
#  define FRENCH_LOCALE         "Fr_FR.850"
#  define JAPANESE_LOCALE       "Ja_JP.932"
#elif defined (_WIN32)
#  define US_LOCALE             "us"
#  define GERMAN_LOCALE         "german_germany.1252"
#  define FRENCH_LOCALE         "french_france.1252"
#  define JAPANESE_LOCALE       "japanese_japan.1252"
#else   // POSIX simple names
#  define US_LOCALE             "en_US"
#  define GERMAN_LOCALE         "de_DE"
#  define FRENCH_LOCALE         "fr_FR"
#  define JAPANESE_LOCALE       "ja_JP"
#endif


namespace {
    const int stock_change[10] = { 0, 1, -1, 2, -1, 3, -3, 4, -1, -1 };
    const int indexSize = 10;
    struct std::tm tmb;
}


struct Company
{
   Company (const std::string &s, double p)
       : companyName (s), offerPrice (p), stockPrice (0) {} 

   //In actuality a company should not manipulate stocks.
   //For simplicity let the company play with its own stock.

   void updateStock () {
      stockPrice = offerPrice + stockChange();
      if (stockPrice < 0) 
          stockPrice = -(stockPrice);
   }

   int stockChange () { 
       return stock_change[changeIndex(true)];         
   }

   int changeIndex (bool increment = false) {
       static int i = 0;
       if(increment)
           ++i;
       if (i > indexSize-1)
           i = 0;
       return i;
   }
 
   std::string companyName;
   double      offerPrice;  //initial offer price
   double      stockPrice;  //current market price

private:
   std::string companyNews; //Make use of messaging (if at all it works !)

   std::size_t marketOutlook() { 
       return changeIndex();
   }
};


struct StockXchange: public std::locale::facet
{
   typedef std::basic_ostream<char,std::char_traits<char> > outStream;
   typedef std::ostreambuf_iterator<char, std::char_traits<char> > iter_type;
   typedef std::deque<Company*,std::allocator<Company*> > database;
   
   typedef std::locale::facet facet;
     
   StockXchange(std::size_t refs=0): facet(refs){}
   virtual ~StockXchange();
   
   static std::locale::id id;
   virtual bool put (std::ostream& os) const;
   virtual void add (const std::string& name, double initPrice);
   virtual void localTime (std::ostream&) const;

protected:
   database companyDatabase;  

   friend StockXchange::outStream&
   operator<< (StockXchange::outStream&, const StockXchange&);
};


struct TokyoStockXchange: public StockXchange
{
    TokyoStockXchange (std::size_t refs = 0)
        : StockXchange (refs) { }
   
    virtual void localTime (std::ostream& os) const {
        // our contrived GMT time is 07:43:00, to use an actual time,
        // replace the following tmb assignments with:
        // tmb = std::gmtime (&tm); 
        tmb.tm_sec   =   0;
        tmb.tm_min   =  43;
        tmb.tm_hour  =   7;
        tmb.tm_mday  =   4;
        tmb.tm_mon   =   6;
        tmb.tm_year  =  88;
        tmb.tm_wday  =   1;
        tmb.tm_yday  = 185;
        tmb.tm_isdst =   1;

        StockXchange::localTime(os);
     }

    virtual bool put (std::ostream& os) const { 
       os << '\n';
       os << "######## TOKYO STOCK EXCHANGE #########\n";
       return StockXchange::put (os);
     }
}; 


struct FrankfurtStockXchange: public StockXchange
{
    FrankfurtStockXchange (std::size_t refs = 0)
        : StockXchange (refs) { }

    virtual void localTime(std::ostream& os) const {
       // our contrived GMT time is 07:43:00, to use an actual time, replace
       // the following three tmb assignments with:
       // tmb = gmtime(&tm); 
       tmb.tm_hour=7;
       tmb.tm_min=43;
       tmb.tm_sec=00;
       StockXchange::localTime(os);
     }

    virtual bool put (std::ostream& os) const { 
       os << '\n';
       os << "######## FRANKFURTER WERTPAPIERB\366RSE #########\n";
       return StockXchange::put (os);
     }
};


struct NewYorkStockXchange: public StockXchange
{
    NewYorkStockXchange (std::size_t refs = 0)
        : StockXchange (refs) { }

    virtual void localTime(std::ostream& os) const {
       // our contrived GMT time is 07:43:00, for a GMT-5 of 02:43:00
       // to use an actual time, replace the following three tmb 
       // assignments with:
       // tmb = localtime(&tm); 
       tmb.tm_hour=2;
       tmb.tm_min=43;
       tmb.tm_sec=00;
       StockXchange::localTime(os);
     }

    virtual bool put (std::ostream& os) const { 
       os << '\n';
       os << "######## NEW YORK STOCK EXCHANGE ########\n";
       return StockXchange::put (os);
     }
}; 


struct ParisStockXchange: public StockXchange
{
    ParisStockXchange (std::size_t refs = 0)
        : StockXchange (refs){}

    virtual void localTime(std::ostream& os) const {
       // our contrived GMT time is 07:43:00, to use an actual time, replace
       // the following three tmb assignments with:
       // tmb = gmtime(&tm); 
       tmb.tm_hour=7;
       tmb.tm_min=43;
       tmb.tm_sec=00;
       StockXchange::localTime(os);
    }

    virtual bool put (std::ostream& os) const { 
       os << '\n';
       os << "######## BOURSE DE PARIS #########\n";
       return StockXchange::put (os);
     }
};

#endif   // STOCKS_H_INCLUDED
