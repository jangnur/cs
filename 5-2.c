#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define NBYTES 16
#define MAXDIGITS 16

int **mat1 = NULL;
int **mat2 = NULL;
int **mat3 = NULL;
int m, n;
pthread_t *thid;

void *thread_multiply_matrix(void *);

void print_matrix_file(int fd, int** matrix, int n);

char* read_file(char*);

int main(int argc, char** argv, char** envp)
{
    if (argc > 2)
    {
        printf("Invalid number of parameters\n");
        exit(EXIT_FAILURE);
    }
    
    char *end;
    if ((m = strtol(argv[1], &end, 10)) == 0)
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
    
    if (m <= 0)
    {
        printf("Cannot create zero or negative number of threads\n");
        exit(EXIT_FAILURE);
    }
    
    char path[] = "./data.txt";
    char *text;
    text = read_file(path);
    
    int i = 0, j, count = 0;
    while (text[i] != '\n')
    {
        i++;
    }
    n = sqrt(i);
    
    mat1 = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        mat1[i] = (int*) malloc (n*sizeof(int));
    
    mat2 = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        mat2[i] = (int*) malloc (n*sizeof(int));
    
    mat3 = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        mat3[i] = (int*) malloc (n*sizeof(int));
    
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            char num[1];
            num[0] = text[count];
            if ((mat1[i][j] = strtol(num, &end, 10)) == 0)
            {
                if (errno == EINVAL)
                {
                    printf("Invalid number %d: check the input\n", count);
                    exit(EXIT_FAILURE);
                }
                
                if (errno == ERANGE)
                {
                    printf("The number %d is out of range\n", count);
                    exit(EXIT_FAILURE);
                }
            }
            count++;
        }
    }
    
    count++;
    
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            char *end1;
            char num1[1];
            num1[0] = text[count];
            if ((mat2[i][j] = strtol(num1, &end1, 10)) == 0)
            {
                if (errno == EINVAL)
                {
                    printf("Invalid number %d: check the input\n", count);
                    exit(EXIT_FAILURE);
                }
                
                if (errno == ERANGE)
                {
                    printf("The number %d is out of range\n", count);
                    exit(EXIT_FAILURE);
                }
            }
            count++;
        }
    }
    
    if (m > n)
    {
        printf("Too much threads for my algorithm. I'll use only %d threads.\n", n);
        m = n;
    }
    
    thid = (pthread_t*) malloc (m*sizeof(pthread_t));
    
    time_t launch_time = time(NULL);
    
    for (count = 0; count < m; count++)
    {
        int result;
        result = pthread_create( &thid[count], (pthread_attr_t *)NULL, thread_multiply_matrix, NULL);
        if(result != 0)
        {
            printf ("Error on thread create, return value = %d\n", result);
            exit(-1);
        }
        
    }
    
    for (count = 0; count < m; count++)
    {
        if (pthread_join(thid[count], (void**) NULL) > 0)
        {
            printf("Cannot join the thread %d\n", count);
            exit(EXIT_FAILURE);
        }
    }
    
    time_t end_time = time(NULL);
    int delta = (end_time - launch_time);
    
    int fd;
    int size;
    (void)umask(0);
    if ((fd = open("./result.txt", O_RDWR | O_CREAT | O_EXCL, 0666)) < 0)
    {
        printf("Cannot create a file or it already exists\n");
        exit(EXIT_FAILURE);
    }
    
    if (size = write(fd, "Matrix 1:\n", strlen("Matrix 1:\n")) < 0)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }
    
    print_matrix_file(fd, mat1, n);
    
    if (size = write(fd, "Matrix 2:\n", strlen("Matrix 2:\n")) < 0)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }
    
    print_matrix_file(fd, mat2, n);
    
    if (size = write(fd, "The result of multiplication:\n", strlen("The result of mutiplication:\n")) < 0)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }
    
    print_matrix_file(fd, mat3, n);
    
    if (size = write(fd, "Multiplication time(in seconds): ", strlen("Multiplication time(in seconds): ")) < 0)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }
    
    char* num;
    if (sprintf(num, "%d", delta) < 0)
    {
        printf("Cannot convert integer to string\n");
        exit(EXIT_FAILURE);
    }
    
    if (size = write(fd, num, strlen(num)) < 0)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }

    if (close(fd) < 0)
    {
        printf("Cannot close the file\n");
    }
    
    free(text);
    free(mat1);
    free(mat2);
    free(mat3);
    free(thid);
    
    if (execl("/bin/rm", "rm", "data.txt", NULL) < 0)
    {
        printf("Cannot delete the file\n");
        exit(EXIT_FAILURE);
    }
        
    
    return 0;
}


void print_matrix_file(int fd, int** matrix, int n) //print a single matrix in the file
{
    size_t size;
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            char *num;
            num = (char*) malloc (MAXDIGITS*sizeof(char));
            if (sprintf(num, "%6d", matrix[i][j]) < 0)
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
        
        if (size = write(fd, "\n", strlen("\n")) < 0)
        {
            printf("Cannot write to file\n");
            exit(EXIT_FAILURE);
        }

    }
    
}

void *thread_multiply_matrix(void *para) //function for thread
{
    pthread_t mythid = pthread_self();
    int i = 0, j, h, k;
    
    while (thid[i] != mythid)
    {
        i++;
    }
    
    for (j = i; j < n; j = j + m)
    {
        for(h = 0; h < n; h++)
        {
            for(k = 0; k < n; k++)
            {
                mat3[j][h] += mat1[j][k] * mat2[k][h];
            }
        }
    }
    
    return NULL;
}

char* read_file(char* path)   //read from file
{
    int fd;
    (void)umask(0);
    if ((fd = open(path, O_RDONLY, 0666)) < 0)
    {
        printf("Cannot read the file\n");
        exit(EXIT_FAILURE);
    }
    
    char *text = NULL;
    size_t size;
    char buffer[NBYTES];
    int total = 14;         //forgive me for this, this is only for computer's realloc to work. Actually total = 0.
    while (size = read(fd, buffer, NBYTES))
    {
        if (size < 0)
        {
            printf("Cannot read the file\n");
            exit(EXIT_FAILURE);
        }
        total = total + size;
        text = (char*)realloc(text, total*sizeof(char));
        strncat(text, buffer, size);
    }
    
    if (close(fd) < 0)
    {
        printf("Cannot close the file\n");
        exit(EXIT_FAILURE);
    }
    
    return text;

}

