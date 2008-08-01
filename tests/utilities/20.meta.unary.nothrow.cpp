/***************************************************************************
 *
 * 20.meta.unary.prop.cpp - test exercising meta.unary.prop,
 *                          has_nothrow_xxx traits
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 **************************************************************************/

#include <rw_driver.h>

#ifndef _RWSTD_NO_EXT_CXX_0X

// only compele when C++ 0x extensions are enabled

#include <type_traits>

#define THROW   public: enum { value = 0 }
#define NOTHROW public: enum { value = 1 }

struct A {
    struct A1 { NOTHROW; /* trivial copy ctor */ };
    struct A2 { NOTHROW; A2 (A2&) throw (); };
    struct A3 { NOTHROW; A3 (const A3&) throw (); };
    struct A4 { NOTHROW; A4 (volatile A4&) throw (); };
    struct A5 { NOTHROW; A5 (const volatile A5&) throw (); };

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    // move ctor overloads (move ctor is not a copy ctor)
    struct A6 { NOTHROW; A6 (A6&&) throw (); };
    struct A7 { NOTHROW; A7 (const A6&&) throw (); };
    struct A8 { NOTHROW; A8 (volatile A6&&) throw (); };
    struct A9 { NOTHROW; A9 (const volatile A6&&) throw (); };

#endif   // _RWSTD_NO_RVALUE_REFERENCES

};


struct B {
    struct B1 { THROW; B1 (B1&); };
    struct B2 { THROW; B2 (const B2&); };
    struct B3 { THROW; B3 (volatile B3&); };
    struct B4 { THROW; B4 (const volatile B4&); };

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    // move ctor overloads (move ctor is not a copy ctor)
    struct B5 { NOTHROW; B5 (B5&&); };
    struct B6 { NOTHROW; B6 (const B6&&); };
    struct B7 { NOTHROW; B7 (volatile B7&&); };
    struct B8 { NOTHROW; B8 (const volatile B8&&); };

#endif   // _RWSTD_NO_RVALUE_REFERENCES

};


struct C {
    // same as A::A* but with inaccessible copy ctor

    class C1 { NOTHROW; private: /* trivial copy ctor */ };
    class C2 { NOTHROW; private: C2 (C2&) throw (); };
    class C3 { NOTHROW; private: C3 (const C3&) throw (); };
    class C4 { NOTHROW; private: C4 (volatile C4&) throw (); };
    class C5 { NOTHROW; private: C5 (const volatile C5&) throw (); };

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    // move ctor overloads (move ctor is not a copy ctor)
    class C6 { NOTHROW; private: C6 (C6&&) throw (); };
    class C7 { NOTHROW; private: C7 (const C7&&) throw (); };
    class C8 { NOTHROW; private: C8 (volatile C8&&) throw (); };
    class C9 { NOTHROW; private: C9 (const volatile C9&&) throw (); };

#endif   // _RWSTD_NO_RVALUE_REFERENCES

};


struct D {
    // same as B::B* but with inaccessible copy ctor

    class D1 { NOTHROW; /* trivial copy ctor */ };
    class D2 { NOTHROW; D2 (D2&) throw (); };
    class D3 { NOTHROW; D3 (const D3&) throw (); };
    class D4 { NOTHROW; D4 (volatile D4&) throw (); };
    class D5 { NOTHROW; D5 (const volatile D5&) throw (); };

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    // move ctor overloads (move ctor is not a copy ctor)
    class D6 { NOTHROW; D6 (D6&&) throw (); };
    class D7 { NOTHROW; D7 (const D7&&) throw (); };
    class D8 { NOTHROW; D8 (volatile D8&&) throw (); };
    class D9 { NOTHROW; D9 (const volatile D9&&) throw (); };

#endif   // _RWSTD_NO_RVALUE_REFERENCES

};


struct E {
    // both const and non-const copy ctors

    struct E1 {
        NOTHROW;
        E1 (E1&) throw () { }
        E1 (const E1&) throw () { }
    };

    struct E2 {
        THROW;
        E2 (E2&) throw () { }
        E2 (const E2&);
    };

    struct E3 {
        THROW;
        E3 (E3&);
        E3 (const E3&) throw () { }
    };

    struct E4 {
        THROW;
        E4 (E4&);
        E4 (const E4&);
    };
};


struct F {
    // non-const, const, and volatile copy ctors

    struct F1 {
        NOTHROW;
        F1 (F1&) throw () { }
        F1 (const F1&) throw () { }
        F1 (volatile F1&) throw () { }
    };

    struct F2 {
        THROW;
        F2 (F2&) throw () { }
        F2 (const F2&) throw () { }
        F2 (volatile F2&);
    };

    struct F3 {
        THROW;
        F3 (F3&) throw () { }
        F3 (const F3&);
        F3 (volatile F3&) throw () { }
    };

    struct F4 {
        THROW;
        F4 (F4&);
        F4 (const F4&) throw () { }
        F4 (volatile F4&) throw () { }
    };
};


struct G {
    // non-const, const, volatile, and const volatile copy ctors

    struct G1 {
        NOTHROW;
        G1 (G1&) throw ();
        G1 (const G1&) throw ();
        G1 (volatile G1&) throw ();
        G1 (const volatile G1&) throw ();
    };

    struct G2 {
        THROW;
        G2 (G2&) throw ();
        G2 (const G2&) throw ();
        G2 (volatile G2&) throw ();
        G2 (const volatile G2&);
    };

    struct G3 {
        THROW;
        G3 (G3&) throw ();
        G3 (const G3&) throw ();
        G3 (volatile G3&);
        G3 (const volatile G3&) throw ();
    };

    struct G4 {
        THROW;
        G4 (G4&) throw ();
        G4 (const G4&);
        G4 (volatile G4&) throw ();
        G4 (const volatile G4&) throw ();
    };

    struct G5 {
        THROW;
        G5 (G5&);
        G5 (const G5&) throw ();
        G5 (volatile G5&) throw ();
        G5 (const volatile G5&) throw ();
    };
};


struct H {
    // non-const, const, volatile, and const volatile copy ctors

    struct H1 {
        NOTHROW;
        H1 (H1&) throw ();
        H1 (const H1&) throw ();
        H1 (volatile H1&) throw ();
        H1 (const volatile H1&) throw ();
    };

    struct H2 {
        THROW;
        H2 (H2&) throw ();
        H2 (const H2&) throw ();
        H2 (volatile H2&) throw ();
        H2 (const volatile H2&);
    };

    struct H3 {
        THROW;
        H3 (H3&) throw ();
        H3 (const H3&) throw ();
        H3 (volatile H3&);
        H3 (const volatile H3&) throw ();
    };

    struct H4 {
        THROW;
        H4 (H4&) throw ();
        H4 (const H4&);
        H4 (volatile H4&) throw ();
        H4 (const volatile H4&) throw ();
    };

    struct H5 {
        THROW;
        H5 (H5&);
        H5 (const H5&) throw ();
        H5 (volatile H5&) throw ();
        H5 (const volatile H5&) throw ();
    };
};


struct I {
    // non-const, const, volatile, and const volatile copy ctors
    // all with second argument

    struct I1 {
        NOTHROW;
        I1 (I1&, int = 0) throw ();
        I1 (const I1&, int = 0) throw ();
        I1 (volatile I1&, int = 0) throw ();
        I1 (const volatile I1&, int = 0) throw ();
    };

    struct I2 {
        THROW;
        I2 (I2&, int = 0) throw ();
        I2 (const I2&, int = 0) throw ();
        I2 (volatile I2&, int = 0) throw ();
        I2 (const volatile I2&, int = 0);
    };

    struct I3 {
        THROW;
        I3 (I3&, int = 0) throw ();
        I3 (const I3&, int = 0) throw ();
        I3 (volatile I3&, int = 0);
        I3 (const volatile I3&, int = 0) throw ();
    };

    struct I4 {
        THROW;
        I4 (I4&, int = 0) throw ();
        I4 (const I4&, int = 0);
        I4 (volatile I4&, int = 0) throw ();
        I4 (const volatile I4&, int = 0) throw ();
    };

    struct I5 {
        THROW;
        I5 (I5&, int = 0);
        I5 (const I5&, int = 0) throw ();
        I5 (volatile I5&, int = 0) throw ();
        I5 (const volatile I5&, int = 0) throw ();
    };
};


struct J {
    // non-const, const, volatile, and const volatile copy ctors
    // plus a template ctor (a template ctor is never a copy ctor)

    struct J1 {
        NOTHROW;
        J1 (J1&) throw ();
        J1 (const J1&) throw ();
        J1 (volatile J1&) throw ();
        J1 (const volatile J1&) throw ();
        template <class T> J1 (T); // not a copy ctor
    };

    struct J2 {
        NOTHROW;
        J2 (J2&) throw ();
        J2 (const J2&) throw ();
        J2 (volatile J2&) throw ();
        J2 (const volatile J2&) throw ();
        template <class T> J2 (const T&); // not a copy ctor
    };

    struct J3 {
        NOTHROW;
        J3 (J3&) throw ();
        J3 (const J3&) throw ();
        J3 (volatile J3&) throw ();
        J3 (const volatile J3&) throw ();
        template <class T> J3 (volatile T&); // not a copy ctor
    };

    struct J4 {
        NOTHROW;
        J4 (J4&) throw ();
        J4 (const J4&) throw (){ }
        J4 (volatile J4&) throw ();
        J4 (const volatile J4&) throw ();
        template <class T> J4 (const volatile T&); // not a copy ctor
    };
};


struct K {
    // nothrow non-const, const, volatile, and const volatile copy
    // ctors plus a throwing non-copy ctor

    struct K1 {
        NOTHROW;
        K1 (K1&) throw ();
        K1 (const K1&) throw ();
        K1 (volatile K1&) throw ();
        K1 (const volatile K1&) throw ();
        template <class T> K1 (T) throw (); // not a copy ctor
    };

#ifndef _RWSTD_NO_VARIADIC_TEMPLATES

    struct K2 {
        NOTHROW;
        K2 (K2&) throw ();
        K2 (const K2&) throw ();
        K2 (volatile K2&) throw ();
        K2 (const volatile K2&) throw ();
        template <class ...T> K2 (T...); // not a copy ctor
    };

#endif   // _RWSTD_NO_VARIADIC_TEMPLATES

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    struct K3 {
        NOTHROW;
        K3 (K3&) throw ();
        K3 (const K3&) throw ();
        K3 (volatile K3&) throw ();
        K3 (const volatile K3&) throw ();
        K3 (K3&&); // not a copy ctor
    };

#endif   // _RWSTD_NO_RVALUE_REFERENCES

    struct K4 {
        NOTHROW;
        K4 (K4&) throw ();
        K4 (const K4&) throw ();
        K4 (volatile K4&) throw ();
        K4 (const volatile K4&) throw ();
        K4 (...); // not a copy ctor
    };

    struct K5 {
        NOTHROW;
        K5 (K5&) throw ();
        K5 (const K5&) throw ();
        K5 (volatile K5&) throw ();
        K5 (const volatile K5&) throw ();
        K5 (int); // not a copy ctor
    };
};

struct L {

    struct L1 {
        THROW;
        // member non-const copy ctor may throw
        struct L1_1 { L1_1 (L1_1&); } l1_1;
    };

    struct L2 {
        THROW;
        // member const copy ctor may throw
        struct L2_1 { L2_1 (const L2_1&); } l2_1;
    };

    struct L3 {
        THROW;
        // member volatile copy ctor may throw
        struct L3_1 { L3_1 (volatile L3_1&); } l3_1;
    };

    struct L4 {
        THROW;
        // member const volatile copy ctor may throw
        struct L4_1 { L4_1 (const volatile L4_1&); } l4_1;
    };

    struct L5 {
        NOTHROW;
        // member non-const copy ctor may throw
        // but same copy ctor of parent class doesn't
        L5 (L5&) throw ();
        struct L5_1 { L5_1 (L5_1&); } l5_1;
    };

    struct L6 {
        NOTHROW;
        // member const copy ctor may throw
        // but same copy ctor of parent class doesn't
        L6 (const L6&) throw ();
        struct L6_1 { L6_1 (const L6_1&); } l6_1;
    };

    struct L7 {
        NOTHROW;
        // member volatile copy ctor may throw
        // but same copy ctor of parent class doesn't
        L7 (volatile L7&) throw ();
        struct L7_1 { L7_1 (volatile L7_1&); } l7_1;
    };

    struct L8 {
        NOTHROW;
        // member const volatile copy ctor may throw
        // but same copy ctor of parent class doesn't
        L8 (const volatile L8&) throw ();
        struct L8_1 { L8_1 (const volatile L8_1&); } l8_1;
    };

    struct L9 {
        NOTHROW;
        // member const copy ctor may throw
        // but non-const copy ctor of parent class doesn't
        L9 (L9&) throw ();
        struct L9_1 { L9_1 (const L9_1&); } l9_1;
    };

    struct L10 {
        NOTHROW;
        // member non-const copy ctor may throw
        // but const copy ctor of parent class doesn't
        L10 (const L10&) throw ();
        struct L10_1 { L10_1 (L10_1&); } l10_1;
    };

    struct L11 {
        NOTHROW;
        L11 (L11&) throw ();
        L11 (const L11&) throw ();

        struct L11_1 {
            L11_1 (L11_1&) throw (int);
            L11_1 (const L11_1&) throw (int);
        } l11_1;
    };

    struct L12 {
        NOTHROW;
        L12 (const L12&) throw ();

        struct L12_1 {
            L12_1 (L12_1&) throw (int);
            L12_1 (const L12_1&) throw (int);
            L12_1 (volatile L12_1&) throw (int);
            L12_1 (const volatile L12_1&) throw (int);
        } l12_1;
    };

    struct L13 {
        NOTHROW;
        L13 (const L13&) throw ();

        struct L13_1 {
            L13_1 (L13_1&) throw ();
            L13_1 (const L13_1&) throw ();
            L13_1 (volatile L13_1&) throw ();
            L13_1 (const volatile L13_1&) throw ();
        } l13_1;

        struct L13_2 {
            L13_2 (L13_2&) throw ();
            L13_2 (const L13_2&) throw ();
            L13_2 (volatile L13_2&) throw ();
            L13_2 (const volatile L13_2&) throw (int);   // may throw
        } l13_2;
    };

    struct L14 {
        NOTHROW;
        L14 (const L14&) throw ();

        struct L14_1 {
            L14_1 (L14_1&) throw ();
            L14_1 (const L14_1&) throw ();
            L14_1 (volatile L14_1&) throw ();
            L14_1 (const volatile L14_1&) throw ();
        } l14_1;

        struct L14_2 {
            L14_2 (L14_2&) throw ();
            L14_2 (const L14_2&) throw ();
            L14_2 (volatile L14_2&) throw (int);    // may throw
            L14_2 (const volatile L14_2&) throw ();
        } l14_2;
    };

    struct L15 {
        NOTHROW;
        L15 (const L15&) throw ();

        struct L15_1 {
            L15_1 (L15_1&) throw ();
            L15_1 (const L15_1&) throw ();
            L15_1 (volatile L15_1&) throw ();
            L15_1 (const volatile L15_1&) throw ();
        } l15_1;

        struct L15_2 {
            L15_2 (L15_2&) throw ();
            L15_2 (const L15_2&) throw (int);   // may throw
            L15_2 (volatile L15_2&) throw ();
            L15_2 (const volatile L15_2&) throw ();
        } l15_2;
    };

    struct L16 {
        NOTHROW;
        L16 (const L16&) throw ();

        struct L16_1 {
            L16_1 (L16_1&) throw ();
            L16_1 (const L16_1&) throw ();
            L16_1 (volatile L16_1&) throw ();
            L16_1 (const volatile L16_1&) throw ();
        } l16_1;

        struct L16_2 {
            L16_2 (L16_2&) throw (int);   // may throw
            L16_2 (const L16_2&) throw ();
            L16_2 (volatile L16_2&) throw ();
            L16_2 (const volatile L16_2&) throw ();
        } l16_2;
    };
};


static int
test_has_nothrow_copy ()
{
#ifndef _RWSTD_TT_HAS_NOTHROW_COPY

    rw_warn (0, 0, __LINE__,
             "test_has_nothrow_copy() disabled because "
             "_RWSTD_TT_HAS_NOTHROW_COPY is not defined");

#else

    // exercise the has_nothrow_copy_constructor trait
#define TEST(T)                                                         \
    rw_assert (std::has_nothrow_copy_constructor<T>::value == T::value, \
               0, __LINE__,                                             \
               "std::has_nothrow_copy_constructor<%s>::value == %d", #T, \
               std::has_nothrow_copy_constructor<T>::value)

    TEST (A::A1);
    TEST (A::A2);
    TEST (A::A3);
    TEST (A::A4);
    TEST (A::A5);
#ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST (A::A6);
    TEST (A::A7);
    TEST (A::A8);
    TEST (A::A9);
#endif

    TEST (B::B1);
    TEST (B::B2);
    TEST (B::B3);
    TEST (B::B4);
#ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST (B::B5);
    TEST (B::B6);
    TEST (B::B7);
    TEST (B::B8);
#endif

    TEST (C::C1);
    TEST (C::C2);
    TEST (C::C3);
    TEST (C::C4);
    TEST (C::C5);
#ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST (C::C6);
    TEST (C::C7);
    TEST (C::C8);
    TEST (C::C9);
#endif

    TEST (D::D1);
    TEST (D::D2);
    TEST (D::D3);
    TEST (D::D4);
    TEST (D::D5);
#ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST (D::D6);
    TEST (D::D7);
    TEST (D::D8);
    TEST (D::D9);
#endif

    TEST (E::E1);
    TEST (E::E2);
    TEST (E::E3);
    TEST (E::E4);

    TEST (F::F1);
    TEST (F::F2);
    TEST (F::F3);
    TEST (F::F4);

    TEST (G::G1);
    TEST (G::G2);
    TEST (G::G3);
    TEST (G::G4);
    TEST (G::G5);

    TEST (H::H1);
    TEST (H::H2);
    TEST (H::H3);
    TEST (H::H4);
    TEST (H::H5);

    TEST (I::I1);
    TEST (I::I2);
    TEST (I::I3);
    TEST (I::I4);
    TEST (I::I5);

    TEST (J::J1);
    TEST (J::J2);
    TEST (J::J3);
    TEST (J::J4);

    TEST (K::K1);
#  ifndef _RWSTD_NO_VARIADIC_TEMPLATES
    TEST (K::K2);
#  endif
#  ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST (K::K3);
#  endif
    TEST (K::K4);
    TEST (K::K5);

    TEST (L::L1);
    TEST (L::L2);
    TEST (L::L3);
    TEST (L::L4);
    TEST (L::L5);
    TEST (L::L6);
    TEST (L::L7);
    TEST (L::L8);
    TEST (L::L9);
    TEST (L::L10);
    TEST (L::L11);
    TEST (L::L12);
    TEST (L::L13);
    TEST (L::L14);
    TEST (L::L15);
    TEST (L::L16);

#endif

    return 0;
}

static int
run_test (int, char*[])
{
    test_has_nothrow_copy ();

    return 0;
}

/**************************************************************************/

#else // _RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

static int run_test (int, char*[])
{
    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

int main (int argc, char*argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "meta.unary.prop",
                    "std::has_nothrow_xxx traits",
                    run_test,
                    0);
}
