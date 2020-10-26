#include"crc32.h"

static unsigned int table[256];

void crc32_init()
{
    unsigned int i,j,k;
    for( i = 0; i < 256;i++ )
    {
        j = i;
        for( k = 0;k < 8;k++)
        {
            if( j % 2 != 0 )
                j = (j / 2) ^ 0xEDB88320;
            else
                j /= 2;
        }
        table[ i ] = j;
    }
}
int     crc32_encode( char *str,unsigned int len )
{
    unsigned int i,j,crc32sum;
    unsigned char ch;

    crc32sum = 0xffffffff;
    for( i = 0;i < len;i++)
    {
        ch = str[ i ];
        j = (crc32sum ^ ch ) & 0xff;
        crc32sum = ((crc32sum >> 8 ) & 0xffffff) ^ table[ j ];
    }
    return ~crc32sum;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   