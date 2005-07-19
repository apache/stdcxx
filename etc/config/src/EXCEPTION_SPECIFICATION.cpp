// checking exception specification

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    abort ();
}

}   // std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD

extern "C" {

void handle_ABRT (int)
{
    // exit successfully
    exit (0);
}

}   // extern "C"

// test functionality; 
struct X {};
struct Y {};

void foobar () { throw Y (); }
void foo () throw (X) { foobar (); }

// test syntax
void bar (int) throw (int) {}

struct A { };
struct B
{
     B () throw (A*) { }
    ~B () throw ()   { }
};

int main ()
{
    signal (SIGABRT, handle_ABRT);

    try {
        // must not return
        foo ();
    }
    catch (X) {
        return 1;
    }
    catch (Y) {
        return 2;
    }
    catch (...) {
        return 3;
    }

    return 4;
}
