// checking for exceptions from extern "C" functions

// test tries to determine whether it's possible to throw exceptions
// from functions with C language linkage

extern "C" {

int extern_c_function (int x, int y, int (*fun)(int, int))
{
    if (fun)
        return fun (x, y);

    return 0;

}

int div_throw (int x, int y)
{
    if (0 == y)
        throw y;

    return x / y;
}

int div_nothrow (int x, int y)
{
    if (0 == y)
        return 0;

    return x / y;

}

typedef int cfun_t (int, int);


}   // extern "C"


int main (int argc, char *argv[])
{
    const int x = argc < 2 ? 0 : int (*argv [1]);
    const int y = argc < 3 ? 0 : int (*argv [2]);

    cfun_t* pcfun = argc < 4 ? div_throw : div_nothrow;

    int result;

    try {
        result = extern_c_function (x, y, pcfun);
    }
    catch (int res) {
        result = res;
    }

    return result;
}
