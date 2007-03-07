// checking for static_cast

void foo (void *p, char *s, int i, double d)
{
    p = static_cast<void*>(s);
    s = static_cast<char*>(p);
    i = static_cast<int>(d);
    d = static_cast<double>(i);
}
