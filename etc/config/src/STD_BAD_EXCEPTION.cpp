// checking for class std::bad_exception

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

    if ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();         // recurse infinitely
    }
}

}   // namespace std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


#ifndef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throws(args)   throw args
#else
#  define throws(ignore)
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION


#ifndef _RWSTD_NO_STD_SET_UNEXPECTED

namespace std {

#endif   // _RWSTD_NO_STD_SET_UNEXPECTED


typedef void (*unexpected_handler)();
unexpected_handler set_unexpected (void (*)()) throws (());


#ifndef _RWSTD_NO_STD_SET_UNEXPECTED

}   // std

#  define SET_UNEXPECTED(handler)   std::set_unexpected (handler)

#else

#  define SET_UNEXPECTED(handler)   set_unexpected (handler)

#endif   // _RWSTD_NO_STD_SET_UNEXPECTED

namespace std {

class bad_exception { };

}   // namespace std


// will fail if namespace std is not honored
enum bad_exception { };


void foo () throws ((std::bad_exception))
{
    // throw an object of a disallowed type
    throw 0;
}

void handler ()
{
    // throw an object of a type not allowed by the exception-specification
    // of the function that already violated its specification (due to which
    // the handler has been called); this requires the exception object to be
    // replaced by the runtime library by one of type [std]::bad_exception
    throw "";
}

int main ()
{
    SET_UNEXPECTED (handler);

    try {
        // will fail if runtime library doesn't recognize std::::bad_exception
        // by calling terminate()
        foo ();
    }
    catch (std::bad_exception&) {
        return 0;
    }
    catch (...) {
        return 1;
    }

    // runtime library is broken
    return 2;
}
