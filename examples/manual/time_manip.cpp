/**************************************************************************
 *
 * time_manip.cpp - Example program demonstrating the implementation
 *                  of user-defined manipulators for the convenient
 *                  extraction and insertion of std::tm values
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

#include <ctime>      // for localtime(), time()
#include <iomanip>    // for __rw_smanip
#include <iostream>   // for cout
#include <locale>     // for time_get, time_put

#include <examples.h>


// implementation class of the extraction manipulator of std::tm values
template <class charT>
struct time_get_manip
{
    std::tm *tmb_;

    explicit time_get_manip (std::tm *tmb)
        : tmb_ (tmb) { }

    template <class Traits>
    void operator() (std::basic_istream<charT, Traits>&, const charT*) const;
};


template <class charT>
template <class Traits>
void time_get_manip<charT>::
operator()(std::basic_istream<charT, Traits> &strm, const charT *fmt) const
{
    // begin by constructing a sentry object for the stream
    const typename std::basic_istream<charT, Traits>::sentry guard (strm);

    if (guard) {

        // compute the end pointer of the format specifier
        const charT* const end = fmt + Traits::length (fmt);

        std::ios_base::iostate err = strm.goodbit;

        typedef std::istreambuf_iterator<charT> Iter;
        typedef std::time_get<charT, Iter>      TimeGet;

        try {
            const TimeGet &tg = std::use_facet<TimeGet>(strm.getloc ());

            // try to extract the time from the stream using
            // an extension of this implementation
            tg.get (Iter (strm.rdbuf ()), Iter (), strm, err, tmb_, fmt, end);
        }
        catch (...) {
            bool rethrow = false;

            try {
                // set badbit in exceptions without causing
                // ios_base::failure to be thrown
                strm.setstate (strm.badbit);
            }
            catch (...) {
                // set a flag to rethrow the originally caught exception
                rethrow = true;
            }

            if (rethrow)
                throw;
        }

        if (strm.goodbit != err)
            strm.setstate (err);
    }
}


// implementation class of the insertion manipulator of std::tm values
template <class charT>
struct time_put_manip
{
    const std::tm *tmb_;

    explicit time_put_manip (const std::tm *tmb)
        : tmb_ (tmb) { }

    template <class Traits>
    void operator() (std::basic_ostream<charT, Traits>&, const charT*) const;
};


template <class charT>
template <class Traits>
void time_put_manip<charT>::
operator() (std::basic_ostream<charT, Traits> &strm, const charT *fmt) const
{
    // begin by constructing a sentry object for the stream
    const typename std::basic_ostream<charT, Traits>::sentry guard (strm);

    if (guard) {

        // compute the end pointer of the format specifier
        const charT* const end = fmt + Traits::length (fmt);

        typedef std::ostreambuf_iterator<charT> Iter;
        typedef std::time_put<charT, Iter>      TimePut;

        Iter it (strm.rdbuf ());

        try {
            const TimePut &tp = std::use_facet<TimePut>(strm.getloc ());

            // try to format and insert the time object into the stream
            it = tp.put (it, strm, strm.fill (), tmb_, fmt, end);
        }
        catch (...) {
            bool rethrow = false;

            try {
                // set badbit in exceptions without causing
                // ios_base::failure to be thrown
                strm.setstate (strm.badbit);
            }
            catch (...) {
                // set a flag to rethrow the originally caught exception
                rethrow = true;
            }

            if (rethrow)
                throw;
        }

        // set badbit on failure to insert (which may cause
        // ios_base::failure to be thrown)
        if (it.failed ())
            strm.setstate (strm.badbit);
    }
}


// manipulator for the extraction of std::tm values
template <class charT>
inline std::__rw_smanip<time_get_manip<charT>, const charT*>
get_time (std::tm *tmb, const charT *fmt)
{
    // return an object of the manipulator implementation type
    // that will store the function arguments until its function
    // call operator is invoked by the extraction operator for
    // std::__rw_smanip
    typedef time_get_manip<charT>                   GetTime;
    typedef std::__rw_smanip<GetTime, const charT*> Manip;

    return Manip (GetTime (tmb), fmt);
}


// manipulator for the insertion of std::tm values
template <class charT>
inline std::__rw_smanip<time_put_manip<charT>, const charT*>
put_time (const std::tm *tmb, const charT *fmt)
{
    // return an object of the manipulator implementation type
    // that will store the function arguments until its function
    // call operator is invoked by the insertion operator for
    // std::__rw_smanip
    typedef time_put_manip<charT>                   PutTime;
    typedef std::__rw_smanip<PutTime, const charT*> Manip;

    return Manip (PutTime (tmb), fmt);
}


int main (int argc, char *argv[])
{
    // the first optional command line argument specifies
    // the extration format specifier string
    const char* const get_fmt = argc < 2 ? "%c" : argv [1];

    // the second optional command line argument specifies
    // the insertion format specifier string
    const char* const put_fmt = argc < 3 ? "%c" : argv [2];

    // obtain the local time
    const std::time_t t = std::time (0);
    std::tm* const tmb = std::localtime (&t);

    // set cout's locale to the one specified in the environment
    std::cout.imbue (std::locale (""));

    // output the local time using the specified output format
    std::cout << put_time (tmb, put_fmt) << '\n';

    // input time in the specified input format using the "C" locale
    std::cin >> get_time (tmb, get_fmt);

    // on success output the extracted time using the specified
    // output format; otherwise output an error message
    if (std::cin.good ())
        std::cout << put_time (tmb, put_fmt) << '\n';
    else {
        std::cerr << "get_time (std::tm*, \"" << get_fmt << "\") failed\n";
        return 1;
    }

    return 0;
}
