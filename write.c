#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define KEY 1234
#define KEY2 2345

int main() {

    int sem_id=semget(KEY,1,0644);
    int * numero;

    struct sembuf semmmy;
    semmmy.sem_op = -1;
    semmmy.sem_flg = SEM_UNDO;
    semmmy.sem_num = 0;
    semop(sem_id, &semmmy,1);

    int fd=open("story.txt",O_RDWR | O_APPEND,0644);

    if (fd == -1) {
      printf("Error while opening %s\n",strerror(errno));
      semmmy.sem_op = 1;
      semop(sem_id, &semmmy,1);
      return 0;
    }

    int shmid=shmget(KEY2,sizeof(int),IPC_CREAT | IPC_EXCL | 0644);

    char laline[256];
    if (shmid==-1) {
        shmid=shmget(KEY2,sizeof(int),IPC_CREAT | 0644);
        numero=shmat(shmid,0,0);
        lseek(fd,-1*(*numero),SEEK_END);
        read(fd,laline,*numero);
        printf("Last line:\n%s",laline);
    }
    else {
        printf("Last line: N/A\n");
        numero=shmat(shmid,0,0);
    }
    char input[256];
    printf("What would you like the next line to be? ");
    fgets(input, 256, stdin);
    *numero=strlen(input);
    shmdt(numero);
    write(fd, input, strlen(input));
    close(fd);
    semmmy.sem_op = 1;
    semop(sem_id, &semmmy, 1);
    return 0;
}
