// checking for pointer exception specification

struct S
{
    void foo () throw () { }
    void bar () const throw (int) { }
    void foobar () throw (int, double) { }
};

void foo () throw (char, long)
{
    // exception specification is not part of a function type (15.4, p12)
    // but must be observed when assigning the adress of a function to
    // a pointer to a function (15.4, p3)
    void (*p_foo)() throw (char, long)         = &foo;

    void (S::*p_mfoo)() throw ()               = &S::foo;
    void (S::*p_mbar)() const throw (int)      = &S::bar;
    void (S::*p_mfoobar)() throw (int, double) = &S::foobar;

    // prevent "unused" warnings
    (void)&p_foo;
    (void)&p_mfoo;
    (void)&p_mbar;
    (void)&p_mfoobar;
}
