#include "threadpool.h"


ThreadPool_t *ThreadPool_create(unsigned int num) {
    ThreadPool_t* threadpool = malloc(sizeof(ThreadPool_t));
    if (threadpool == NULL) {
        perror("Failed to allocate thread pool");
        return NULL;
    }
    threadpool->thread_count = num;
    threadpool->idle_count = num;
    threadpool->shutdown = 0;
    pthread_mutex_init(&(threadpool->mutex), NULL);
    pthread_cond_init(&(threadpool->cond), NULL);
    pthread_cond_init(&(threadpool->job_done_cond), NULL);

    // Initialize the thread array
    threadpool->threads = malloc(sizeof(pthread_t) * num);
    if (threadpool->threads == NULL) {
        perror("Failed to allocate threads");
        // Clean up on failure
        pthread_mutex_destroy(&(threadpool->mutex));
        pthread_cond_destroy(&(threadpool->cond));
        pthread_cond_destroy(&(threadpool->job_done_cond));
        free(threadpool);
        return NULL;
    }

    // Initialize the job queue
    ThreadPool_job_queue_t j;
    j.size = 0;
    j.head = NULL;
    pthread_mutex_init(&(j.mutex), NULL);

    // Set the jobs queue
    threadpool->jobs = j;

    // Initialize each thread with Thread run function
    // While this is a bad cast, I don't want to add more definitions to threadpool.h
    for (size_t i = 0; i < num; i++) {
        pthread_create(&(threadpool->threads[i]), NULL, (void*) Thread_run, threadpool);
    }

    return threadpool;
}


void ThreadPool_destroy(ThreadPool_t *tp) {
    tp->shutdown = 1;
    pthread_cond_broadcast(&(tp->cond));            // All threads wake up
    pthread_cond_broadcast(&(tp->job_done_cond));   // Also wake up from main thread waiting

    // De-allocate the threads array
    for (size_t i = 0; i < tp->thread_count; i++) {
        // Join the thread to destroy it
        pthread_join(tp->threads[i], NULL);
    }

    // Then de-allocate the locks
    pthread_mutex_destroy(&(tp->mutex));
    pthread_mutex_destroy(&(tp->jobs.mutex));
    pthread_cond_destroy(&(tp->cond));
    pthread_cond_destroy(&(tp->job_done_cond));

    // Free the thread pool
    free(tp->threads);
    free(tp);
}


bool ThreadPool_add_job(ThreadPool_t *tp, thread_func_t func, void *arg) {
    ThreadPool_job_queue_t* job_queue;  // Reference to job queue
    ThreadPool_job_t* temp;             // For traversing the job queue
    struct stat st1, st2;               // For storing filesizes

    ThreadPool_job_t* new_job = malloc(sizeof(ThreadPool_job_t));
    if (new_job == NULL) {
        perror("Failed to allocate new job");
        return false;
    }
    new_job->func = func;
    new_job->arg = arg;
    new_job->next = NULL;

    double current_size = 0;
    // Now check if this is a file (MAP) or a int partition (Reduce)
    if (stat((char*)arg, &st1) == 0) {
        current_size = st1.st_size;
    }
    // Else this is a reducer job, we just append to front

    // First lock the mutex before working with critical data
    pthread_mutex_lock(&(tp->jobs.mutex));


    // Get temp ref to job queue
    job_queue = &(tp->jobs);
    temp = job_queue->head; // Set temp to head of queue


    // Check if queue is empty
    if (job_queue->size == 0) {
        job_queue->head = new_job;
    } else {
        // Get the size of the first job in the queue (head)
        double next_size = 1;
        if (stat((char*)temp->arg, &st2) == 0) {
            next_size = st2.st_size;
        }

        // Check if we insert before the head
        if (current_size <= next_size) {
            new_job->next = job_queue->head;
            job_queue->head = new_job;
        } else {
            ThreadPool_job_t* prev = temp;
            ThreadPool_job_t* curr = temp->next;

            while (curr != NULL) {
                if (stat((char*)curr->arg, &st2) != 0) {
                    pthread_mutex_unlock(&(tp->jobs.mutex));
                    free(new_job);
                    perror("Failed to get file size for node");
                    return false;
                }

                // Insert in middle of queue
                if (current_size <= st2.st_size) {
                    new_job->next = curr;
                    prev->next = new_job;
                    break;
                }

                prev = curr;
                curr = curr->next;
            }

            // Insert at end
            if (curr == NULL) {
                prev->next = new_job;
            }
        }
    }

    job_queue->size = job_queue->size + 1;

    // Signal that a new job has been added
    pthread_cond_signal(&(tp->cond));

    // Done working with critical data
    pthread_mutex_unlock(&(tp->jobs.mutex));


    return true;
}


ThreadPool_job_t *ThreadPool_get_job(ThreadPool_t *tp) {
    // I assume that threadpool_add_job will insert jobs in right order
    // Hence just take jobs in order

    // Try to lock the mutex
    pthread_mutex_lock(&(tp->jobs.mutex));


    ThreadPool_job_t* job = NULL;
    ThreadPool_job_queue_t* job_queue = &(tp->jobs);

    if (job_queue->size > 0) {
        job = job_queue->head;
        // Move head to next, decrement size
        job_queue->head = job->next;
        job_queue->size = job_queue->size - 1;

        // Detach the new job from the queue
        job->next = NULL;
    }

    pthread_mutex_unlock(&(tp->jobs.mutex));

    return job;
}


void *Thread_run(ThreadPool_t *tp) {
    // Constantly loop trying to get a job from the job queue
    ThreadPool_job_t* job;

    //  Exit by calling pthread_exit
    while(true) {
        job = NULL;

        // Lock the mutex first
        pthread_mutex_lock(&(tp->mutex));

        // Enter a waiting list with conditional. Signalled when a new job is added.
        // While loop to check in case another thread takes the job first
        while(((job = ThreadPool_get_job(tp)) == NULL) && !tp->shutdown) {
            pthread_cond_wait(&(tp->cond), &(tp->mutex));
        }

        // Unlock the mutex before executing/exiting
        if (tp->shutdown) {
            pthread_mutex_unlock(&(tp->mutex));
            pthread_exit(NULL); // Kill the thread
        }
        // Else there's a job to run
        tp->idle_count = tp->idle_count - 1;  // Still under mutex lock

        pthread_mutex_unlock(&(tp->mutex));

        // Execute the job
        (job->func)(job->arg);


        // Thread becomes idle again, update under mutex lock
        pthread_mutex_lock(&(tp->mutex));
        tp->idle_count = tp->idle_count + 1;

        // Signal to main thread check another job was completed
        pthread_cond_signal(&(tp->job_done_cond));
        pthread_mutex_unlock(&(tp->mutex));


        // Free the job memory
        free(job);

    }
    return NULL;
}

void ThreadPool_check(ThreadPool_t *tp) {
    pthread_mutex_lock(&(tp->mutex));
    while ((tp->idle_count < tp->thread_count) || (tp->jobs.size > 0)) {
        // Releases whenever a thread completes a job
        pthread_cond_wait(&(tp->job_done_cond), &(tp->mutex));
    }
    pthread_mutex_unlock(&(tp->mutex));
}
