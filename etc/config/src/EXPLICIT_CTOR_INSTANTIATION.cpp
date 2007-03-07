// checking for explicit instantiation of ctors

template <class T>
struct S
{
    S () { }

    S (const S&) { }
};

template S<int>::S ();
template S<double>::S ();

template S<char>::S (const S<char>&);
template S<unsigned>::S (const S<unsigned>&);
