#ifndef INSTANTIATION_BEFORE_DEFINITION_INCLUDED
#define INSTANTIATION_BEFORE_DEFINITION_INCLUDED

template <class T>
T instantiated_before_defined (T);

template <class T>
struct InstantiatedBeforeDefined
{
    T defined_first (T t) {
        return instantiated_first (t);
    }

    // defined later, lexically after the explicit
    // instantiation directives below, to exercise
    // (for example) gcc 4.0 bug #623
    T instantiated_first (T);
};

#if defined (INSTANTIATE_TEMPLATE)
template int instantiated_before_defined (int);
template struct InstantiatedBeforeDefined<int>;
#endif   // INSTANTIATE_TEMPLATE

#if defined (INCLUDE_CC_FILE)
#  include "instantiation_before_definition.cc"
#endif   // INCLUDE_CC_FILE

#endif   // INSTANTIATION_BEFORE_DEFINITION_INCLUDED
