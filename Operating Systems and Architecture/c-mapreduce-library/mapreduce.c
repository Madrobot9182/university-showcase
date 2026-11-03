#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mapreduce.h"
#include "threadpool.h"
#include "partition.h"

Partition_queue** partition;        // The global intermediate partition structure
unsigned int partition_count;       // Number of partitions in array (Does not change)


void MR_Run(unsigned int file_count, char *file_names[],
            Mapper mapper, Reducer reducer,
            unsigned int num_workers, unsigned int num_parts) {
    // Initialize the global partition structure
    partition = Partition_create(num_parts);
    partition_count = num_parts;

    // First Initialize the thread pool
    ThreadPool_t* thread_pool = ThreadPool_create(num_workers);
    // Add mapper jobs to the job queue
    for (size_t i = 0; i < file_count; i++) {
        // Converting the wrapper like this is dangerous but fine for now
        ThreadPool_add_job(thread_pool, (thread_func_t) mapper, (void*) file_names[i]);
    }

    // After adding all jobs, wait until all jobs complete, and threads wait
    ThreadPool_check(thread_pool);
    //  Add reduce jobs to the job queue
    // Outer loop iterates through all partition. each thread gets one part.
    for (unsigned int i = 0; i < num_parts; i++) {
        // Create array with [reducer_function, partition_index]
        void **args = malloc(sizeof(void*) * 2);
        args[0] = (void*)reducer;
        args[1] = (void*)(uintptr_t) i;
        ThreadPool_add_job(thread_pool, MR_Reduce, args);
    }

    // Wait again for all jobs complete, and threads wait
    ThreadPool_check(thread_pool);

    // Finally delete the thread pool
    ThreadPool_destroy(thread_pool);
    // Delete the partition
    Partition_free(partition, num_parts);
}


void MR_Emit(char *key, char *value) {
    unsigned int index = MR_Partitioner(key, partition_count);  // Which queue to add to
    Partition_add(partition, key, value, index);                // Add key-value to partition
}


unsigned int MR_Partitioner(char *key, unsigned int num_partitions) {
    // Taken from the lab manual
    unsigned long hash = 5831;
    int c;
    while((c = *key++) != '\0')
        hash = hash * 33 + c;

    return hash % num_partitions;
}


void MR_Reduce(void *threadarg) {
    // Reconvert the arguments back
    void **args = (void**)threadarg;
    Reducer reducer = (Reducer)args[0];
    unsigned int index = (uintptr_t)args[1];
    char* key;
    free(threadarg);

    // Get next unprocessed key and run reducer on it
    while((key = Partition_next_key(partition, index)) != NULL) {
        // This is one thread working on one partition
        (reducer)(key, index);
        free(key);  // free the copy made in partition_next_key
    }
}


char *MR_GetNext(char *key, unsigned int partition_idx) {
    // Get the next value and make a copy if needed
    //return Partition_get_next(partition, key, partition_idx);
    pthread_mutex_lock(&(partition[partition_idx]->mutex));

    char* result = NULL;
    Partition_queue* queue = partition[partition_idx];
    Partition_element* current = queue->head;

    // If the current key is correct, move onto the next element and free the previous
    if ((current != NULL) && (strcmp(key, current->key) == 0)) {
        result = current->value;
        queue->head = current->next;
        queue->size = queue->size - 1;

        // Free current since we no longer need
        free(current->key);
        free(current);
    }

    pthread_mutex_unlock(&(partition[partition_idx]->mutex));
    return result;
}
