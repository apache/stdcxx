// checking function template overload

template <class T, class U>
struct A { };

template <class T, class U>
int foo (A<T, U>, T)
{
    return 0;
}

template <class T, class U>
int foo (A<T, U>, char)
{
    return 1;
}

template <class U>
int foo (A<char, U>, char)
{
    return 2;
}

int main ()
{
    A<double, double>  add;
    A<char, double>    acd;

    return !(0 == foo (add, 0.0) && 1 == foo (add, 'a') && 2 == foo (acd, 'a'));
}
