
#include <cassert>
#include <iosfwd>
#include <strstream>

int main ()
{
    std::strstreambuf sb;
    std::streampos pos = sb.pubseekoff (10, std::ios::beg, std::ios::in);
    assert (-1 == pos);
    return 0;
}

