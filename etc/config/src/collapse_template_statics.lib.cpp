
// also defined in COLLAPSE_TEMPLATE_STATICS.cpp
template <class T>
struct S
{
    // multiple instances of `t' must be collapsed
    static T t;
};

template <class T>
T S<T>::t;


int bar ()
{
    // S<int>::t instantiated and modified in COLLAPSE_TEMPLATE_STATICS.cpp
    return S<int>::t++;
}
