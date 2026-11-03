#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mapreduce.c"
#include "partition.c"
#include "threadpool.c"


void Map(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);

    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
            MR_Emit(token, "1");
        }
    }
    free(line);
    fclose(fp);
    //("Map is Finished\n");

}

void Reduce(char *key, unsigned int partition_idx) {
    //printf("Reduce is run for partition %d\n", partition_idx);
    int count = 0;
    char *value, name[100];

    while ((value = MR_GetNext(key, partition_idx)) != NULL) {
        count++;
        free(value);
    }
    sprintf(name, "result-%d.txt", partition_idx);
    FILE *fp = fopen(name, "a");
    fprintf(fp, "%s: %d\n", key, count);
    fclose(fp);
}

void queue_test() {
    unsigned int partition_count = 5; //DEBUG
    Partition_queue** partition = Partition_create(partition_count);

    /*
    struct Partition_element* test3 = &(struct Partition_element){"trolled", "50", NULL};
    struct Partition_element* test2 = &(struct Partition_element){"bye", "5", test3};
    struct Partition_element* test1 = &(struct Partition_element){"hello", "1", test2};
    partition[0]->head = test1;
    */

    Partition_add(partition, "trolled", "50", 0);
    Partition_add(partition, "abc", "1", 0);
    Partition_add(partition, "uuu", "100", 0);
    Partition_add(partition, "bbb", "77", 0);
    Partition_add(partition, "ccc", "88", 0);

    // printf("%u\n", partition[0]->size);
    char* result = NULL;
    //result = Partition_get(partition, "abc", 0);

    if (result == NULL)
        printf("Result was Null\n");
    else
        printf("Result was %s\n", result);

    // Print out queue

    Partition_element* current = partition[0]->head;
    printf("Queue:\n");
    while (current != NULL) {
    //for (size_t i = 0; i < 2; i++) {
        printf("%s:%s <- ", current->key, current->value);
        current=current->next;
    }

    Partition_free(partition, partition_count);
}


void jobqueue_test() {
    ThreadPool_t* tp = ThreadPool_create(2); // A thread pool with 3 queues

    ThreadPool_add_job(tp, (thread_func_t) Map, "sample_inputs/sample1.txt");

    assert (tp->jobs.size == 1);
    assert (tp->jobs.head != NULL);

    ThreadPool_add_job(tp, (thread_func_t) Map, "sample_inputs/sample2.txt");
    assert (tp->jobs.size == 2);
    assert (tp->jobs.head->next != NULL);

    ThreadPool_job_t* job = ThreadPool_get_job(tp);
    assert (tp->jobs.size == 1);
    assert (tp->jobs.head->next == NULL);
    assert(job != NULL);
    (job->func)(job->arg);  // Attempt to run the job
}


int main(int argc, char *argv[]) {
    // queue_test();
    //jobqueue_test();
    MR_Run(argc - 1, &(argv[1]), Map, Reduce, 5, 10);
}
