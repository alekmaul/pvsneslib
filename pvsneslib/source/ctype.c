int isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

int isxdigit(int c)
{
    if (isdigit(c) ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F'))
        return 1;
    else return 0;
}

int islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else
        return 0;
}

int toupper(int c)
{
    if (islower(c))
        return c - ('a'-'A');
    else
        return c;
}

int isspace(int c)
{
    if (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
        return 1;
    else
        return 0;
}

int isprint(int c)
{
    if (c >= ' ' && c <= '~')
        return 1;
    else
        return 0;
}
