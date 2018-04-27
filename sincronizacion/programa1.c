
/* universidad de Los andes
 * sincronización de procesos
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

void shmem_init(shmem_data *);
void show_mon(shmem_data *);
void exit_signal(int);

int main() {
  system("clear");
  printf("\n\t_______________________  monitor de procesos _______________________\n\n");

  key_t id_shmem = ftok(ROUTE, ID);
  void * pto_shmem;
  shmem_data *pto_inf;
  int i = 0, shmem;
  signal(2, exit_signal);

  int sem;

  /* creación del semaforo controlador de procesos,
   * solo se aceptaran 4 procesos a la vez en el monitor
   */
  if ((sem  = semget(SEM_ID, 1, IPC_CREAT | 0644)) < 0) {
    perror("\tsemget");
    return(-1);
  }

  /** inicialización del semaforo **/
  semctl(sem, 0, SETVAL, 4);

  /** creación del segmento de memoria compartida **/
  if((shmem = shmget(id_shmem, sizeof(shmem_data), IPC_CREAT | 0666)) < 0)
  {
		perror("\tshmget");
		exit(EXIT_FAILURE);
  }

  /** vinculación al segmento **/
	if ((pto_shmem = shmat(shmem, NULL, 0)) == (char *) -1)
	{
		perror("\tshmat");
		exit(EXIT_FAILURE);
	}

  /** inicialización **/
  pto_inf = (shmem_data *) pto_shmem;
  shmem_init(pto_inf);

  while(1)
  {
    show_mon(pto_inf);
    usleep(100000);
  }

  return(0);
}


void shmem_init(shmem_data *pto_inf)
{
  int i=0;
	pto_inf->pid_mon = getpid();
	for(i; i<10; i++)
	{
	  pto_inf->array_p[i].pid = 0;
	  pto_inf->array_p[i].numero = 0;
	  pto_inf->array_p[i].termino = 0;
	}
}

void show_mon(shmem_data *pto_inf)
{
  int i=0;
  system("clear");
  printf("\n\t_______________________  monitor de procesos %d _______________________\n\n",pto_inf->pid_mon);
  printf("\t\t pid\tnúmero\ttermino\n");
  printf("\t\t-------------------------\n");
  for(i; i<10; i++)
  {
    if(pto_inf->array_p[i].pid != 0)
    {
      printf(" \t\t%d\t%d\t", pto_inf->array_p[i].pid,pto_inf->array_p[i].numero);
      fflush(stdout);
      if(pto_inf->array_p[i].termino == 0)
        printf("\tno \n");
      else
        printf("\tsi \n");

      fflush(stdout);
    }
  }
}

void exit_signal(int num_signal)
{
  int i = 0, shmem;
  key_t id_shmem = ftok(ROUTE, ID);

  int sem;
  if ((sem  = semget(SEM_ID, 1, 0644)) < 0) {
    perror("\tsemget");
    exit(EXIT_FAILURE);
  }

  if (semctl(sem, 0, IPC_RMID, 0) == -1)
  {
    perror("\terror al eliminar el semaforo");
    exit(EXIT_FAILURE);
  }


  if((shmem = shmget(id_shmem, sizeof(shmem_data), 0666)) < 0)
  {
		perror("\tshmget");
		exit(EXIT_FAILURE);
	}

	if (shmctl(shmem, IPC_RMID, 0) < 0)
	{
		perror("\tshmctl");
		exit(EXIT_FAILURE);
	}

	system("clear");
	printf("\thasta luego!\n");
  exit(EXIT_SUCCESS);
}
