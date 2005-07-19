// checking for __PRETTY_FUNCTION__

const char* foo ()
{
    const char *func = __PRETTY_FUNCTION__;

    return func;
}
