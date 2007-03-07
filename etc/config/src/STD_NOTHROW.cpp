// checking for std::nothrow


// will fail if namespaces aren't supported
namespace std {

struct nothrow_t { };

extern const nothrow_t nothrow;

}   // std


// force a failure by using the same name for an object of a diferent
// type than std:nothrow_t if namespace std isn't honored
const std::nothrow_t *nothrow = &std::nothrow;


int main (int argc, char *argv[])
{
    (void)&argv;

    // foil clever optimizers
    if (argc > 2)
        nothrow = 0;

    // will fail at link-time if std::nothrow is not defined
    // in the compiler support library
    return !(0 != nothrow);
}
