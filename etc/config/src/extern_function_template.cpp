
template <class T>
struct S
{
    T t;
};

template <class T>
S<T> foobar (S<T> s)
{
    s.t = 1;

    return s;
}

template S<int> foobar (S<int>);

// extern template S<int> foobar(S<int>) declared
// in EXTERN_FUNCTION_TEMPLATE.cpp returns -1
