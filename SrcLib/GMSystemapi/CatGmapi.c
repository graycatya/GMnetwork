#include "CatGmapi.h"


FILE* Cat_Openfile(const char* path, const char* mocde)
{
    FILE* fd = fopen(path, mocde);
    if(fd == NULL)
    {
        Cat_Errexit("Cat_Openfile errno");
    }
    return fd;
}

size_t Cat_Readfile(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t len;
    len = fread(ptr, size, nmemb, stream);
    if(len <= 0)
    {
        Cat_Errexit("Cat_Readfile errno");
    }
    if(ferror(stream) != 0)
    {
        clearerr(stream);
        Cat_Closefile(stream);
        Cat_Errexit("Cat_Readfile len errno");
    }
    return len;
}

size_t Cat_Writfile(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t len;
    len = fwrite(ptr, size, nmemb, stream);
    if(len <= 0)
    {
        Cat_Errexit("Cat_Writfile errno");
    }
    if(ferror(stream) != 0)
    {
        clearerr(stream);
        Cat_Closefile(stream);
        Cat_Errexit("Cat_Readfile len errno");
    }
    return len;
}

long int Cat_Filesize(FILE *stream)
{
    long int len;
    if(stream == NULL)
    {
        Cat_Errexit("Cat_Filesize stream is null");
    }
    fseek(stream, 0L, SEEK_END);
    len = ftell(stream);
    if(errno == EOF)
    {
        Cat_Errexit("Cat_Filesize ftell errno.");
    }
    fseek(stream, 0L, SEEK_SET);
    return len;
}

void Cat_Closefile(FILE *stream)
{
    if(stream != NULL)
    {
        if(fclose(stream) == EOF)
        {
            Cat_Errexit("Cat_Closefile errno.");
        }
    }
    else
    {
        Cat_Errexit("Cat_Closefile stream is null.");
    }
}
