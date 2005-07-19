// checking for scope of for-init-statements

int foo (int n)
{
    int result = 0;

    // 6.5.3, p3, the scope of `i' extends to the end of
    // the for-statement (and not beyond)
    for (int i = 0; i != n; ++i)
        result += i;

    for (int i = 0; i != n; ++i)
        result += i * i;

    return result;
}
