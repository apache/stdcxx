// checking for std::initializer_list<T>

#include <initializer_list>

// see if the initializer_list feature is implemented
int main ()
{
    std::initializer_list<int> il = { 1, 2, 3, 4 };

    if (4 == il.size () && 1 == *il.begin () && 4 == *(il.end () - 1))
        return 0;   // success

    return 1;   // failure
}
