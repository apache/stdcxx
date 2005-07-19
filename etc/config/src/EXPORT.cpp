// checking for exported templates

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


#ifndef _RWSTD_NO_EXPORT_KEYWORD

// establish a dependency on EXPORT_KEYWORD.cpp which must be
// compiled first since it defines the exported function template
 
#endif   // _RWSTD_NO_EXPORT_KEYWORD


export template <class T>
T foo (T);

export template <class T>
struct S
{
    T bar ();
};

template <class T>
T S<T>::bar () { return foo (T ()); }


int main ()
{
    return S<int>().bar ();
}
