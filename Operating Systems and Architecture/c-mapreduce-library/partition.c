#include "partition.h"


Partition_queue** Partition_create(unsigned int num_parts) {
    Partition_queue** partition = malloc(sizeof(Partition_queue*) * num_parts);
    if (partition == NULL){
        perror("Partition allocation failed");
        return NULL;
    }

    // Initialize all queues
    for (size_t i = 0; i < num_parts; i++) {
        partition[i] = malloc(sizeof(Partition_queue));
        if (partition[i] == NULL) {
            perror("Partition allocation failed");
            free(partition);
            return NULL;
        }

        partition[i]->size = 0;
        partition[i]->head = NULL;
        pthread_mutex_init(&(partition[i]->mutex), NULL);
    }
    return partition;
}


bool Partition_add(Partition_queue** partition, char* key, char* value, unsigned int index) {
    // Test validate input
    if (partition == NULL || key == NULL || value == NULL || partition[index] == NULL) {
        perror("INVALID MAP INPUT");
        return false;
    }

    // Create the new partition element
    struct Partition_element* new_element = malloc(sizeof(Partition_element));

    // Copy the key and values into allocated strings
    size_t len1 = strlen(key) + 1;
    char* kcopy = malloc(len1);
    if (kcopy == NULL) {
        perror("Failed to allocate copy");
        return false;
    }
    strcpy(kcopy, key);

    size_t len2 = strlen(value) + 1;
    char* vcopy = malloc(len2);
    if (kcopy == NULL) {
        perror("Failed to allocate copy");
        free(kcopy);
        return false;
    }
    strcpy(vcopy, value);

    new_element->key = kcopy;
    new_element->value = vcopy;
    new_element->next = NULL;

    // Lock mutex, only allow one thread to traverse and modify the queue
    pthread_mutex_lock(&(partition[index]->mutex));

    Partition_queue* queue = partition[index];
    Partition_element* current = queue->head;
    Partition_element* previous = NULL;

    // Empty queue case
    if (current == NULL) {
        queue->head = new_element;
    }
    // Insert at head case
    else if (strcmp(key, current->key) < 0) {
        queue->head = new_element;
        new_element->next = current;
    }
    // Insert in middle/end case
    else {
        while (current != NULL && strcmp(key, current->key) >= 0) {
            previous = current;
            current = current->next;
        }
        // Insert between previous and current
        previous->next = new_element;
        new_element->next = current;
    }

    queue->size = queue->size + 1;

    pthread_mutex_unlock(&(partition[index]->mutex));
    return true;
}


char* Partition_next_key(Partition_queue** partition, unsigned int index) {
    pthread_mutex_lock(&(partition[index]->mutex));

    Partition_queue* queue = partition[index];

    if (queue->head == NULL)
        return NULL;

    // Make a copy of key, free it in MR reduce
    size_t len = strlen(queue->head->key) + 1;
    char* kcopy = malloc(len);
    strcpy(kcopy, queue->head->key);

    pthread_mutex_unlock(&(partition[index]->mutex));

    return kcopy;
}


void Partition_free(Partition_queue** partition, unsigned int num_parts) {
    for (size_t i = 0; i < num_parts; i++){
        // Iterate through and free every element
        // This loop will free any leftover elements not used and free manually
        Partition_element* current = partition[i]->head;
        while (current != NULL) {
            Partition_element* next = current->next;

            free(current->key);
            free(current->value);
            free(current);
            current = next;
        }

        // Free the mutex and each queue
        pthread_mutex_destroy(&(partition[i]->mutex));
        free(partition[i]);
    }

    // free the queue array
    free(partition);
}
