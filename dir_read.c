#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pwd.h>
#include <grp.h>

#include "fdesc.h"

int read_dir(const char* dir_path, const char* out_path)
{
    DIR* dir = opendir(dir_path);
    if (dir == NULL)
        return 1;
    
    int count = 0;
    while (readdir(dir) != NULL)
        ++count;
    closedir(dir);
    
    sem_t* sem = sem_open(SEM_CHR_ID, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED)
        return 5;
    
    int fd = open(out_path, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
        return 2;

    // AP: а можно ли здесь использовать ftruncate?
    lseek(fd, count*sizeof(fdesc) + 1, SEEK_SET); //making it large enough to hold the information
    // AP: а зачем вы держите открытым файл до конца программы?
    write(fd, "", 1);
    lseek(fd, 0, SEEK_SET);
    
    void* mm = mmap(NULL, count*sizeof(fdesc), PROT_WRITE, MAP_SHARED, fd, 0);
    if (mm == 0)
        return 2;
    
    dir = opendir(dir_path);
    if(dir == NULL)
        return 1;
    
    char buffer[PATH_MAX + 1];
    struct stat fs;
    struct dirent *dent = NULL;
    fdesc* fdsc = (fdesc*) mm;
    
    int i;
    for(i = 0; i < count; ++i)
    {
        dent = readdir(dir);
        strcpy(fdsc->file_name,dent->d_name); //copy name
        strcpy(buffer, dir_path);
        strcat(buffer, "/");
        strcat(buffer, dent->d_name);
        lstat(buffer, &fs);
        
        fdsc->file_owner[0] = '\0'; //owner
        struct passwd* pwd = getpwuid(fs.st_uid);
        if( pwd != NULL)
        {
            strcpy(fdsc->file_owner,pwd->pw_name);
        }
        
        fdsc->file_owner_group[0] = '\0'; //group
        struct group* grp = getgrgid(fs.st_gid);
        if (grp != NULL)
        {
            strcpy(fdsc->file_owner_group, grp->gr_name);
        }
        
        fdsc->file_mode = fs.st_mode; //mode
        fdsc->file_size = fs.st_size; //size
        fdsc->file_creation_time = fs.st_ctime; //creation time
        fdsc->file_modification_time = fs.st_mtime; //modification time
        
        ++fdsc;
    }
    
    if (close(fd) < 0)
        return 3;
    if (munmap(mm,count*sizeof(fdesc)) < 0)
        return 4;
    sem_post(sem);
    return 0;
}


int main(int argc, char** argv)
{
    if (argc < 3) 
    {
        printf("Invalid usage: dir_reader directory out_file.\n");
        return -1;
    }
    
    int res = read_dir(argv[1], argv[2]);
    if (res != 0)
    {
        printf("Failed to read directory %s into file %s. Error: %d.\n", argv[1], argv[2], res);
    }
    return res;
}

