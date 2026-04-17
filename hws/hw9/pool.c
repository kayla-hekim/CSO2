#include "pool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_TASKS_NUM 200 

static pthread_t *workers = NULL; // array of thread workers
static int worker_count = 0; // count of how many thread workers there are in `workers`

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t task_available = PTHREAD_COND_INITIALIZER;

static pthread_cond_t all_done = PTHREAD_COND_INITIALIZER;
static int total_finished = 0;

struct Task {
    task_fn function;
    void *arg;
    void *result;
    int id;
    int finished;
};

static struct Task tasks[MAX_TASKS_NUM];

static int next_task_id = 0;
static int stopping = 0;

static int queue[MAX_TASKS_NUM];
static int queue_head = 0;
static int queue_tail = 0;
static int queue_count = 0;


static void *worker_task_do(void *arg) {
    (void) arg;

    while (1) {
        pthread_mutex_lock(&lock);

        // wait for a task
        while (queue_count == 0 && !stopping) {
            pthread_cond_wait(&task_available, &lock);
        }
 
        // if we're stopping and there's no tasks in the queue
        if (stopping && queue_count == 0) {
            pthread_mutex_unlock(&lock);
            break; // !
        }

        // dqeueue task to show we're doing it/done with it
        int id = queue[queue_head];
        queue_head = (queue_head + 1) % MAX_TASKS_NUM;
        queue_count -= 1;

        pthread_mutex_unlock(&lock); // !

        // run the task
        void *res = tasks[id].function(tasks[id].arg);

        pthread_mutex_lock(&lock); // !
        // store the results in the tasks' result attribute
        tasks[id].result = res;
        tasks[id].finished = 1;
        total_finished += 1;
        pthread_cond_broadcast(&all_done);

        pthread_mutex_unlock(&lock); // !
    }
    return NULL;
}

// Initialize the thread pool.
void pool_setup(int threads) {
    worker_count = threads;

    next_task_id = 0;
    stopping = 0;
    queue_head = 0;
    queue_tail = 0;
    queue_count = 0;

    total_finished = 0;

    workers = malloc(sizeof(pthread_t) * threads);
    if (workers == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    for (int i = 0; i < threads; i += 1) {
        int rc = pthread_create(&workers[i], NULL, worker_task_do, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed\n");
            exit(1);
        }
    }
}

// what to account for
// task_fn function;
// void *arg;
// void *result; <- not yet that's in worker_task_do() at the end
// int id;
// int finished;

// Submit a new task to the thread pool and return its ID.
int pool_submit_task(task_fn task, void *argument) {
    pthread_mutex_lock(&lock);

    int new_id = next_task_id;
    next_task_id += 1;

    // populate tasks at new_id for the current task to do
    tasks[new_id].function = task;
    tasks[new_id].arg = argument;
    tasks[new_id].result = NULL;
    tasks[new_id].id = new_id;
    tasks[new_id].finished = 0;

    // enqueue the task to take
    queue[queue_tail] = new_id;
    queue_tail = (queue_tail + 1) % MAX_TASKS_NUM;
    queue_count += 1;

    // tell AT LEAST 1 thread to take new available task
    pthread_cond_signal(&task_available);

    pthread_mutex_unlock(&lock);

    return new_id;
}

// Get the return value from a task.
void *pool_get_task_result(int task_id) {
    pthread_mutex_lock(&lock);

    // make sure the task does have a valid finished value and is finished to return, else return NULL 
    if (task_id < 0 || task_id >= next_task_id || !tasks[task_id].finished) {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    void *res = tasks[task_id].result; // grab task's result value & return

    pthread_mutex_unlock(&lock);
    return res;
}

// Return after all existing tasks have finished.
void pool_wait(void) {
    pthread_mutex_lock(&lock);

    int target = next_task_id; // snapshot of what task we're on at the moment
    // while we haven't really completed up to our next task, just keep waiting to finish
    while (total_finished < target) {
        pthread_cond_wait(&all_done, &lock);
    }

    pthread_mutex_unlock(&lock);
}

// Tell the thread pool threads to finish any current task and then exit.
void pool_stop(void) {
    pthread_mutex_lock(&lock);
    // we're stopping, tell all remaining and available threads to pick up rest of work and finish to complete all 
    stopping = 1;
    pthread_cond_broadcast(&task_available);
    pthread_mutex_unlock(&lock);

    // clean up threads and workers structure
    for (int i = 0; i < worker_count; i+=1) {
        pthread_join(workers[i], NULL);
    }
    free(workers);
    workers = NULL;
    worker_count = 0;
}
