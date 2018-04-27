
/* universidad de Los andes
 * sincronizacion de procesos
 * asignatura: sistemas operativos
 * autor: alvaro araujo
 * fecha: 20/04/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/sem.h>
#include "memsh.h"

int shmem_init(shmem_data *);

int llamadaSemaforo(int semId, int semNum, int op)
{
  struct sembuf sops;
  sops.sem_num = semNum;
  sops.sem_op = op;
  sops.sem_flg = 0;
  return (semop(semId, &sops, 1)); /** retorna -1 en caso de error **/
}

int main()
{

  int sem;

  system("clear");
  printf("\n\t--> proceso %d \n\n",getpid());
  srand(getpid());
  key_t id_shmem = ftok(ROUTE, ID);
  void *pto_shmem;
  shmem_data *pto_inf;
  int i = 0, shmem, pos, repeticion;

  if ((sem  = semget(SEM_ID, 1, 0644)) < 0) {
    perror("\tsemget");
    exit(EXIT_FAILURE);
  }

  /* busqueda del segmento de memoria compartida */

  if((shmem = shmget(id_shmem, sizeof(shmem_data), 0666)) < 0)
  {
		perror("\tshmget");
		exit(EXIT_FAILURE);
	}

  /* Vinculacion al segmento */
	if((pto_shmem = shmat(shmem, NULL, 0)) == (char *) -1)
	{
		perror("\tshmat");
		exit(EXIT_FAILURE);
	}

	pto_inf = (shmem_data *) pto_shmem;
  pos = shmem_init(pto_inf);

  if(pos == -1)
  {
    if(shmdt(pto_shmem) == -1)
	  {
		  perror("\tshmdt");
		  exit(EXIT_FAILURE);
	  }

    printf("\t\tmonitor sin espacio!!!\n\n");
    exit(EXIT_SUCCESS);
  }

  llamadaSemaforo(sem, 0, -1);

  repeticion = rand()%(100-10+1)+10;
  for(i=0; i<repeticion; i++)
  {
    pto_inf->array_p[pos].numero++;

    printf("Numero: %d\n",i);
    usleep(400000);

  }

  pto_inf->array_p[pos].termino = 1;
  if(shmdt(pto_shmem) == -1)
	{
	  perror("\tshmdt");
	  exit(EXIT_FAILURE);
	}

  llamadaSemaforo(sem, 0, 1);
  return(0);
}

int shmem_init(shmem_data *pto_inf)
{
  int i=0;
	pto_inf->pid_mon = getpid();
	for(i; i<10; i++)
	  if(pto_inf->array_p[i].pid == 0)
	  {
	    pto_inf->array_p[i].pid = getpid();
	    return i;
	  }
	return -1;
}
