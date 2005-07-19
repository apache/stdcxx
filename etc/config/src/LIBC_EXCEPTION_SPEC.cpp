// checking for exception specification in libc

#include <stdlib.h>

extern "C" {

// careful about testing varargs functions such as sprintf()
// due to gcc bugs -- see bug #461 or #462, or functions that
// are possible cancellation points (which in GNU glibc are
// not declared with throw())

int atoi (const char*) throw ();

}   // extern "C"
