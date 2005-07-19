// checking template<> in friend specializations

template <class T>
struct S;

template <class T>
int operator== (const S<T>&, const S<T>&);

template <class T>
struct S
{
    friend int operator==<> (const S&, const S&);
}; 

template <class T>
int operator== (const S<T>&, const S<T>&)
{
    return 1;
}

int foo ()
{
    S<int> s;
    return !(s == s);
}
