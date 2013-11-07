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

struct inf
{
    int **mat1;
    int **mat2;
    int **mat3;
    int n;
    int m;
    int line;
};

void *thread_multiply_matrix(void *para);

void print_matrix_file(int fd, int** matrix, int n);

char* read_file(char*);

struct inf structure_fulling(struct inf one, char* text, int m);

// AP: оформите передачу данных для работы потоков через аргументы как и возврат результата их работы, т.е. mat11 и т.д. не должны находиться в статической памяти

int main(int argc, char** argv, char** envp)
{
    if (argc > 2)
    {
        printf("Invalid number of parameters\n");
        exit(EXIT_FAILURE);
    }
    
    char *end;
    int m;
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
    
    // AP: оформите создание и заполнение матриц также в отдельной функции NN: structure_fulling
    
    int i = 0, j, count = 0, n;
    
    struct inf pass[m];
    for (j = 0; j < m; j++)
        pass[j] = structure_fulling(pass[j], text, m);
    
    n = pass[0].n;
    
    pthread_t thid[m];
    int **matrix;
    matrix = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        matrix[i] = (int*) malloc (n*sizeof(int));

    if (m > n)
    {
        printf("Too much threads for my algorithm. I'll use only %d threads.\n", n);
        m = n;
    }
    
    
    time_t launch_time = time(NULL);
    
    for (count = 0; count < m; count++)
    {
        int result;
        pass[count].line = count;
        pass[count].mat3 = matrix;
        result = pthread_create( &thid[count], (pthread_attr_t *)NULL, thread_multiply_matrix, (void *) (pass + count));
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
    if (close(1) < 0)
    {
        printf("Cannot close standard output\n");
        exit(EXIT_FAILURE);
    }
    
    if ((fd = open("./result.txt", O_RDWR | O_CREAT | O_EXCL, 0666)) < 0)
    {
        printf("Cannot create a file or it already exists\n");
        exit(EXIT_FAILURE);
    }

    printf("Matrix 1:\n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf("%6d", pass[0].mat1[i][j]);
        printf("\n");
    }
    
    printf("Matrix 2:\n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf("%6d", pass[0].mat2[i][j]);
        printf("\n");
    }
    
    printf("Result of multiplication:\n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf("%6d", matrix[i][j]);
        printf("\n");
    }

    printf("Multiplication time(in seconds): %d\n", delta);
    
    free(matrix);

    return 0;
}


void *thread_multiply_matrix(void *para) //function for thread
{
    struct inf *arg = (struct inf*) para;
    int line  = (*arg).line;
    int n = (*arg).n;
    int m = (*arg).m;
    int j, h, k;
    // AP: так делать некорректно - никто вам не гарантирует что инициализация thid будет первее начала выполнения потока  - передавать в качестве аргумента (см. другой комментарий про аргументы)
    for (j = line; j < n; j = j + m)
    {
        for(h = 0; h < n; h++)
        {
            for(k = 0; k < n; k++)
            {
                arg->mat3[j][h] += arg->mat1[j][k] * arg->mat2[k][h];
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
    // AP: вы дос их пор не разобрались с этим костылем? NN: Еще нет.
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

struct inf structure_fulling(struct inf one, char* text, int m)
{
    int i = 0, j, count = 0, n;
    while (text[i] != '\n')
    {
        i++;
    }
    n = sqrt(i);
    
    one.mat1 = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        one.mat1[i] = (int*) malloc (n*sizeof(int));
    
    one.mat2 = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        one.mat2[i] = (int*) malloc (n*sizeof(int));
    
    one.mat3 = (int**) malloc (n*sizeof(int*));
    for (i = 0; i < n; i++)
        one.mat3[i] = (int*) malloc (n*sizeof(int));
    
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            char *end;
            char num[1];
            num[0] = text[count];
            if ((one.mat1[i][j] = strtol(num, &end, 10)) == 0)
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
            if ((one.mat2[i][j] = strtol(num1, &end1, 10)) == 0)
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
    
    one.n = n;
    one.m = m;

    return one;

}

