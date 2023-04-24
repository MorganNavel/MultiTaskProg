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

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Utilisation: %s <fichier IPC> <clé IPC>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int key = ftok(argv[1], atoi(argv[2]));
  if (key == -1) {
    perror("(ftok) ");
    return EXIT_FAILURE;
  }

  int sem = semget(key, 0, 0600);
  if (sem == -1) {
    perror("(semget) ");
    return EXIT_FAILURE;
  }

  int memory = shmget(key, 0, 0600);
  if (memory == -1) {
    perror("(shmget) ");
    return EXIT_FAILURE;
  }

  if (semctl(sem, 1, IPC_RMID) == -1) {
    perror("(semctl) ");
    return EXIT_FAILURE;
  }

  if (shmctl(memory, 0, IPC_RMID) == -1) {
    perror("(shmctl) ");
    return EXIT_FAILURE;
  }

  printf("La mémoire partagée et le sémaphore ont été supprimés\n");

  return EXIT_SUCCESS;
}
