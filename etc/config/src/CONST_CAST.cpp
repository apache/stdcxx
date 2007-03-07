// checking for const_cast

const char *s1 = 0;
char *s2 = const_cast<char*>(s1);
const char *s3 = const_cast<const char*>(s2);
