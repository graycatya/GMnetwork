#pragma once

#include "../Catdef.h"


FILE* Cat_Openfile(const char* path, const char* mocde);
size_t Cat_Readfile(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t Cat_Writfile(const void *ptr, size_t size, size_t nmemb, FILE *stream);
long int Cat_Filesize(FILE *stream);
void Cat_Closefile(FILE *stream);
