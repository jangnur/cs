#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


// AP: у вас много мест, где повторяется один и тот же код обработки ошибки - оформите его в виде функции
void write_matrices(int fd, int n);

void print_error(int n);

int main(int argc, char** argv, char** envp)
{
    if (argc > 2)
    {
        print_error(0);
    }
    
    int n;
    char *end;
    if ((n = strtol(argv[1], &end, 10)) == 0)
    {
        if (errno == EINVAL)
        {
            print_error(1);
        }
        
        if (errno == ERANGE)
        {
            print_error(2);
        }
    }
    
    if (n <= 0)
    {
        print_error(3);
    }
    
    int fd;
    (void)umask(0);
    if ((fd = open("./data.txt", O_RDWR | O_CREAT | O_EXCL, 0666)) < 0)
    {
        print_error(4);
    }
    
    write_matrices(fd, n);
    
    if (close(fd) < 0)
    {
        print_error(5);
    }
    
    return 0;

}

// AP: sprintf - нестандартная функция - замените ее
// AP: вы не проверяете write на возможность записи меньше, чем ожидается и лучше записывать формируемую стоку за раз,
// а не по-символьно - перепишите

void write_matrices(int fd, int n)
{
    char* text;
    int length  = 4*n*n;
    text = (char*) malloc (length*sizeof(char));
    int i, j, count = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            srand(time(NULL));
            int randnum = random() % 10;
            char dig = (char)(((int)'0') + randnum);
            text[count] = dig;
            count++;
        }        
    }
    
    text[count] = '\n';
    count++;
    
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            srand(time(NULL));
            int randnum = random() % 10;
            char dig = (char)(((int)'0') + randnum);
            text[count] = dig;
            count++;
        }
    }
    
    size_t size;

    if (size = write(fd, text, strlen(text)) != strlen(text))
    {
        if (size < 0)
        {
            print_error(6);
        }
        else
        {
            print_error(7);
        }
    }
    
    free(text);

}

void print_error(int n)
{
    if (n == 0)
    {
        printf("Invalid number of parameters\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 1)
    {
        printf("Invalid number: check the input\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 2)
    {
        printf("The number is out of range\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 3)
    {
        printf("Incorrect input. Cannot create matrix with zero or negative size.\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 4)
    {
        printf("Cannot create a file or it already exists\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 5)
    {
        printf("Cannot close the file\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 6)
    {
        printf("Cannot write to file\n");
        exit(EXIT_FAILURE);
    }
    else if (n == 7)
    {
        printf("Cannot write all string to file\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Unknown error\n");
        exit(EXIT_FAILURE);
    }
}
