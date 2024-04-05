#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "a2_helper.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct
{
    int id;
} proces4;

void *process4(void *param)
{
    proces4 *s = (proces4 *)param;

    info(BEGIN, 4, s->id);

    info(END, 4, s->id);

    return NULL;
}

typedef struct
{
    int id;
    sem_t * start_thread5;
    sem_t * stop_thread4;
} proces7;

void *process7(void *param)
{
    proces7 *s = (proces7*) param;

    if (s->id == 4)
        sem_wait(s->start_thread5);

    info(BEGIN, 7, s->id);

    if (s->id == 5)
    {
        sem_post(s->start_thread5);
        sem_wait(s->stop_thread4);
    }

    info(END, 7, s->id);

    if (s->id == 4)
        sem_post(s->stop_thread4);

    return NULL;
}

typedef struct
{
    int id;
    int *threaduriRuleaza;
    pthread_mutex_t *mutex;
    sem_t *sem;
    pthread_cond_t *cond;
} proces8;

void *process8(void *param)
{
    proces8 *s = (proces8 *)param;

    pthread_mutex_lock(s->mutex);

    bool pozitiv = true;

    if((*s->threaduriRuleaza)<0)
        pozitiv=false;

    pthread_mutex_unlock(s->mutex);

    if (pozitiv == false)
    {
        sem_wait(s->sem);

        info(BEGIN, 8, s->id);

        info(END, 8, s->id);

        sem_post(s->sem);
    }
    else
    {
        int cont = 0;

        if (s->id == 14)
        {
            pthread_mutex_lock(s->mutex);

            cont = (*s->threaduriRuleaza);
            for (; cont < 4; cont = (*s->threaduriRuleaza))
                pthread_cond_wait(s->cond, s->mutex);

            pthread_mutex_unlock(s->mutex);

            sem_wait(s->sem);

            info(BEGIN, 8, s->id);

            info(END, 8, s->id);

            pthread_mutex_lock(s->mutex);

            (*s->threaduriRuleaza) = -1;

            pthread_cond_broadcast(s->cond);

            pthread_mutex_unlock(s->mutex);

            sem_post(s->sem);
        }
        else
        {
            pthread_mutex_lock(s->mutex);

            if ((*s->threaduriRuleaza) < 4)
            {
                (*s->threaduriRuleaza)++;

                pthread_cond_broadcast(s->cond);

                sem_wait(s->sem);

                info(BEGIN, 8, s->id);

                cont = (*s->threaduriRuleaza);
                for (; cont > 0; cont = (*s->threaduriRuleaza))
                    pthread_cond_wait(s->cond, s->mutex);

                pthread_mutex_unlock(s->mutex);

                info(END, 8, s->id);

                sem_post(s->sem);
            }
            else
            {
                cont = (*s->threaduriRuleaza);
                for (; cont > 0; cont = (*s->threaduriRuleaza))
                    pthread_cond_wait(s->cond, s->mutex);

                pthread_mutex_unlock(s->mutex);

                sem_wait(s->sem);

                info(BEGIN, 8, s->id);

                info(END, 8, s->id);

                sem_post(s->sem);
            }
        }
    }

    return NULL;
}

int main()
{
    init();

    info(BEGIN, 1, 0);

    pid_t p2 = -1, p3 = -1, p4 = -1, p5 = -1, p6 = -1, p7 = -1, p8 = -1;

    if ((p2 = fork()) == 0)
    {
        info(BEGIN, 2, 0);

        if (( p3 = fork()) == 0)
        {
            info(BEGIN, 3, 0);

            if ((p4 = fork()) == 0)
            {
                info(BEGIN, 4, 0);

                proces4 param[5];
                pthread_t tids[5];

                for (int i = 0; i <= 4; i++)
                {
                    param[i].id = i + 1;

                    pthread_create(&tids[i], 0, process4, &param[i]);
                }

                for (int i = 0; i <= 4; i++)
                    pthread_join(tids[i], NULL);

                info(END, 4, 0);
            }
            else
            {
                if ((p7 = fork()) == 0)
                {
                    info(BEGIN, 7, 0);

                    sem_t start_thread5;
                    sem_t stop_thread4;

                    sem_init(&start_thread5, 0, 0);
                    sem_init(&stop_thread4, 0, 0);

                    proces7 param[5];
                    pthread_t tids[5];

                    for (int i = 0; i <= 4; i++)
                    {
                        param[i].id = i + 1;
                        param[i].stop_thread4 = &stop_thread4;
                        param[i].start_thread5 = &start_thread5;

                        pthread_create(&tids[i], 0, process7, &param[i]);
                    }

                    for (int i = 0; i <= 4; i++)
                        pthread_join(tids[i], NULL);

                    sem_destroy(&start_thread5);
                    sem_destroy(&stop_thread4);

                    info(END, 7, 0);
                }
                else
                {
                    wait(NULL);
                    wait(NULL);

                    info(END, 3, 0);
                }
            }
        }
        else
        {
            if ((p5 = fork()) == 0)
            {
                info(BEGIN, 5, 0);

                info(END, 5, 0);
            }
            else
            {
                if ((p8 = fork()) == 0)
                {
                    info(BEGIN, 8, 0);

                    proces8 param[44];
                    pthread_t tids[44];

                    int threaduriRuleaza=0;

                    pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

                    sem_t sem;
                    sem_init(&sem,0,5);

                    pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

                    for (int i = 0; i <= 43; i++)
                    {
                        param[i].id = i + 1;
                        param[i].cond=&cond;
                        param[i].mutex=&mutex;
                        param[i].threaduriRuleaza=&threaduriRuleaza;
                        param[i].sem=&sem;

                        pthread_create(&tids[i], 0, process8, &param[i]);
                    }
                    for (int i = 0; i <= 43; i++)
                        pthread_join(tids[i], NULL);

                    pthread_mutex_destroy(&mutex);

                    pthread_cond_destroy(&cond);

                    sem_destroy(&sem);

                    info(END, 8, 0);
                }
                else
                {
                    wait(NULL);
                    wait(NULL);
                    wait(NULL);

                    info(END, 2, 0);
                }
            }
        }
    }
    else
    {
        if ((p6 = fork()) == 0)
        {
            info(BEGIN, 6, 0);

            info(END, 6, 0);
        }
        else
        {
            wait(NULL);
            wait(NULL);

            info(END, 1, 0);
        }
    }

    return 0;
}
