// checking for specialization on return type


template <class T>
T foo () { return T (); }

template <>
int foo<int>() { return 1; }

template <>
long foo<long>() { return 2; }

int main ()
{
    int result = 0;

    if (0 != foo<char>())
        result = result << 1 + 1;

    if (1 != foo<int>())
        result = result << 1 + 1;

    if (2 != foo<long>())
        result = result << 1 + 1;

    return result;
}
