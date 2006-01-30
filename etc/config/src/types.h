// definitions of the type_name() helper overloads

#define STRSTR(symbol)   #symbol
#define STR(symbol)      STRSTR (symbol)
#define DEFINE_TYPE_HELPER(T, symbol)                   \
    const char* type_name (T t) {                       \
        if (t) return symbol; else return STR (T);      \
    } typedef void unused_typedef


DEFINE_TYPE_HELPER (char, "CHAR");
DEFINE_TYPE_HELPER (signed char, "SCHAR");
DEFINE_TYPE_HELPER (unsigned char, "UCHAR");

DEFINE_TYPE_HELPER (short, "SHRT");
DEFINE_TYPE_HELPER (unsigned short, "USHRT");

DEFINE_TYPE_HELPER (int, "INT");
DEFINE_TYPE_HELPER (unsigned, "UINT");

DEFINE_TYPE_HELPER (long, "LONG");
DEFINE_TYPE_HELPER (unsigned long, "ULONG");

#ifdef LONG_LONG

DEFINE_TYPE_HELPER (LONG_LONG, "LLONG");
DEFINE_TYPE_HELPER (unsigned LONG_LONG, "ULLONG");

#endif   // LONG_LONG

// for any other type
const char* type_name (...) { return 0; }
