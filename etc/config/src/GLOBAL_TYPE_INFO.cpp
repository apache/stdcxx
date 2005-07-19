// checking for class ::type_info

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif


#if defined (__EDG__) || defined (__sgi) && !defined (__GNUG__)
   // let edg (or its derivative) know about this fake type_info
#  pragma define_type_info
#endif   // __EDG__


class type_info { };


struct B { virtual ~B () { } };
struct D: B { };
struct E: B { };


int main (int argc, char *argv[])
{
    const type_info* ti1 = 0;
    const type_info* ti2 = 0;

    if (argc > 3) {
        ti1 = &typeid (D);
        ti2 = &typeid (E);
    }
    else if (argc > 2) {
        ti1 = &typeid (D);
        ti2 = &typeid (D);
    }

    if (argc > 1)
        return ti1 == ti2;

    // link only test 
    return 0;
}
