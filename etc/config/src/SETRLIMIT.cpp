// checking for setrlimit() in <sys/resource.h>

#include <sys/resource.h>   // for setrlimit()

void foo ()
{
    struct rlimit rl = { 0, 0 };
    setrlimit (RLIMIT_DATA, &rl);
}
