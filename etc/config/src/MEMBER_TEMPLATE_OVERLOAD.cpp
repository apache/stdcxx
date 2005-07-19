// checking for member template overloads on return type

template <class T, class U, class V>
struct A { };

template <class T>
struct B { };

template <class T>
struct C { };

int n;

template <int N>
struct D
{
    template <class T, class U, class V>
    A<T, U, V>
    foo (T = T ()) { ++n; return A<T, U, V>(); }

    template <class T, class U>
    A<T, U, C<T> >
    foo (T t = T ()) { ++n; return foo<T, U, C<T> >(t); }

    template <class T>
    A<T, B<T>, C<T> >
    foo (T t = T ()) { ++n; return foo<T, B<T> >(t); }

    A<int, B<int>, C<int> >
    foo (int i = int ()) { ++n; return foo<int>(i); }
};

int main ()
{
    D<0> d;

    d.foo ();

    return !(4 == n);
}
