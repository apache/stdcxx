// checking for newline (CR or CR-LF)

#include <stdio.h>

int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_NEWLINE\n");

#endif   // _RWSTD_USE_CONFIG

    // determine the byte sequence that corresponds to the '\n'
    // source character, i.e., one of
    //   <CR>     -- Apple II and Mac OS
    //   <LF>     -- UNIX systems
    //   <CR><LF> -- CP/M, DOS, Windows
    //   <LF><CR> -- unknown

    char fname [256];
    if (0 == tmpnam (fname))
        return 1;

    FILE *fp = fopen (fname, "w");
    if (0 == fp)
        return 2;

    fprintf (fp, "%s", "a\nb");
    fclose (fp);

    fp = fopen (fname, "rb");
    if (0 == fp)
        return 3;

    char buffer [32] = "";

    size_t nbytes = fread (buffer, 1, sizeof buffer, fp);

    fclose (fp);
    remove (fname);

    char *pc;

    for (pc = buffer; *pc && *pc != '\n'; ++pc);

    if ('\0' == *pc) {

        for (pc = buffer; *pc && *pc != '\r'; ++pc);

        if ('\0' == *pc) {

#if 'A' == '\x41'   // ASCII

            // no newline?
            return 4;
            

#elif 'A' == '\xc1'   // EBCDIC

            
            for (pc = buffer; *pc && *pc != '\x15'; ++pc);

            if ('\0' == *pc) {
                // no newline?
                return 4;
            }

            printf ("#define _RWSTD_NEWLINE \"\\x15\"\n");
            printf ("#define _RWSTD_NEWLINE_NEL\n");

#endif   // ASCII or EBCDIC

        }
        else {

            printf ("#define _RWSTD_NEWLINE \"\\r\"\n");
            printf ("#define _RWSTD_NEWLINE_CR\n");
        }
    }
    else {
        if ('\r' == pc [1]) {
            printf ("#define _RWSTD_NEWLINE \"\\n\\r\"\n");
            printf ("#define _RWSTD_NEWLINE_LF_CR\n");
        }
        if ('\r' == pc [-1]) {
            printf ("#define _RWSTD_NEWLINE \"\\r\\n\"\n");
            printf ("#define _RWSTD_NEWLINE_CR_LF\n");
        }
        else {
            printf ("#define _RWSTD_NEWLINE \"\\n\"\n");
            printf ("#define _RWSTD_NEWLINE_LF\n");
        }
    }

    return 0;
}
