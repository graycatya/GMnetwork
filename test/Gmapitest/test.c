#include "CatGmapi.h"

int main()
{
    FILE* fd = Cat_Openfile("./test.c", "r");
    FILE* fdw = Cat_Openfile("./testm.txt", "w");
    const long int len = Cat_Filesize(fd);
    printf("len = %ld\n", len);
    char buffer[len];
    Cat_Readfile(buffer, sizeof(char), len, fd);
    printf("buffer = %s\n", buffer);
    Cat_Writfile(buffer, sizeof(char), len, fdw);
    Cat_Closefile(fd);
    Cat_Closefile(fdw);
    return 0;
}