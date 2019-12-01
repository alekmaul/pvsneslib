/*---------------------------------------------------------------------------------

	Copyright (C) 2012
		Alekmaul 

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/

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
