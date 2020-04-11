#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//Mendeklarasikan matriks dan isi matriks//
#define M 4
#define K 2
#define N 5
#define NUM_THREADS M * N
int (*value)[10];

int A[M][K] = {{0,1},
               {2,3},
               {4,5},
               {6,7}
              };
int B[K][N] = {{11,12,13,14,15}, 
               {16,17,18,19,10}
              };


/* Struct untuk passing data ke threads */
struct v
{
	int i; /* row */
	int j; /* column */
};

//Thread//
void *runner(void *ptr);

//Membuat shared memory//
int main(int argc, char **argv)
{
	int i, j;
	int thread_counter = 0;
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int[10][10]), IPC_CREAT | 0666);
    value = shmat(shmid, 0, 0);
    
	
	pthread_t workers[NUM_THREADS];
	
	//Membuat M*N bekerja di thread//
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++) 
		{
			struct v *data = (struct v *) malloc(sizeof(struct v));
			data->i = i;
			data->j = j;
			//Membuat thread dan mempassing data sebagai parameter//
			pthread_create(&workers[thread_counter], NULL, runner, data);
			pthread_join(workers[thread_counter], NULL);
			thread_counter++;
		}
	}
	
	//Menunggu thread selesai dikalikan//
	for (i = 0; i < NUM_THREADS; i++)
	{
	    pthread_join(workers[i], NULL);
	}
	
	for(i = 0; i < M; i++)
	{ 
		for(j = 0; j < N; j++)
		{ 
			printf("%d\t", value[i][j]);
		}
		printf("\n");
	}
	return 0;
}

//Menghitung perkalian matriks//
void *runner(void *ptr)
{
	struct v *data = ptr;
	int i, sum = 0;
	
	for(i = 0; i < K; i++)
	{	
		sum += A[data->i][i] * B[i][data->j];
	}
	
	value[data->i][data->j] = sum;
	pthread_exit(0);
}
