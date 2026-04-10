#include "life.h"
#include <pthread.h>


typedef struct {
    int id;
    int start_row;
    int end_row;
    int steps;
    LifeBoard *state;
    LifeBoard *next_state;
    pthread_barrier_t *barrier;
} WorkerArgs;


static void *worker(void *arg) {
    WorkerArgs *data = (WorkerArgs *) arg;

    for (int step = 0; step < data->steps; step += 1) {
        for (int y = data->start_row; y < data->end_row; y += 1) {
            for (int x = 1; x < data->state->width - 1; x += 1) {
            
                /* For each cell, examine a 3x3 "window" of cells around it,
                * and count the number of live (true) cells in the window. */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset += 1) {
                    for (int x_offset = -1; x_offset <= 1; x_offset += 1) {
                        if (LB_get(data->state, x + x_offset, y + y_offset)) {
                            live_in_window += 1;
                        }
                    }
                }
                
                /* Cells with 3 live neighbors remain or become live.
                Live cells with 2 live neighbors remain live. */
                LB_set(data->next_state, x, y,
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && LB_get(data->state, x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        pthread_barrier_wait(data->barrier);

            if (data->id == 0) {
                LB_swap(data->state, data->next_state);
            }

        pthread_barrier_wait(data->barrier);
    }

    return NULL;
}



void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    // og from serial
    LifeBoard *next_state = LB_new(state->width, state->height);

    // initialize pthread resources
    pthread_t tids[threads];
    WorkerArgs args[threads];

    pthread_barrier_t wait_for;
    pthread_barrier_init(&wait_for, NULL, threads);

    int inner_rows = state->height - 2;
    
    // create those [threads] threads
    for (int i = 0; i < threads; i+=1) {
        // fill arg structu WorkerArgs[threads];
        args[i].id = i;
        args[i].start_row = 1 + (inner_rows * i) / threads;
        args[i].end_row   = 1 + (inner_rows * (i + 1)) / threads;
        args[i].steps = steps;
        args[i].state = state;
        args[i].next_state = next_state;
        args[i].barrier = &wait_for;

        // worker helper function call - what threads run each
        pthread_create(&tids[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < threads; i+=1) {
        // worker helper function call - what threads run each
        pthread_join(tids[i], NULL);
    }
    
    // clean barrier resources
    pthread_barrier_destroy(&wait_for);
    // from serial
    LB_del(next_state);
}
