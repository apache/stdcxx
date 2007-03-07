// checking for ::unexpected()


// looking for a definition in compiler support library
void unexpected ();

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1)
        unexpected ();

    return 0;
}
