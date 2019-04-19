#define N 10
#define READTIMES 100000
#define WRITETIMES 10000

#include "csapp.h"

sem_t common_sem, w; /* Both initially 1 */

void *read_thread(void *vargp);
void *write_thread(void *vargp);

int main() {
	pthread_t readers[N];
	pthread_t writers[N];
 	sem_init(&common_sem, 0, N);
	sem_init(&w, 0, 1);

	int i;
	for (i=0;i<N;i++) {
		Pthread_create(&readers[i], NULL, read_thread, (void *)i);
		Pthread_create(&writers[i], NULL, write_thread, (void *)i);
	}

	for (i=0;i<N;i++) { 
		Pthread_join(readers[i], NULL);
		Pthread_join(writers[i], NULL);
	}
}

void *read_thread(void *vargp)
{   
    for (int i=0;i<READTIMES;i++) {
        P(&common_sem);
        
        /* Reading happens here */
        printf("Reader (%d) is reading, i=%d\n", (int)vargp, i);
	if (i==READTIMES-1)
		printf("Reader (%d) finished!\n", (int)vargp);        

        V(&common_sem);
    }
}

void *write_thread(void *vargp)
{
    int j;
    for (int i=0;i<WRITETIMES;i++) {
        P(&w);
		for (j=0;j<N;j++) 
			P(&common_sem);
		V(&w);

        /* Writing here */
        printf("writing...i=%d\n", i);
		if(i==WRITETIMES-1)
			printf("Writer (%d) finished!\n", (int)vargp);
	
		for (j=0;j<N;j++)
        	V(&common_sem);
    }
}
