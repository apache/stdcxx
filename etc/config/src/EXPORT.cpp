// checking for exported templates

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


#ifndef _RWSTD_NO_EXPORT_KEYWORD

// establish a dependency on EXPORT_KEYWORD.cpp which must be
// compiled first since it defines the exported function template
// LDOPTS = ./EXPORT_KEYWORD.o 
#endif   // _RWSTD_NO_EXPORT_KEYWORD


export template <class T>
T exported_function_template (T);

export template <class T>
struct ExportedClass
{
    T exported_class_member (T);
};

template <class T>
T ExportedClass<T>::
exported_class_member (T t) {
    return exported_function_template (t);
}


struct MyClass {
    int i_;
    MyClass (): i_ (0) { }
};


int main ()
{
    MyClass x;

    return ExportedClass<MyClass>().exported_class_member (x).i_;
}
