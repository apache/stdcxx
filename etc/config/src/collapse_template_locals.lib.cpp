
// also defined in COLLAPSE_TEMPLATE_STATICS.cpp
template <class T>
T foo (T)
{
    // multiple instances of `t' must be collapsed
    static T t;
    return t++;
}


// bar() returns foo()
#if defined (_WIN32) && defined (_DLL)
__declspec (dllexport)
#endif  // _WIN32 && _DLL
int bar ()
{
    return foo (0);
}
