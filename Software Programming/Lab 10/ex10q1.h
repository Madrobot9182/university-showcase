#pragma once

#include <stdio.h>
#include <stdlib.h>
typedef struct {
    int* head;    // Start of dynamic storage
    int* start;   // Index 0 of ring buffer
    int* end;     // One after the last element in the ring buffer
    int length;   // Number of elements currently stored in the ring buffer
    int capacity; // Maximum number of elements this ring buffer can hold
} Ring;

//Function Definitions//

Ring* init_empty_ring(); // initialize a empty ring

void push_front(Ring* ring, int element);
void push_back(Ring* ring, int element);

int pop_front(Ring* ring);
int pop_back(Ring* ring);

int is_full(Ring *ring) {return(ring->length == ring->capacity);}
void resize(Ring *ring); //Reorder, Double capacity

void print(Ring *ring);
void free_ring(Ring *ring);

void rebalance(Ring *front, Ring *back); // for rebalancing two rings

//Function Implementation//


Ring* init_empty_ring(){
    int* arr = NULL;
    Ring *r = malloc(sizeof(Ring));
    r->head = arr; // Head is never changed
    r->start = arr;
    r->end = arr;
    r->length = 0;
    r->capacity = 0;
    return r;
}


//TODO: pushfront is pushing elements to back instead?
void push_front(Ring* ring, int element){
    // Check if ring is full, resize when needed
    if (is_full(ring))
        resize(ring);
    // Move the start pointer
    if (ring->start == ring->head)
        ring->start = ring->head + ring->capacity - 1;
    else
        ring->start--;

    // Put element at front, move front
    *(ring->start) = element;

    // Increment length
    ring->length++;
}



void push_back(Ring* ring, int element) {
    // check if ring is full, resize when needed
    if (is_full(ring))
        resize(ring);

    // move the end pointer
    if (ring->end == ring->head + ring->capacity - 1)
        ring->end = ring->head;
    else
        ring->end++;

    // put element at end
    *(ring->end) = element;

    // increment length
    ring->length++;
}



int pop_front(Ring* ring){
    // check if ring is empty
    if (ring->length == 0){
        printf("empty\n");
        return -1;
    }

    // get element at start, move start forward
    int element = *(ring->start);

    // move the end pointer
    if (ring->start == ring->head + ring->capacity - 1)
        ring->start = ring->head;
    else
        ring->start += 1;

    ring->length--;
    return element;
}


int pop_back(Ring* ring){
    // check if ring is empty
    if (ring->length == 0){
        printf("empty\n");
        return -1;
    }

    // get element at start, move start forward
    int element = *(ring->end);

    // move the end pointer
    if (ring->end == ring->head)
        ring->end = ring->head + ring->capacity - 1;
    else
        ring->end -= 1;

    ring->length--;
    return element;
}


void resize(Ring *ring){
    int new_capacity = (ring->capacity == 0) ? 1 : ring->capacity * 2;

    // Allocate a new array with the updated capacity
    int *new_head = malloc(new_capacity * sizeof(int));
    if (new_head == NULL)
        exit(-1);

    // Copy elements from the old array to the new array
    for (int i = 0; i < ring->length; i++)
        new_head[i] = ring->head[(ring->start - ring->head + i) % ring->capacity];

    // Free the old array
    free(ring->head);

    // Update pointers and capacity
    ring->head = new_head;
    ring->start = ring->head;
    ring->end = ring->head + ring->length - 1;
    ring->capacity = new_capacity;
}


void print(Ring *ring){
    // For printing and debugging purposes
    printf("[");
    for (int i = 0; i < ring->capacity; i++){
        int temp = *(ring->head+i);
        if (temp)
            printf(" %d,", temp);
        else
            printf(" -,");
    }
    printf("] len = %d, start = %d, end = %d ", ring->length, *ring->start, *ring->end);
}

void free_ring(Ring *ring){
    free(ring->head);
    free(ring);
}


void rebalance(Ring *front, Ring *back) {
    // Balanced 1 2 || 3 4 <- good
    // Left leaning 1 2 || 3 <- good

    // Right leaning 1 || 2 3 <- adjust to left leaning
    while (front->length - back->length < 0) {
        push_back(front, pop_front(back));
    }

    // Excessive left leaning 1 2 3 || 4 <- adjust to balance
    while (front->length - back->length > 1) {
        push_front(back, pop_back(front));
    }
}
