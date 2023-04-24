#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define ZONE_SIZE 100000

int main(int argc, char *argv[]) {
  if (argc != 5) {
    printf("Utilisation: %s <fichier IPC> <clé IPC> <nb travailleurs> <nb "
           "zones>\n",
           argv[0]);

    return EXIT_FAILURE;
  }

  int travailleurs = atoi(argv[3]);
  int zones = atoi(argv[4]);
  int image[zones * ZONE_SIZE];

  int key = ftok(argv[1], atoi(argv[2]));
  if (key == -1) {
    perror("(ftok) ");
    return EXIT_FAILURE;
  }

  int sem = semget(key, zones, IPC_CREAT | IPC_EXCL | 0600);
  if (sem == -1) {
    perror("(semget) ");
    return EXIT_FAILURE;
  }

  int memory = shmget(key, sizeof(image), IPC_CREAT | IPC_EXCL | 0600);
  if (memory == -1) {
    perror("(shmget) ");
    return EXIT_FAILURE;
  }

  ushort count[zones];
  for (int i = 0; i < zones; i++)
    count[i] = 1;
  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } arg;
  arg.array = count;
  if (semctl(sem, zones, SETALL, arg) == -1) {
    printf("(semctl) %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("(Main) J'attends que tout le monde finisse (Qu'il y ait %i jetons en "
         "%i).\n",
         travailleurs + 1, zones - 1);
  struct sembuf op;
  op.sem_flg = 0;
  op.sem_num = zones - 1;
  op.sem_op = -travailleurs - 1;
  if (semop(sem, &op, 1) == -1) {
    perror("(semop) ");
    return EXIT_FAILURE;
  }
  printf("(Main) Adieu tout le monde!");
  fflush(stdout);

  if (semctl(sem, 1, IPC_RMID) == -1) {
    perror("(semctl) ");
    return EXIT_FAILURE;
  }

  if (shmctl(memory, 0, IPC_RMID) == -1) {
    perror("(shmctl) ");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
