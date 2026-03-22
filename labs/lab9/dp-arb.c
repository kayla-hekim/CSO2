#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>

// ADD: global arbitrator mutex
pthread_mutex_t global_arbitrator_mutex;

pthread_barrier_t barrier; // optional: to hopefully make deadlock more consistent

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void *pickUpAndEat(void *arg) {
    int n = (int) (long)arg;

    // optional: sync up threads to make deadlock hopefully happen more consistently
    pthread_barrier_wait(&barrier);

    // ADD: lock the global arbitrator mutex
    pthread_mutex_lock(&global_arbitrator_mutex);

    // take two chopsticks
    pthread_mutex_lock(&chopstick[n]);
    printf("Philosopher %d got chopstick %d\n", n, n);
    pthread_mutex_lock(&chopstick[(n+1)%5]);
    printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);

    // ADD: unlock the global arbitrator mutex
    pthread_mutex_unlock(&global_arbitrator_mutex);
    
    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    
    // set them back down
    printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
    pthread_mutex_unlock(&chopstick[(n+1)%5]);
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);

    return NULL;
}

int main(int argc, const char *argv[]) {
    // ADD: initialize the global arbitrator mutex
    pthread_mutex_init(&global_arbitrator_mutex, NULL);

    pthread_barrier_init(&barrier, NULL, 5);

    for(int i = 0; i < 5; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);

    for(int i =0; i < 5; i += 1)
        pthread_create(&philosopher[i], NULL, pickUpAndEat, (void *)(size_t)i);

    for(int i=0; i < 5; i += 1)
        pthread_join(philosopher[i], NULL);

    for(int i=0; i < 5; i += 1)
        pthread_mutex_destroy(&chopstick[i]);

    pthread_barrier_destroy(&barrier);

    // ADD: destroy the global arbitrator mutex at the end of eating
    pthread_mutex_destroy(&global_arbitrator_mutex);
    return 0;
}