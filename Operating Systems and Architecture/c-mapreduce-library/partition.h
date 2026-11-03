#ifndef PARTITION_H
#define PARTITION_H
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Partition_element {
    char* key;                          // Key of pair
    char* value;                        // Value of pair
    struct Partition_element *next;    // Next element in queue
} Partition_element;

typedef struct {
    unsigned int size;                  // no. elements in the queue
    Partition_element *head;            // pointer to the first (ascii ascending key) element
    pthread_mutex_t mutex;              // Mutex lock for the given queue
} Partition_queue;


/**
 * Construct a new partition object
 *
 * @param num_parts
 * The number of queues in the partition
 *
 * @return Map_Partition_queue**
 * return an array of queue pointers
 */
Partition_queue** Partition_create(unsigned int num_parts);


/**
 * Add a key value to the indexed queue.
 *
 * @param partition** the array of queue pointers
 * @param key key in pair
 * @param value value in pair
 * @param index
 * The index of which queue to add into
 *
 * @return
 * true if element was successfully added, else return false
 */
bool Partition_add(Partition_queue** partition, char* key, char* value, unsigned int index);


/*
 * Get the next key value from a queue.
 *
 * @param partition** the array of queue pointers
 * @param index The index of which queue to add into
 *
 * @return the next key, or NULL if the queue is empty
 */
char* Partition_next_key(Partition_queue** partition, unsigned int index);
/*
 * Safely free a array of partitions
 *
 * @param partition** the array of queue pointers
 * @param num_parts The number of queues in the partition
 */
void Partition_free(Partition_queue** partition, unsigned int num_parts);
#endif
