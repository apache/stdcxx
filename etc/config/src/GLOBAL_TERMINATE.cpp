// checking for ::terminate()


// looking for a definition in compiler support library
void terminate ();

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1)
        terminate ();

    return 0;
}
