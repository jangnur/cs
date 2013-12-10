#if !defined FDESC_H
#define FDESC_H

#include <limits.h>
#include <sys/stat.h>

#define LOGIN_NAME_MAX 255

typedef struct fdesc_
{
  char file_name[NAME_MAX + 1];
  char file_owner[LOGIN_NAME_MAX + 1];
  char file_owner_group[LOGIN_NAME_MAX + 1];
  mode_t file_mode;
  off_t file_size;
  time_t file_creation_time;
  time_t file_modification_time;
} fdesc;

#define SEM_CHR_ID "dirsem"

#endif


/* my little comment 
here I studied functions sem_open, sem_post and sem_wait which are based on functions from <sys/sem.h>
sem_open initializes and opens new semaphore (using ftok based on *pathname and then running semop)
sem_post unlocks the semaphore(increases the value by 1 and starts the execution of any streams waiting for semaphore value to change)
sem_wait locks the semaphore(check if > 0, then -1; if ==0, then process stops until >0)
I hope you will appreciate my efforts.

p.s. Sorry that I wrote this comment in English.*/
// AP: оценил :)
// NN: thank you :)

