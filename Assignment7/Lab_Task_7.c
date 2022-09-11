#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#define MAX 50
#define LINE_BUFFER 500
#define ARRAY_BUFFER 300
pthread_mutex_t lock;
pthread_cond_t cond[MAX];
int is_sorted[MAX];
int th_no[MAX];
void Pthread_cond_signal(int index)
{
    pthread_cond_broadcast(&cond[index - 1]);
    return;
}
int init()
{
    srand(time(0));
    FILE *fp;
    fp = fopen("arraydata.txt", "w");
    if (fp == NULL)
    {
        printf("Failed to open file.\n");
        return 1;
    }
    int i = 1;
    while (i <= MAX)
    {
        char str[5];
        sprintf(str, "%d", i);
        fputs(str, fp);
        fputs(":", fp);
        int sz = 100 + rand() % 101;
        for (int j = 0; j < sz; j++)
        {
            int number = 1 + rand() % 1000;
            sprintf(str, "%d", number);
            fputs(str, fp);
            if (j != sz - 1)
                fputs(" ", fp);
        }

        fputs("\n", fp);
        i++;
    }
    fclose(fp);
    fp = fopen("querydata.txt", "w");
    if (fp == NULL)
    {
        printf("Failed to open file.\n");
        return 1;
    }
    i = 1;
    while (i <= MAX)
    {
        char str[5];
        sprintf(str, "%d", MAX);
        fputs(str, fp);
        fputs(":", fp);
        int number = 1 + rand() % 1000;
        sprintf(str, "%d", number);
        fputs(str, fp);
        fputs("\n", fp);
        i++;
    }
    fclose(fp);
    if (pthread_mutex_init(&lock, NULL))
    {
        printf("mutex init failed.\n");
        return 1;
    }
    for (int i = 0; i < MAX; i++)
    {
        if (pthread_cond_init(&cond[i], NULL))
        {
            printf("cond init failed.\n");
            return 1;
        }
    }

    return 0;
}
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
int query(char line[], int size)
{
    int num = 0, flag = 0;
    for (int i = 0; i < size; i++)
    {
        if (line[i] == ':')
        {
            flag = 1;
            continue;
        }
        if (!flag)
            continue;
        int x = line[i] - '0';
        if (x >= 0 && x <= 9)
            num = num * 10 + x;
    }
    return num;
}
void *searching_routine(void *args)
{
    int *num = (int *)args;
    printf("%d thread begin..\n", *num);
    // read querydata here
    FILE *fp;
    fp = fopen("querydata.txt", "r");
    char line[LINE_BUFFER];
    while (fgets(line, sizeof(line), fp))
    {
        int find_number = query(line, strlen(line));
        FILE *fpn;
        fpn = fopen("arraydata.txt", "r");
        char c;
        int index;
        // read arraydata here
        do
        {
            index = 0;
            int x;
            do
            {
                c = fgetc(fpn);
                x = c - '0';
                if (x >= 0 && x <= 9)
                    index = index * 10 + x;

            } while (c != ':');
            pthread_mutex_lock(&lock);
            while (!is_sorted[index - 1])
                pthread_cond_wait(&cond[index - 1], &lock);
            pthread_mutex_unlock(&lock);
            int arr[ARRAY_BUFFER];
            int sz = 0, chars = 0;
            do
            {
                int num = 0;
                do
                {
                    c = fgetc(fpn);
                    chars++;
                    x = c - '0';
                    if (x >= 0 && x <= 9)
                        num = num * 10 + x;
                } while (x >= 0 && x <= 9);
                arr[sz++] = num;
            } while (c != '\n');

            for (int i = 0; i < sz; i++)
            {
                if (arr[i] == find_number)
                    printf("Found %d at location %d in Array %d\n", find_number, i + 1, index);
            }
        } while (index != MAX);
        fclose(fpn);
    }
    fclose(fp);
    printf("%d thread end..\n", *num);
    return NULL;
}
void *sorting_routine(void *args)
{

    FILE *fp;
    fp = fopen("arraydata.txt", "r+");
    char c;
    // file read here
    int index;
    do
    {
        index = 0;
        int x;
        do
        {
            c = fgetc(fp);
            x = c - '0';
            if (x >= 0 && x <= 9)
                index = index * 10 + x;

        } while (c != ':');
        int arr[ARRAY_BUFFER];
        int sz = 0, chars = 0;
        pthread_mutex_lock(&lock);
        do
        {
            int num = 0;
            do
            {
                c = fgetc(fp);
                chars++;
                x = c - '0';
                if (x >= 0 && x <= 9)
                    num = num * 10 + x;
            } while (x >= 0 && x <= 9);
            arr[sz++] = num;
        } while (c != '\n');
        // sort array[index]
        for (int i = 0; i < sz; i++)
        {
            int mini = 1e7, minindex;
            for (int j = i; j < sz; j++)
            {
                if (mini > arr[j])
                    mini = arr[j], minindex = j;
            }
            swap(&arr[i], &arr[minindex]);
        }
        // rewrite the array here
        fseek(fp, -chars, SEEK_CUR);
        for (int i = 0; i < sz; i++)
        {
            int write_num = arr[i];
            char str[5];
            sprintf(str, "%d", write_num);
            fputs(str, fp);
            if (i != sz - 1)
                fputs(" ", fp);
        }
        fputs("\n", fp);
        printf("Array %d is now sorted\n", index);
        is_sorted[index - 1] = 1;
        Pthread_cond_signal(index - 1);
        pthread_mutex_unlock(&lock);
    } while (index != MAX);
    fclose(fp);

    return NULL;
}
int check_ret(int ret)
{
    if (!ret)
        return 0;
    printf("pthread_create() failed!\n");
    return 1;
}
int main(int argc, char *argv[])
{
    if (init())
        return 1;

    pthread_t sorting_thread;
    pthread_t searching_thread[MAX];
    int ret;
    ret = pthread_create(&sorting_thread, NULL, sorting_routine, NULL);
    if (check_ret(ret))
        return 1;
    for (int i = 0; i < MAX; i++)
    {
        th_no[i] = i + 1;
        ret = pthread_create(&(searching_thread[i]), NULL, searching_routine, &th_no[i]);
        if (check_ret(ret))
            return 1;
    }
    pthread_join(sorting_thread, NULL);
    for (int i = 0; i < MAX; i++)
        pthread_join(searching_thread[i], NULL);

    return 0;
}