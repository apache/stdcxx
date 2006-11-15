
// also defined in COLLAPSE_TEMPLATE_STATICS.cpp
template <class T>
struct S
{
    // multiple instances of `t' must be collapsed
    static T t;
};

template <class T>
T S<T>::t;


#if defined (_WIN32) && defined (_DLL)
__declspec (dllexport)
#endif  // _WIN32 && _DLL
int bar ()
{
    // S<int>::t instantiated and modified in COLLAPSE_TEMPLATE_STATICS.cpp
    return S<int>::t++;
}
