// implementation file for instantiation_before_definition.h

template <class T>
T instantiated_before_defined (T t)
{
    return t;
}

template <class T>
T InstantiatedBeforeDefined<T>::instantiated_first (T t)
{
    return instantiated_before_defined (t);
}
