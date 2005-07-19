
// also defined in COLLAPSE_TEMPLATE_STATICS.cpp
template <class T>
T foo (T)
{
    // multiple instances of `t' must be collapsed
    static T t;
    return t++;
}


// bar() returns foo()
int bar ()
{
    return foo (0);
}
