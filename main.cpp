// chliu created 2014/02/07
//a tool format otool's output 
// 0018a8d0	40 28 23 29 50 52 4f 47 52 41 4d 3a 53 79 73 74
// like hexdump -C xxx.mp3 to format otool's output.
// hexdump [-s offset] [-n length] -C xxx.mp3
// 00451f20  00 00 00 00 54 41 47 cd  f2 ce ef c9 fa 20 20 20  |....TAG??????   |
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static unsigned char g_char2hex [256];

static void process  ( char* buf, int size, int bufsize )
{ 
    char* in = buf;
    char* out = buf + size;
    
    char* inEnd = out;
    char* outEnd = buf + bufsize - 1;
    char* stop = NULL;

    //skip address part
    unsigned long address = strtoul ( in, &stop, 16 );
    
    if ( (in == stop) || (0 == address) || 
         (NULL == stop) || (*stop > 0x20)  ) 
    {
        return ; // invalid line
    }
    
    in = stop + 1;
    
    //insert separate "  |"
    *out++ = ' ';
    *out++ = ' ';
    *out++ = '|';
    
    while ( out < outEnd && in < inEnd )
    {
        unsigned char v = ( g_char2hex[ in[0] ] << 4 ) | g_char2hex[  in[1] ];
        
        *out++ = ( v > 0x7f || v < 0x20 ) ? '.' : v ;
        in += 3;
    } 
    
    *out = 0;
}

static int get_line (FILE* pf, char* buf, int size )
{
    int i = 0;

    for (; i<size; ++i )
    {
        char c = fgetc (pf);
        
        if ( '\n' == c )
        {
            break;
        }
        else if ( '\r' == c )
        {
            c = getc (pf); //skip next \n
            assert ( '\n' == c );

            break;
        }
        else
        {
            if ( EOF == c )
            {
                break;
            }
            
            buf[i] = c;
        }
    } 

    return i;
}

int main (int argc, char * const argv[]) 
{
    char buf[256];
    FILE* pf = stdin;

    //0~9 -> 0x0 ~ 0x09 
    //a~f -> 0xa ~ 0x0f 
    //A~F -> 0xa ~ 0x0f 
    for ( char i = '0'; i<='9'; ++i )
    {
        g_char2hex[i] = i - '0' ;
    }
    for ( char i = 'a'; i<='f'; ++i )
    {
        g_char2hex[i] = i - 'a' + 0xa;
    } 
    for ( char i = 'A'; i<='F'; ++i )
    {
        g_char2hex[i] = i - 'A' + 0x0a;
    } 

    if ( argc > 1 )
    {
        if ( !strcmp (argv[1], "-h" ) )
        {
            fprintf (stderr, "usage: %s input_file [ or stdin] \n", argv[0] );
            return 0;
        }
        
        pf = fopen ( argv[1], "rb" );    
        if ( NULL == pf )
        {
            fprintf (stderr, "failed to open file:%s\n", argv[1] );
            return -1;
        }
    }

    while ( !feof(pf) )
    {
        int count = get_line ( pf, buf, 200 );
        if ( !count )
        {
            continue;
        } 
        
        buf[count] = 0;
        process ( buf, count, 256 );

        printf ( "%s\n", buf );
    }
    
    return 0;
}

//end
//
