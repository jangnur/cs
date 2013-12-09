#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "fdesc.h"

#define MAX_SIZE 128
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824

void file_mod_to_str(char* buf, mode_t mode)
{
    if (S_ISLNK(mode))
    {
        strcpy(buf, "SLINK");
    }
    else if (S_ISDIR(mode))
    {
        strcpy(buf, "DIR");
    }
    else if (S_ISFIFO(mode))
    {
        strcpy(buf, "FIFO");
    }
    else if (S_ISREG(mode))
    {
        strcpy(buf, "REG");
    }
    else
    {
        strcpy(buf, "OTHER");
    }
}

void format_size(unsigned int size)
{
    if (size < KILOBYTE)
    {
        printf("S: [%u Bytes].\n", size);
    }
    else if (size < MEGABYTE)
    {
        printf("S: [%.2f Kilobytes].\n", (double)size / KILOBYTE);
    }
    else if (size < GIGABYTE)
    {
        printf("S: [%.2f Megabytes].\n", (double)size / MEGABYTE);
        
    }
    else
    {
        printf("S: [%.2f Gigabytes].\n", (double)size / GIGABYTE);
    }
}

int read_dir_data(const char* path)
{
    sem_t* sem = sem_open(SEM_CHR_ID, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED)
        return 5;
    sem_wait(sem);

    int fd = open(path, O_RDONLY, 0666);
    if (fd < 0)
        return 2;
    
    struct stat fs; //stat for size
    fstat(fd, &fs);
    
    int count = fs.st_size / sizeof(fdesc); //amount of entries
    
    void* mm = mmap(NULL, fs.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (mm == 0)
        return 2;
    fdesc* fdsc = (fdesc*) mm;
    
    char buf[MAX_SIZE];
    int i;
    
    for(i = 0; i < count; ++i)
    {
        printf("N: [%s].\n", fdsc->file_name);
        printf("O: [%s].\n", fdsc->file_owner);
        printf("G: [%s].\n", fdsc->file_owner_group);
        
        file_mod_to_str(buf, fdsc->file_mode);
        
        printf("M: [%s].\n", buf);
        // AP: а если размер большой? распечатайте его, если нужно, и в Mb и в Gb //NN: Сделала. Функция format_size.
        unsigned int size;
        size = (unsigned int)fdsc->file_size;
        format_size(size);
        
        char *p = ctime(&(fdsc->file_creation_time));
        
        printf("CT: %s", ctime(&fdsc->file_creation_time));
        printf("MT: %s", ctime(&fdsc->file_modification_time));
        
        printf("\n");
        ++fdsc;
    }
    
    if (close(fd) < 0)
        return 3;
    
    if (sem_close(sem) < 0)
        return 3;
    return 0;
}


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: dir_printer in _file.\n");
        return -1;
    }
    
    int res = read_dir_data(argv[1]);
    if (res != 0)
    {
        printf("Failed to read directory data from file %s. Error: %d.\n",argv[1],res);
    }
    return res;
}

