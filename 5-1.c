#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define MAXDIGITS 10

void write_matrix(int fd, int n);

int main(int argc, char** argv, char** envp)
{
    if (argc > 2)
    {
        printf("Invalid number of parameters\n");
        exit(EXIT_FAILURE);
    }
    
    int n;
    char *end;
    if ((n = strtol(argv[1], &end, 10)) == 0)
    {
        if (errno == EINVAL)
        {
            printf("Invalid number: check the input\n");
            exit(EXIT_FAILURE);
        }
        
        if (errno == ERANGE)
        {
            printf("The number is out of range\n");
            exit(EXIT_FAILURE);
        }
    }
    
    if (n <= 0)
    {
        printf("Incorrect input. Cannot create matrix with zero or negative size.\n");
        exit(EXIT_FAILURE);
    }
    
    int size;
    int fd;
    (void)umask(0);
    if ((fd = open("./data.txt", O_RDWR | O_CREAT | O_EXCL, 0666)) < 0)
    {
        printf("Cannot create a file or it already exists\n");
        exit(EXIT_FAILURE);
    }
    
    write_matrix(fd, n);
    
    if (size = write(fd, "\n", strlen("\n")) < 0)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }
    
    write_matrix(fd, n);
    
    if (close(fd) < 0)
    {
        printf("Cannot close the file\n");
    }
    
    return 0;

}

void write_matrix(int fd, int n)
{
    size_t size;
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            srand(time(NULL));
            int randnum = random() % 10;
            char *num;
            num = (char*) malloc (MAXDIGITS*sizeof(char));
            if (sprintf(num, "%d", randnum) < 0)
            {
                printf("Cannot convert integer to string\n");
                exit(EXIT_FAILURE);
            }
            
            if (size = write(fd, num, strlen(num)) < 0)
            {
                printf("Cannot write to file\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}
