// checking for references to static symbols in template

static int static_function (int i) { return i; }
static int static_function (double d) { return int (d); }

template <class T>
int foo (T t)
{
    // check whether static functions can be called from
    // a function template ( EDG eccp in strict mode or
    // SunPro do not allow it -- see PR #25961)
    return static_function (t);
}

int main ()
{
    foo (0);
    foo (0.0);
}
