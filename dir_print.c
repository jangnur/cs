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
        // AP: а если размер большой? распечатайте его, если нужно, и в Mb и в Gb
        printf("S: [%u Bytes].\n", (unsigned int)fdsc->file_size);
        
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

