#include <stdlib.h>

#include "physfsfgets.h"

char * PHYSFS_fgets(char * str, int size, PHYSFS_file * f)
{
    int i = 0;
    char c;

    if(size <= 0 || PHYSFS_eof(f))
        return NULL;

    do
    {
        if (PHYSFS_readBytes(f, &c, 1) < 1)
            break;

        str[i] = c;
        ++i;
    } while(c != '\n' && i < size - 1);

    str[i] = '\0';
    if (i == 0)
        return NULL;
    else
        return str;
}
