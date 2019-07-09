#include "CatGmapi.h"

int main()
{
    FILE* fd = Cat_Openfile("./test.c", "r");
    printf("len = %ld\n", Cat_Filesize(fd));
    Cat_Closefile(fd);
    return 0;
}