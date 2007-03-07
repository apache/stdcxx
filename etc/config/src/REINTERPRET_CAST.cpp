// checking for reinterpret_cast


int main ()
{
    const int *i = reinterpret_cast<const int*>("");

    return !(0 != i);
}
