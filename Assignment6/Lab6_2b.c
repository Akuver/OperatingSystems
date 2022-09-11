#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#define MAX 100
int arr[MAX];
int counter;
pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
typedef struct __myarg_t
{
    int ind;
} myarg_t;
void *worker_thread(void *args)
{
    pthread_mutex_lock(&lock);
    counter++;
    for (int i = 0; i < MAX; i++)
    {
        if (arr[i] == -1)
        {
            arr[i] = rand() % MAX + 1;
            //printf("Thread %d here. Modifying index %d with the value %d\n", counter, i, arr[i]);
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond);
    sleep(0);
    return NULL;
}
void *consume_thread(void *args)
{
    myarg_t *val = (myarg_t *)args;
    pthread_mutex_lock(&lock);
    while (!arr[val->ind])
        pthread_cond_wait(&cond, &lock);
    printf("Consumer thread here... Value at index %d is %d\n", val->ind, arr[val->ind]);
    pthread_mutex_unlock(&lock);
    return NULL;
}
int main(int argc, char *argv[])
{
    srand(time(0));
    pthread_t p[MAX];
    pthread_t c[10];
    myarg_t args[10];
    for (int i = 0; i < 10; i++)
    {
        args[i].ind = i;
    }
    for (int i = 0; i < MAX; i++)
        arr[i] = -1;
    if (pthread_mutex_init(&lock, NULL))
    {
        printf("mutex init failed.\n");
        return 1;
    }
    for (int i = 0; i < MAX; i++)
    {
        int ret = pthread_create(&(p[i]), NULL, &worker_thread, NULL);
        if (ret)
        {
            printf("Error: pthread_create() failed\n");
            return 1;
        }
    }
    for (int i = 0; i < 10; i++)
    {
        int ret = pthread_create(&(c[i]), NULL, &consume_thread, &args[i]);
        if (ret)
        {
            printf("Error: pthread_create() failed\n");
            return 1;
        }
    }
    for (int i = 0; i < MAX; i++)
    {
        pthread_join(p[i], NULL);
    }
    for (int i = 0; i < 10; i++)
    {
        pthread_join(c[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    // for (int i = 0; i < MAX; i++)
    //     printf("%d ", arr[i]);
    return 1;
}