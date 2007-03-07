// checking template overloads on return type


template <class T>
struct A { char dummy [8]; };

template <class T, class U>
struct B { T t; char dummy [16]; };


// exercise the compiler's ability to define a function template
// whose template paramenter is used only in the return type of
// the template and not in its argument list; this requires that
// the type of the template argument be mangled into the name of
// each specialization of the template

template <class T, class U>
T bar (U)
{
    return T ();
}

int size;

template <class T>
T foo ()
{
    size = sizeof (T);

    return bar<T>("");
}

template <class T, class U>
B<T, U> foobar ()
{
    return foo<B<T, U> >();
}


int main ()
{
    int result = 0;

    foo<char>();

    if (size != sizeof (char))
        result = result << 1 + 1;

    foo<A<char> >();

    if (size != sizeof (A<char>))
        result = result << 1 + 1;

    foo<B<A<char>, int> >();

    if (size != sizeof (B<A<char>, int>))
        result = result << 1 + 1;

    foobar<char, B<char, A<int> > >();

    if (size != sizeof (B<char, B<char, A<int> > >))
        result = result << 1 + 1;

    return result;
}
