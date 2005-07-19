// checking member class access

struct A
{
private:
    enum { e };
    static int f (int i) { return i; }

protected:
    enum { g };
    static int h (int i) { return i; }

public:
    struct AA {
        // check that the compiler implements the resolution of cwg issue 45
        // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#45
        static int ag () { return f (e); }
        static int ah (A a) { return a.f (e); }
    };
};


struct B: A
{
    struct BB {
        // exercise the same in a derived class
        static int bg () { return h (0); }
        static int bh (B b) { return b.h (b.g); }
    };
};


int foo ()
{
    return A::AA::ag () + A::AA::ah (A ()) + B::BB::bg () + B::BB::bh (B ());
}
