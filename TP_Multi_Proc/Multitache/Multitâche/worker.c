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
    printf("Utilisation: %s <fichier IPC> <clé IPC> <id> <nb "
           "zones>\n",
           argv[0]);

    return EXIT_FAILURE;
  }

  long id = (long)atoi(argv[3]);
  long zones = (long)atoi(argv[4]);
  int image[zones * ZONE_SIZE];

  int key = ftok(argv[1], atoi(argv[2]));
  if (key == -1) {
    perror("(ftok) ");
    return EXIT_FAILURE;
  }

  int sem = semget(key, zones, 0600);
  if (sem == -1) {
    perror("(semget) ");
    return EXIT_FAILURE;
  }

  int memory = shmget(key, sizeof(image), 0600);
  if (memory == -1) {
    perror("(shmget) ");
    return EXIT_FAILURE;
  }

  printf("(Travailleur %lu) Hello tout le monde.\n", id);

  for (int zoneActuel = 0; zoneActuel < zones; zoneActuel++) {
    struct sembuf op;
    op.sem_flg = 0;
    op.sem_op = -id; //je prélève exactement le même nombre de jetons que mon id
    op.sem_num = zoneActuel;

    if (semop(sem, &op, 1) == -1) {
      perror("(semop) ");
      return EXIT_FAILURE;
    }

    printf("(Travailleur %lu) Je travaille en zone %i.\n", id, zoneActuel);
    fflush(stdout);
    int *image;
    if ((image = (int *)shmat(memory, NULL, 0)) == (int *)-1) {
      perror("(shmat) ");
      return EXIT_FAILURE;
    }

    // Le travail
    srand(getpid() + time(NULL));
    usleep(rand() / (float)RAND_MAX * 1e6);
    for (int i = ZONE_SIZE * zoneActuel; i < ZONE_SIZE * zoneActuel + ZONE_SIZE;
         i++) {
      image[i]++;
    }

    if (shmdt(image) == -1) {
      perror("(shmdt) ");
      return EXIT_FAILURE;
    }

    op.sem_op = id + 1;
    printf("(Travailleur %lu) Je place %lu jetons au sémaphore %i.\n", id,
           id + 1, zoneActuel + 1); //je sais que le prochain sera exactement le processus d'id +1
    fflush(stdout);
    if (semop(sem, &op, 1) == -1) {
      perror("(semop) ");
      return EXIT_FAILURE;
    }
    //Je passe à la zone suivante
  }

  return EXIT_SUCCESS;
}
