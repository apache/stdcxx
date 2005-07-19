// checking for thread-local storage

__thread int tls_i;

static __thread char tls_static_array [256];
extern __thread char tls_extern_array [256];

__thread char tls_extern_array [256];

int main ()
{
    return tls_i + tls_static_array [0] + tls_extern_array [0];
}
