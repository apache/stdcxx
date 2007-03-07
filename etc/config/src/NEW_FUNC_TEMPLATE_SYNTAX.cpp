// checking template specialization syntax


template <class T>
int foo (T)
{
    return 0;
}

template<>
int foo (int)
{
    return 1;
}

int bar ()
{
    return !(1 == foo (0));
}
