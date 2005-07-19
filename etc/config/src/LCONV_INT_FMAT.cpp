// checking for C99 international lconv members

#include <locale.h>

int main ()
{
    lconv lc;

    lc.int_frac_digits    = 0;
    lc.int_p_cs_precedes  = 0;
    lc.int_n_cs_precedes  = 0;
    lc.int_p_sep_by_space = 0;
    lc.int_n_sep_by_space = 0;
    lc.int_p_sign_posn    = 0;
    lc.int_n_sign_posn    = 0;

    (void)&lc;

    return 0;
}
