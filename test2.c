// test2.c
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "aq.h"

#include "aux_file.h"

// Define the alarm queue
static AlarmQueue q;

void *producer(void *arg) {
    put_normal(q, 100);
    msleep(20);
    put_normal(q, 200);
    msleep(20);
    put_alarm(q, 300); // Alarm message
    msleep(20);
    put_normal(q, 400);

    return 0;
}

void *consumer(void *arg) {
    msleep(500);
    get(q);
    msleep(500);
    get(q);
    msleep(500);
    get(q);
    msleep(500);
    get(q);
    return 0;
}


int main(int argc, char ** argv) {
    int ret;

    q = aq_create();

    if (q == NULL) {
        printf("Alarm queue could not be created\n");
        exit(1);
    }

    pthread_t t1;
    pthread_t t2;

    void * res1;
    void * res2;

    printf("----------------\n");

    /* Fork threads */
    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, consumer, NULL);

    /* Join with all threads */
    pthread_join(t1, &res1);
    pthread_join(t2, &res2);


    printf("----------------\n");
    printf("Threads terminated with %ld, %ld\n", (uintptr_t) res1, (uintptr_t) res2);

    print_sizes(q);

    return 0;
}

