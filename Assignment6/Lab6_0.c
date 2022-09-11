#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

typedef struct __myarg_t
{
    int a;
    char *b;
} myarg_t;

void *mythread(void *arg)
{
    myarg_t *m = (myarg_t *)arg;
    printf("%d %s\n", m->a, m->b);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t p;
    myarg_t args;
    myarg_t *m;
    args.a = 10;
    args.b = "hello world";
    pthread_create(&p, NULL, mythread, &args);
    pthread_join(p, (void **)&m);
    // TO ACCOMPLISH THE TASK WITHOUT pthread_join
    // while (1)
    // {
    //     sleep(0);
    // }
    return 1;
}