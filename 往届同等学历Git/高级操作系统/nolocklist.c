#define N 100
#define Total 100000000

#include "csapp.h"

typedef struct __node_t { // basic node structure
  int             key;
  struct __node_t *next;
} node_t;

typedef struct __list_t { // basic list structure
  node_t          *head;
//  pthread_mutex_t lock;
} list_t;

list_t mylist;

/* return 1 when swapping happens;
   retnrun 0 when not*/
int CAS(unsigned long *dst, unsigned long oldVal, unsigned long newVal)
{
    unsigned char ret;
    //printf("(%lu,%lu,%lu)", *dst, oldVal, newVal);
    __asm__ __volatile__ (
            "  lock cmpxchgq %2,%1\n"
            "  sete %0\n" // ret=zf (0:oldVal!=*dst ; 1:oldVal==*dst)
            : "=q" (ret), "=m" (*dst)
            : "r" (newVal), "m" (*dst), "a" (oldVal)
            : "memory");
    //printf("->(%lu,%lu,%lu):%d\r\n", *dst, oldVal, newVal, ret);
    //above assembly returns 0 in case of failure
    if (ret) 
        return 1;
    return 0;
}


void List_Init(list_t *L) {
   L->head = NULL;
//   pthread_mutex_init(&L->lock, NULL);
}

void printList(list_t *L) {
   node_t* p = L->head;
   printf("!! p init = %d\r\n", (int)p);
   while(p!=NULL) {
      printf("p->key = %d\n", p->key);
      p = p->next;
   }
}

int getListLen(list_t *L) {
   int len = 0;
   node_t* p = L->head;
   while(p != NULL) {
      p = p->next;
      len++;
   }
   return len;
}

void *worker(void *argp) {
   for (int i=0;i<Total/N;i++) 
      List_Insert(&mylist, i);
   //printList(&mylist);
}

void main() {

   List_Init(&mylist);
   pthread_t workers[N];
   int i;


   for (i=0;i<N;i++) {
      Pthread_create(&workers[i], NULL, worker, (void *)i);
   }
   for (i=0;i<N;i++) {
      Pthread_join(workers[i], NULL);
   }
   int len = getListLen(&mylist);
   printf("list len = %d\n", len);
}

void List_Insert(list_t *L, int key) {
   // synchronization not needed
   node_t *new = malloc(sizeof(node_t));
   if (new == NULL) {
      perror("malloc");
      return;
   }
   new->key = key; 
   
   // just lock critical section
   //pthread_mutex_lock(&L->lock);
   int tmp;
   do {
      new->next = L->head;
      tmp = CAS(&(L->head), new->next, new);
   }while(tmp == 0);
   //printf("CurNode(%lu, %d, %lu)\r\n", new, new->key, new->next);
   //printList(L);
   //L->head = new;
   //pthread_mutex_unlock(&L->lock);
}


int List_Lookup(list_t *L, int key) {
   int rv = -1;
   //pthread_mutex_lock(&L->lock);
   node_t *curr = L->head;
   while (curr) {
      if (curr->key == key) {
         rv = 0;
         break;
      }
      curr = curr->next;
   }
   //pthread_mutex_unlock(&L->lock);
   return rv;
}


