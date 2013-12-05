#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("Invalid usage: directory file\n");
        exit(EXIT_FAILURE);
    }
    
    int result = fork();
    if (result < 0)
    {
        printf("Cannot fork\n");
        exit(EXIT_FAILURE);
    }
    else if (result == 0)
    {
        int len = strlen(argv[1]) + strlen (argv[2]) + strlen("./dir_read  ");
        char *sys;
        sys = (char*)malloc(len*sizeof(char));
        strcat(sys, "./dir_read ");
        strcat(sys, argv[1]);
        strcat(sys, " ");
        strcat(sys, argv[2]);
        
        if (execl("/bin/bash", "bash", "-c", sys, NULL) < 0)
        {
            printf("Cannot execute\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int len = strlen(argv[2]) + strlen("./dir_print ");
        char *sys;
        sys = (char*)malloc(len*sizeof(char));
        
        strcat(sys, "./dir_print ");
        strcat(sys, argv[2]);

        if (execl("/bin/bash", "bash", "-c", sys, NULL) < 0)
        {
            printf("Cannot execute\n");
            exit(EXIT_FAILURE);
        }

    }
    
    
    return 0;
}
