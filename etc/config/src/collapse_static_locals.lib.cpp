
// also defined in COLLAPSE_STATIC_LOCALS.cpp
inline int foo ()
{
    // multiple instances of `i' must be collapsed
    static int i = 0;
    return i++;
}

// bar() returns foo()
int bar ()
{
    return foo ();
}
