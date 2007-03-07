// checking for std::unexpected()


// will fail if namespaces aren't supported
namespace std {

// looking for a definition in compiler support library
void unexpected ();

}

// will cause an error if namespace std isn't being honored
int unexpected ();

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1)
        std::unexpected ();

    return 0;
}
