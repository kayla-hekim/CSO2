#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>
#include <time.h>

pthread_barrier_t barrier; // optional: to hopefully make deadlock more consistent

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void *pickUpAndEat(void *arg) {
    int n = (int) (long)arg;
    int delay = 50;

    // optional: sync up threads to make deadlock hopefully happen more consistently
    pthread_barrier_wait(&barrier);

    // take two chopsticks
    //CHANGE: changing the pattern of locks to use pthread_mutex_trylock(&lock) instead of pthread_mutex_lock(&lock)) -> new while loop
    while (1) {
        pthread_mutex_lock(&chopstick[n]);
        printf("Philosopher %d got chopstick %d\n", n, n);
        if (pthread_mutex_trylock(&chopstick[(n+1)%5]) == 0) {
            printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
            break;
        } // pthread_mutex_lock(&chopstick[(n+1)%5]);

        printf("Philosopher %d set down chopstick %d\n", n, n);
        pthread_mutex_unlock(&chopstick[n]);

        int wait_time = delay + rand() % delay; // random delay to reduce livelock and keep threads from retrying in sync
        usleep(wait_time);

        if (delay < 6400) {
            delay *= 2;
        }
    }
    // end CHANGE of while loop

    printf ("Philosopher %d is eating\n",n);
    // ADD: sleeping
    sleep(1);
    
    // set them back down
    printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
    pthread_mutex_unlock(&chopstick[(n+1)%5]);
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);
    return NULL;
}

int main(int argc, const char *argv[]) {
    //ADD: randomized number with varying seeds each time for "truer" randomization
    srand(time(NULL));

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

    return 0;
}