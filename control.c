#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 1234
#define KEY2 2345

void create(){
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short * array;
        struct seminfo * __buf;
    } semaphore;
   printf("CREATING STORY\n");
   int sem_id = semget(KEY,1,IPC_CREAT | IPC_EXCL | 0600);
      if (sem_id >= 0) {
          semaphore.val = 1;
          semctl(sem_id, 0, SETVAL, semaphore.val);
          int fd=open("story.txt", O_CREAT | O_TRUNC,0644);
          if (fd == -1){
              printf("Error in opening: %s\n", strerror(errno));
              return;
          }
          close(fd);
      }
      else{
          printf("Already created.\n");
      }
}

void view(){
    printf("VIEWING STORY:\n");
    int fd = open("story.txt", O_RDONLY);
    if (fd == -1){
        printf("Error in opening: %s\n", strerror(errno));
        return;
    }
    char * file = calloc(2056, sizeof(char));
    int last = read(fd, file, 2056);
    if (last == -1){
        printf("Error in reading: %s\n", strerror(errno));
        return;
    }
    char* text = strrchr(file, '\n');
    if (text){
       *text = 0;
     }
    printf("%s\n", file);
    free(file);
    close(fd);
    return;
}

void delete(){
    printf("REMOVING STORY\n");
    view();
    remove("story.txt");
    int shmid = shmget(KEY2, sizeof(int), 0);
    int what = shmctl(shmid, IPC_RMID, NULL);
    int semid = semget(KEY, 1, 0);
    semctl(semid, 0, IPC_RMID);
    printf("Removed!\n");
}

int main(int argc, char * argv[]) {
    int sem_id;
    if (argc == 2) {
        if (!strcmp(argv[1],"-c")) {
            create();
        }
        else if (!strcmp(argv[1],"-v")) {
            view();
        }
        else if (!strcmp(argv[1],"-r")) {
            delete();
        }
        else
            printf("Invalid Input\n");
        }
    else {
        printf("Invalid Input\n");
    }
    return 0;
}
