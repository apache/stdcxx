#include <strstream>
#include <string>
#include <cassert>

void test (const std::string& str, const std::streamsize width)
{
    char buf[10];
    std::ostrstream os (buf, sizeof(buf));

    os.width (width);
    os.exceptions (std::ios_base::failbit | std::ios_base::badbit);

    try {
        os << str;
    }
    catch (std::ios_base::failure&) {
    }

#ifndef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
    assert (width == os.width ());
#else
    assert (0 == os.width ());
#endif
}

int main ()
{
    std::string str ("abcdefghijk");

    test (str, 2);
    test (str, str.size () + 2);

    return 0;
}
