
// also defined in COLLAPSE_STATIC_LOCALS.cpp
inline int foo ()
{
    // multiple instances of `i' must be collapsed
    static int i = 0;
    return i++;
}

// bar() returns foo()
#if defined (_WIN32) && defined (_DLL)
__declspec (dllexport)
#endif  // _WIN32 && _DLL
int bar ()
{
    return foo ();
}
