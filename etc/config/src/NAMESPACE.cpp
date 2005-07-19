// checking for namespaces

namespace A {

struct AA { };

}

namespace B {

void AA (A::AA) { }

struct BB { };

}

namespace C {

void AA (A::AA) { }   // scope resoolution operator

void BB (B::BB) { }   // and namespace scope

namespace D {   // nested namespace

    struct C_D_DD { };

    }

void CC (C::D::C_D_DD) { }

}
