# Synchronization Primitives

The general structure of mapreduce can be segmented into steps. Revelently, I first assigns map jobs, call Threadpool_check to wait for all jobs to complete, then assign reducing jobs, then call Threadpool_check again to wait for all jobs to complete

- Each partition in my array of partitions has a mutex lock. Any threads that are called must wait on the mutex lock before modifying/writing data to ensure concurrency

- The threadpool uses 3 Synchronization primitives

  - cond: a conditional variable that all threads wait on. I signal this variable to wake up a thread, which either checks and executes a job, or terminates if signalled by the shutdown flag

  - job_done_cond: a conditional variable that the main thread waits on. Any subthreads that completes a job will signal this variable, which wakes the main thread and checks if the job queue is empty and all threads are idle. This is how I implemented Threadpool_check

  - mutex: a mutex lock used for cond to function. Ie lock the mutex before waiting, unlock before performing work.

- The job queue also has a mutex lock. Any subthread must wait on this lock before modifying the job queue to protect this critical data

# Implementation of Partitions

Partition is essentially a simple array of linked list, with a mutex lock on each queue. Threads will access the queue at a given index, then acquire the lock if they need to modify the critical data.

Each element of the queue contains the key-value pair, and a pointer to its next element

# Testing

I split the process into chunks: [Creation, mapping, checking, reducing, destruction]. Every step of the way I program in a chunk, the use constant print statment, assertions, and test cases to ensure no defects. Whenever issues propergate I test chunks of code and fix issues that arises from new code added. I also diagramed the program flow on my whiteboard, just to help visualize everything!

Valgrind and gbd was then used near the end when I had memory corruption issues. It help me discover where I needed allocate copies of certain key-values, and where I was preemptivly freeing memory. This part actually took me longer to debug vs concurrency issues.

I left my testing file in the submission but is not part of my final program. I made testing cases and played around with the provided Map Reduce functions to see how everything works.

# Sources

[C - Segmentation Fault with strcmp? - Stack Overflow](https://stackoverflow.com/questions/7316306/c-segmentation-fault-with-strcmp)

[What are the rules for casting pointers in C? - Stack Overflow](https://stackoverflow.com/questions/17260527/what-are-the-rules-for-casting-pointers-in-c)

[How to Use Mutex Lock in C | Delft Stack](https://www.delftstack.com/howto/c/mutex-in-c/)[How to Use Mutex Lock in C | Delft Stack](https://www.delftstack.com/howto/c/mutex-in-c/)

[Conditional wait and signal in multi-threading - GeeksforGeeks](https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/)

Map_Partition function was taken from the lab manual. 
