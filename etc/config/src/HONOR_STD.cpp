// checking if namespace std is honored


// g++ 2.9x ignores namespace std unless the -fhonor-std option is used

namespace std {

int foo () { return 0; }

struct S { };
   
}

long foo () { return 1; }

enum S { s0 };

int bar (std::S)
{
    return !(0 == std::foo ());
}
