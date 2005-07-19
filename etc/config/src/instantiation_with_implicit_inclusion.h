template <class T>
struct S
{
    T foo (T t) const { return t; }
    T bar (T);
};

template <class T>
T foo (S<T>, T);

template <class T>
T bar (S<T>, T);
