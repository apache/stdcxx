// checking for empty mem-initializer arglist

template <class T, class U>
struct S
{
    T t;
    U u;

    // exercise 12.6.2, p3, bullet 1
    S (): t (/* expression-list optional */), u () { }
};

int foo (char c)
{
    // dirty up the stack
    char dummy [32];
    for (int i = 0; i != sizeof dummy; ++i)
        dummy [i] = c + i;

    return dummy [0] + dummy [sizeof dummy - 1];
}

struct X
{
    char c;
    int  i;
};

int bar ()
{
    S<int, X> s;
    const char *p = (const char*)&s;

    // member must be default-initialized (zeroed out)
    for (int i = 0; i != sizeof s; ++i)
	if (*p)
	    return 0;
    return 1;
}

int main ()
{
    foo ('a');

    return !bar ();
}
