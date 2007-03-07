// checking partial template specialization

#include "config.h"

// primary template
template <class T, class U>
struct PartiallySpecialized
{
    enum { e = 0 };
};

// required by std::vector<bool>
template <class U>
struct PartiallySpecialized<int, U>
{
    enum { f = 1 };
};

// required by std::iterator_traits
template <class U>
struct PartiallySpecialized<int, U*>
{
    enum { g = 2 };
};

// required by std::iterator_traits
template <class U>
struct PartiallySpecialized<int, const U*>
{
    enum { h = 3 };
};

// required by std::allocator<const T>
template <class T, class U>
struct PartiallySpecialized<const T, const U>
{
    enum { i = 4 };
};

// required by __rw_is_same
// exercises bug 638 in SunPro 5.7, patch 117831-02 2005/03/30
template <class T>
struct PartiallySpecialized<T, T>
{
    enum { j = 5 };
};

// compile only, do not link
int foo ()
{
#define PS PartiallySpecialized

    return !(   0 == PS<char, int>::e
             && 1 == PS<int, char>::f
             && 2 == PS<int, char*>::g
             && 3 == PS<int, const char*>::h
             && 4 == PS<const int, const char>::i
             && 5 == PS<long, long>::j);
}
