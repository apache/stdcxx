// checking overloading on extern "C"

extern "C" {

    typedef int (*pf_t)(void);

    int foo (pf_t f) { return f (); }

    int bar () { return 0; }
}

extern "C++" {

    int foo (int (*f)(void)) { return f (); }

    int baz () { return 1; }
    
}

int main ()
{
    return !(0 == foo (bar) && 1 == foo (baz));
}
