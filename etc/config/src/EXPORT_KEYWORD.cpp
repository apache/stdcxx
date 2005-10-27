// checking for the export keyword


// NOTE: test EXPORT.cpp links with EXPORT_KEYOWRD.o and expects
// to find a definition of the function template below there
export
template <class T>
T exported_function_template (T t)
{
    return t;
}
