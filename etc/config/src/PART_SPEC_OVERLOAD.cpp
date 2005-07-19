// checking partial ordering of function templates

template <class T>
struct S { };

template <class T>
int operator== (const T &a, const T &b)
{
    return a.foobar (b);
}

template <class T>
int foo (T &a, T &b)
{
    return a.foobar (b);
}

template <class T>
int operator== (const S<T>&, const S<T>&)   // <-- testing
{
    return 1;
}

template <class T>
int foo (S<T> &a, S<T> &b)   // <-- testing
{
    return a == b;
}

int bar ()
{
    S<int> s;
    return !(s == s && foo (s, s));
}
