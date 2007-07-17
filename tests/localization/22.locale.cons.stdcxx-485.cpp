#include <cassert>
#include <locale>

int main (int argc, char *argv[])
{
    for (int j = 0; j != 2; ++j) {
        std::locale locales [64];

        for (int i = 1; i < argc; ++i)
            locales [i] = std::locale (argv [i]);

        for (int i = 1; i < argc; ++i)
            assert (std::has_facet<std::collate<char> >(locales [i]));
    }
}
