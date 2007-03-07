// checking for conversion in template default arguments


template <class T>
struct A { };

template <class T>
void foo (T t = T ('x'))
{
}

template <class T>
A<T> bar (T t = T ('y'))
{
    return A<T>();
}

void baz ()
{
    foo<int>();
    bar<int>();
}
