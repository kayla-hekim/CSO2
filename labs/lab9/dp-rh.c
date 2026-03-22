#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>

pthread_barrier_t barrier; // optional: to hopefully make deadlock more consistent

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void *pickUpAndEat(void *arg) {
    int n = (int) (long)arg;

    // optional: sync up threads to make deadlock hopefully happen more consistently
    pthread_barrier_wait(&barrier);

    // ADDED (creates order of left and right, choose whichever is lowest first !):
    int left = n;
    int right = (n + 1) % 5;

    int first = left;
    int second = right;

    if (first > second) {
        int temp = first;
        first = second;
        second = temp;
    }


    // take two chopsticks
    // CHANGE: pick up first chopstick first then second
    pthread_mutex_lock(&chopstick[first]);
    printf("Philosopher %d got chopstick %d\n", n, first);
    pthread_mutex_lock(&chopstick[second]);
    printf("Philosopher %d got chopstick %d\n", n, second);
    
    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    
    // set them back down
    // CHANGE: set down second chopstick, then first chopstick
    printf("Philosopher %d set down chopstick %d\n", n, second);
    pthread_mutex_unlock(&chopstick[second]);
    printf("Philosopher %d set down chopstick %d\n", n, first);
    pthread_mutex_unlock(&chopstick[first]);
    return NULL;
}

int main(int argc, const char *argv[]) {
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