#ifndef INSTANTIATION_BEFORE_DEFINITION_INCLUDED
#define INSTANTIATION_BEFORE_DEFINITION_INCLUDED

// NOTE: do not #include config.h here and avoid relying
// the config infrastructure automatically establishing
// dependencies on other config tests (headers are not
// scanned for such dependencies)

template <class T>
T instantiated_before_defined (T);

template <class T>
struct InstantiatedBeforeDefined
{
    T defined_first (T t) {
        return instantiated_first_and_referenced (t);
    }

    // defined later, lexically after the explicit instantiation directives
    // below, but not referenced by another member of this class to exercise
    // (for example) gcc 4.0 bug #623
    T instantiated_first (T);

    // same as above but also referenced by another member of this class 
    T instantiated_first_and_referenced (T) const;
};

#if defined (INSTANTIATE_TEMPLATE)

template int instantiated_before_defined (int);
template struct InstantiatedBeforeDefined<int>;

#elif defined (EXTERN_TEMPLATE)

#  if __GNUG__ >= 3
     // disable gcc 3.x (and beyond) error: ISO C++ forbids the use
     // of `extern' on explicit instantiations
#    pragma GCC system_header
#  endif   // gcc >= 3

#  ifndef SPECIALIZE_FUNCTION_TEMPLATE
extern template int instantiated_before_defined (int);
#  else   // if defined (SPECIALIZE_FUNCTION_TEMPLATE)
template<> int instantiated_before_defined (int);
#  endif   // SPECIALIZE_FUNCTION_TEMPLATE

extern template struct InstantiatedBeforeDefined<int>;

#endif   // INSTANTIATE_TEMPLATE, EXTERN_TEMPLATE

#if defined (INCLUDE_CC_FILE)
#  include "instantiation_before_definition.cc"
#endif   // INCLUDE_CC_FILE

#endif   // INSTANTIATION_BEFORE_DEFINITION_INCLUDED
