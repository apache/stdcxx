#include <strstream>
#include <string>
#include <cassert>

int main ()
{
    std::string str ("abcdefghijk");

    char buf[10];
    std::ostrstream os (buf, sizeof(buf));
    const std::streamsize width = 2;

    os.width (width);
    os.exceptions (std::ios_base::failbit | std::ios_base::badbit);

    try {
        os << str;
    }
    catch (std::ios_base::failure&) {
    }

    assert (0 == os.width ());
    return 0;
}
