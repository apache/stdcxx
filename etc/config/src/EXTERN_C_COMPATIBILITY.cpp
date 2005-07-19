// checking for compatibility of extern "C" and "C++"

extern "C" {
    typedef int (*ext_c_fun)();

    int foo (int (*pf)()) { return pf (); }
}

extern "C++" {
    int bar () { return 0; }
}

int main ()
{
    return !(0 == foo ((ext_c_fun)bar));
}
