// checking initialization of static template data memebers

template <class T>
struct S
{
    static T a;
    static T b;
};

template <class T>
T S<T>::a;        // <-- NOT initialized

template <class T>
T S<T>::b = 0;    // <-- explicitly initialized

int main ()
{
    // g++ on DEC OSF and IBM AIX fails to link
    return S<char>::a + S<char>::b;
}
