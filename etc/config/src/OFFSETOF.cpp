// checking for a working offsetof() in <stddef.h>

#include <stddef.h>

struct S
{
    int i [2][2];
};

int main ()
{
    return !(offsetof (S, S::i [0][1]) != offsetof (S, S::i [1][0]));
}
