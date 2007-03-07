// checking for type_info::name()

#include "config.h"


#ifndef _RWSTD_NO_STD_TYPE_INFO
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_TYPE_INFO
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_TYPE_INFO
#endif   // _RWSTD_NO_STD_TYPE_INFO


NAMESPACE (std) {


#if defined (__EDG__) || defined (__sgi)
   // let edg (or some of its derivatives) know about this fake type_info
#  if !defined (__GNUG__) && !defined (__INTEL_COMPILER)
#    pragma define_type_info
#  endif
#endif   // __EDG__


class type_info
{
public:

    const char* name () const;

    virtual ~type_info ();
};

#ifdef _RWSTD_NO_TYPE_INFO_DTOR

type_info::~type_info () { }

#endif   // _RWSTD_NO_TYPE_INFO_DTOR

}   // namespace std


struct B { virtual ~B () { } };
struct D: B { };
struct E: B { };
struct F: B { };


int main (int argc, char *argv[])
{
    D d;
    E e;
    F f;

    B *b1 = &d;
    B *b2 = &e;

    if (argc > 3) {
        b1 = &d;
        b2 = &f;
    }
    else if (argc > 2) {
        b1 = &d;
        b2 = &d;
    }

    if (argc > 1)
        return typeid (*b1).name () == typeid (*b2).name ();

    // link only test 
    return 0;
}
