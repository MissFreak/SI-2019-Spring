#define N 10
#define M 10
#define READTIMES 100000
#define WRITETIMES 100000

#include "csapp.h"

int readcnt, writecnt;    /* Initially 0 */
sem_t r, w, rcnt, wcnt; /* Both initially 1 */

void *read_thread(void *vargp);
void *write_thread(void *vargp);

int main() {
	readcnt = 0;
	writecnt = 0;
	pthread_t readers[N];
	pthread_t writers[M];
 	sem_init(&r, 0, 1);
	sem_init(&w, 0, 1);
	sem_init(&rcnt, 0, 1);
	sem_init(&wcnt, 0, 1);

	int i;
	for (i=0;i<N;i++) 
		Pthread_create(&readers[i], NULL, read_thread, (void *)i);
	for (i=0;i<M;i++) 
		Pthread_create(&writers[i], NULL, write_thread, (void *)i);

	for (i=0;i<N;i++) 
		Pthread_join(readers[i], NULL);
	for (i=0;i<M;i++)
		Pthread_join(writers[i], NULL);
}

void *read_thread(void *vargp)
{   
    for (int i=0;i<READTIMES;i++) {
        P(&r);
		P(&rcnt);
        readcnt++;
        if (readcnt == 1) /* First in */
                P(&w);
        V(&rcnt);
		V(&r);
        
        /* Reading happens here */
        printf("Reader (%d) is reading, readcnt = %d, i=%d\n", (int)vargp, readcnt, i);
		if (i==READTIMES-1)
			printf("Reader (%d) finished!\n", (int)vargp);        

        P(&rcnt);
        readcnt--;
        if (readcnt == 0) /* Last out */
                V(&w);
        V(&rcnt);
    }
}

void *write_thread(void *vargp)
{
    for (int i=0;i<WRITETIMES;i++) {
		P(&wcnt);
		writecnt++;
		if (writecnt == 1)
			P(&r);
		V(&wcnt);

        P(&w);

        /* Writing here */
        printf("writing...i=%d\n", i);
		if(i==WRITETIMES-1)
			printf("Writer (%d) finished!\n", (int)vargp);

        V(&w);

		P(&wcnt);
		writecnt--;
		if (writecnt == 0)
			V(&r);
		V(&wcnt);
    }
}
