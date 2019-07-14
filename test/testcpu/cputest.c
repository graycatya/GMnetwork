#include <unistd.h>
#include <stdio.h>

int main()
{
    
    int counter;
    alarm(1);
    for (counter=0; 1; counter++)
        printf("counter=%d\n", counter);
    return 0;
}