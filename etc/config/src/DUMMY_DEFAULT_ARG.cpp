// checking for dummy default arguments

template <class T>
T foo (const T* = 0) { return T (); }

template <>
int foo<int>(const int*) { return 1; }

template <>
long foo<long>(const long*) { return 2; }


struct X { };

template <class T>
struct A {
    int i_;

    A (int i): i_ (i) { }
};

template <class T>
struct B {
    int i_;

    B (int i): i_ (i) { }
};


template <class T>
T bar (const X&, const T* = 0) { return T (); }

template <>
A<int> bar<A<int> >(const X&, const A<int>*) { return A<int>(1); }

template <>
B<long> bar<B<long> >(const X&, const B<long>*) { return B<long>(2); }


int main ()
{
    int result = 0;

    if (0 != foo<char>())
        result = result << 1 + 1;

    if (1 != foo<int>())
        result = result << 1 + 1;

    if (2 != foo<long>())
        result = result << 1 + 1;

    X x;

    if (0 != bar<char>(x))
        result = result << 1 + 1;

    if (1 != bar<A<int> >(x).i_)
        result = result << 1 + 1;

    if (2 != bar<B<long> >(x).i_)
        result = result << 1 + 1;

    return result;
}
