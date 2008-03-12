/**************************************************************************
 *
 * money_manip.cpp - Example program demonstrating the implementation
 *                   of user-defined manipulators for the convenient
 *                   extraction and insertion of monetary values
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
 **************************************************************************/

#include <iomanip>    // for __rw_smanip
#include <iostream>   // for cout
#include <locale>     // for money_get, money_put

#include <examples.h>


// implementation class of the extraction manipulator for monetary values
template <class moneyT>
struct money_get_manip
{
    moneyT *pval_;

    explicit money_get_manip (moneyT *pval)
        : pval_ (pval) { }

    template <class charT, class Traits>
    void
    operator() (std::basic_ios<charT, Traits> &strm, bool intl) const {
        typedef std::istreambuf_iterator<charT> Iter;
        typedef std::money_get<charT, Iter>     MoneyGet;

        std::ios_base::iostate err = std::ios_base::goodbit;
        const MoneyGet &mg = std::use_facet<MoneyGet>(strm.getloc ());

        // extract the monetary value from the stream
        mg.get (Iter (strm.rdbuf ()), Iter (), intl, strm, err, *pval_);

        if (std::ios_base::goodbit != err)
            strm.setstate (err);
    }
};


// implementation class of the insertion manipulator for monetary values
template <class moneyT>
struct money_put_manip
{
    const moneyT &val_;

    explicit money_put_manip (const moneyT &val)
        : val_ (val) { }

    template <class charT, class Traits>
    void
    operator() (std::basic_ios<charT, Traits> &strm, bool intl) const {
        typedef std::ostreambuf_iterator<charT> Iter;
        typedef std::money_put<charT, Iter>     MoneyPut;

        const MoneyPut &tp = std::use_facet<MoneyPut>(strm.getloc ());

        // insert the monetary value into the stream
        const Iter end =
            tp.put (Iter (strm.rdbuf ()), intl, strm, strm.fill (), val_);

        if (end.failed ())
            strm.setstate (std::ios_base::badbit);
    }
};


// manipulator for the extraction of monetary long double values
inline std::__rw_smanip<money_get_manip<long double>, bool>
get_money (long double *pval, bool intl = false)
{
    // return an object of the manipulator implementation type
    // that will store the function arguments until its function
    // call operator is invoked by the extraction operator for
    // std::__rw_smanip
    typedef money_get_manip<long double>     GetMoney;
    typedef std::__rw_smanip<GetMoney, bool> Manip;

    return Manip (GetMoney (pval), intl);
}


// manipulator for the extraction of monetary strings
template <class charT>
inline std::__rw_smanip<money_get_manip<std::basic_string<charT> >, bool>
get_money (std::basic_string<charT> *pval, bool intl = false)
{
    // return an object of the manipulator implementation type
    // that will store the function arguments until its function
    // call operator is invoked by the extraction operator for
    // std::__rw_smanip
    typedef std::basic_string<charT>         String;
    typedef money_get_manip<String>          GetMoney;
    typedef std::__rw_smanip<GetMoney, bool> Manip;

    return Manip (GetMoney (pval), intl);
}


// manipulator for the insertion of monetary long double values
inline std::__rw_smanip<money_put_manip<long double>, bool>
put_money (const long double &val, bool intl = false)
{
    // return an object of the manipulator implementation type
    // that will store the function arguments until its function
    // call operator is invoked by the insertion operator for
    // std::__rw_smanip
    typedef money_put_manip<long double>     PutMoney;
    typedef std::__rw_smanip<PutMoney, bool> Manip;

    return Manip (PutMoney (val), intl);
}


// manipulator for the insertion of monetary strings
template <class charT>
inline std::__rw_smanip<money_put_manip<std::basic_string<charT> >, bool>
put_money (const std::basic_string<charT> &val, bool intl = false)
{
    // return an object of the manipulator implementation type
    // that will store the function arguments until its function
    // call operator is invoked by the insertion operator for
    // std::__rw_smanip
    typedef std::basic_string<charT>         String;
    typedef money_put_manip<String>          PutMoney;
    typedef std::__rw_smanip<PutMoney, bool> Manip;

    return Manip (PutMoney (val), intl);
}


int main (int argc, char *argv[])
{
    const std::string valstr = argc < 2 ? "0" : argv [1];

    // set cin and cout's locale to the one specified in the environment
    std::cin.imbue (std::locale (""));
    std::cout.imbue (std::cin.getloc ());

    // output the monetary value specified by the command line argument,
    // including the currency symbol, in both the local and international
    // formats
    std::cout.setf (std::cout.showbase);
    std::cout << put_money (valstr) << '\n';
    std::cout << put_money (valstr, true) << '\n';

    long double val = 0.0L;

    // input a long double monetary value
    std::cin >> get_money (&val);

    // if the input was successful, output the extracted monetary value
    // otherwise, output an error message
    if (std::cin.good ()) {
        std::cout << put_money (val) << '\n';
        std::cout << put_money (val, true) << '\n';
    }
    else {
        std::cerr << "get_money (long double*) failed\n";
        return 1;
    }

    return 0;
}
