// checking for operator->()

struct A
{
    int a;
};

template<class T>
struct B
{
    T* operator->() const {
        static A a;
        return &a;
    }
};


int foo () 
{ 
    B<int> b;   // <-- testing

    B<A>   ba;

    (void)&b;
    (void)&ba;

    return ba->a;
}
