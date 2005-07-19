// checking for const member initializer

struct A
{
    static const int i = 1;

    enum E { e0, e1 };

    static const E e = e0;
    static const E f = e1;
};

const int A::i;

const A::E A::e;
const A::E A::f;


enum F { f2 = 2, f3 = 3 };

template <class T>
struct B
{
    static const int i = 2;

    static const F e = f2;
    static const F f = f3;
};

template <class T>
const int B<T>::i;

template <class T>
const F B<T>::e;

template <class T>
const F B<T>::f;


int main ()
{
    int result = 0;

    if (1 != A::i)
        result = result << 1 + 1;

    if (A::e0 != A::e)
        result = result << 1 + 1;

    if (A::e1 != A::f)
        result = result << 1 + 1;

    if (2 != B<int>::i)
        result = result << 1 + 1;

    if (f2 != B<char>::e)
        result = result << 1 + 1;

    if (f3 != B<void>::f)
        result = result << 1 + 1;
    
    return result;
}
