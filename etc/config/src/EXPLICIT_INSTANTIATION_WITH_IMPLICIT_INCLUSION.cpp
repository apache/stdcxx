// checking for explicit instantiation with implicit inclusion

#include "instantiation_with_implicit_inclusion.h"

template class S<short>;
template long foo (S<long>, long);
template double bar (S<double>, double);
