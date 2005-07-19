// checking default template arguments

#include "config.h"

template <class T, class U = int>
struct A { };

template <class T = char, class U = double>
struct B { };

template <class T = char, class U = T>
struct C { };

template <class T = A <short, float>, class U = C <T> >
struct D { };

A<void> a;
B<>     b;
C<>     c;
D<>     d;
