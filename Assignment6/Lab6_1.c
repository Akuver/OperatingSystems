#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define MAX 100
int arr[MAX];
int counter;
pthread_mutex_t lock;
void *worker_thread(void *args)
{
    pthread_mutex_lock(&lock);
    counter++;
    for (int i = 0; i < MAX; i++)
    {
        if (arr[i] == -1)
        {
            arr[i] = rand() % MAX + 1;
            printf("Thread %d here. Modifying index %d with the value %d\n", counter, i, arr[i]);
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[])
{
    srand(time(0));
    pthread_t p[MAX];
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
    for (int i = 0; i < MAX; i++)
    {
        pthread_join(p[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    // for (int i = 0; i < MAX; i++)
    //     printf("%d ", arr[i]);
    return 1;
}