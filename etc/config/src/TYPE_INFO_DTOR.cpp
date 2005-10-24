// checking for type_info dtor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

    *ip++ = 0;      // force a SIGSEGV
    terminate ();   // recurse infinitely
}

}

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


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


struct type_info
{
    virtual ~type_info ();
};


}   // namespace std


void foo (void *p)
{
    delete (std::type_info*)p;
}


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    foo (argv [argc]);

    // link only test 
    return 0;
}
