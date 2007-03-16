// checking for explicit instantiation

// IMPLICIT_INSTANTIATION.cpp depends on foo<T>(T)
template <class T>
T foo (T t) { return t; }

template <class T>
struct S
{
    T bar () {
        // explicit instantiation of S<T> (and, consequently,
        // S<T>::bar()) must "implicitly" instantiate foo<T>(T)
        return foo (T ());
    }
};

// the symbols expected to be produced by the explicit instantiation
// directives and their attributes are as follows:
//     TEXT             S<int>::bar()
//     WEAK (or TEXT)   foo<int>(int)
//     TEXT             S<char>::bar()
//     WEAK (or TEXT)   foo<char>(char)

// IMPLICIT_INSTANTIATION.cpp uses foo<int>(int)
template class S<int>;
template class S<char>;
