// checking for inline member templates

struct A
{
    template <class T>
    struct AB { };

    template <class T>
    void foo (T) { }
};


template <class T>
struct B
{
    template <class U>
    struct BB { };

    template <class U>
    void foo (U) { }
};


// check that member templates are usable
void foo (A a, A::AB<int> aab, B<int> b, B<char>::BB<int> bbb)
{
    a.foo (aab);

    b.foo (bbb);
}
