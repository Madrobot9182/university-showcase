#include "ex11q1.h"

// ############################################################################
// Vector
// ############################################################################
Vec* vec_create(void){
    Vec *vec = (Vec*)malloc(sizeof(Vec));

    vec->length = 0;
    vec->capacity = 0;
    vec->storage = NULL;

    return vec;
}

void vec_insert_at(Vec* vec, int at, int item){
    // Increase capacity by 1
    vec->capacity += 1;
    int *tmp_ptr = realloc(vec->storage, vec->capacity * sizeof(int));

    // check if allocation was successful
    if (tmp_ptr == NULL)
        exit(1);
    vec->storage = tmp_ptr;

    // Shift elements to make space for the new item
    for (int i = vec->length; i > at; i--)
        vec->storage[i] = (vec->storage)[i - 1];

    // Assign item
    (vec->storage)[at] = item;
    vec->length += 1;
}

void vec_print(Vec* vec){
    if (vec->length == 0)
        printf("[]\n");

    else if (vec->length == 1)
        printf("[%d]\n", vec->storage[0]);

    else { // length >= 2
        printf("[%d,", vec->storage[0]);
        for (int i = 1; i < vec->length - 1; i++)
            if (vec->storage[i] != 0)
                printf(" %d,", vec->storage[i]);

        printf(" %d]\n", vec->storage[vec->length - 1]);
    }
}

void vec_free(Vec* vec){
    free(vec->storage);
    free(vec);
}

// ############################################################################
// Linked List
// ############################################################################

LinkedListNode* list_node_create(int item){
    LinkedListNode *node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    node->item = item;
    node->next = NULL;
    return node;
}

LinkedList* list_create(void){
    LinkedList *list = (LinkedList*)malloc(sizeof(LinkedList));
    list->length=0;
    list->start = NULL;
    return list;
}

void list_insert_at(LinkedList* list, int at, int item){
    if (at == 0){
        // insert at the front
        LinkedListNode* tmp_node = list_node_create(item);
        tmp_node->next = list->start;
        list->start = tmp_node;
    }

    else {
        LinkedListNode *previous = NULL;
        LinkedListNode *current = list->start;
        for (int i = 0; i < at; i++){
            previous = current;
            current = current->next;
        }

        // insert between previous and current
        LinkedListNode* tmp_node = list_node_create(item);
        previous->next = tmp_node;
        tmp_node->next = current;
    }

    list->length += 1;
}

void list_print(LinkedList* list){
    LinkedListNode *node = list->start;

    if (list->length == 0)
        printf("[]\n");

    else if (list->length == 1)
        printf("[%d]\n", node->item);

    else { // length >= 2
        // Print first element
        printf("[%d, ", node->item);
        node = node->next;

        // Iterate through 1 to length - 1 elements
        for (int i = 1; i < list->length - 1; i++) {
            printf("%d, ", node->item);
            node = node->next;
        }

        //Print the last element
        printf("%d]\n", node->item);
    }
}
void list_free(LinkedList* list){
    LinkedListNode *current = list->start;
    LinkedListNode *previous = NULL;
    while (current != NULL){
        previous = current;
        current = current->next;
        free(previous);
    }
    free(list);
}

// ############################################################################
// Doubly Linked List
// ############################################################################

DoublyLinkedListNode* dlist_node_create(int item){
    DoublyLinkedListNode *node = (DoublyLinkedListNode*)malloc(sizeof(DoublyLinkedListNode));
    node->item = item;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

DoublyLinkedList* dlist_create(void){
    DoublyLinkedList *l = (DoublyLinkedList*)malloc(sizeof(DoublyLinkedList));
    l->length = 0;
    l->start = NULL;
    l->end = NULL;
    return l;
}

void dlist_insert_at(DoublyLinkedList* l, int at, int item){
    if (l->length == 0){
        // insert at the front
        DoublyLinkedListNode* tmp_node = dlist_node_create(item);
        l->start = tmp_node;
        l->end = tmp_node;
    }

    else if (l->length == 1){
        // A 2 element list
        DoublyLinkedListNode* tmp_node = dlist_node_create(item);
        if(at == 0){
            tmp_node->next = l->start;
            l->start->prev = tmp_node;
            l->start = tmp_node;
        } else {
            tmp_node->prev = l->end;
            l->end->next = tmp_node;
            l->end = tmp_node;
        }
    }

    else {
        // Length > 2
        // Move through the list
        DoublyLinkedListNode *previous = NULL, *current = NULL;

        // start from the left
        if(at <= l->length/2){
            current = l->start;
            for (int i = 0; i < at; i++){
                previous = current;
                current = current->next;
            }
        }

        // else it's faster to start from the right
        else {
            previous = l->end;
            for (int i = l->length; i > at; i--){
                current = previous;
                previous = previous->prev;
            }
        }

        //insert previous nodes
        DoublyLinkedListNode *tmp_node = dlist_node_create(item);
        if (previous != NULL) {
            previous->next = tmp_node;
            tmp_node->prev = previous;
        } else {
            // If previous is NULL we are inserting at the beginning
            l->start = tmp_node;
        }

        // insert after nodes
        tmp_node->next = current;
        if (current != NULL) {
            current->prev = tmp_node;
        } else {
            // If current is NULL we are inserting at the end
            l->end = tmp_node;
        }
    }

    l->length += 1;
}

void dlist_print(DoublyLinkedList* l){
    DoublyLinkedListNode *node = l->start;

    if (l->length == 0)
        printf("[]\n");

    else if (l->length == 1)
        printf("[%d]\n", node->item);

    else { // length >= 2
        // Print first element
        printf("[%d, ", node->item);
        node = node->next;

        // Iterate through 1 to length - 1 elements
        for (int i = 1; i < l->length - 1; i++) {
            printf("%d, ", node->item);
            node = node->next;
        }

        //Print the last element
        printf("%d]\n", node->item);
    }
}

void dlist_free(DoublyLinkedList* l){
    DoublyLinkedListNode *current = l->start;
    DoublyLinkedListNode *previous = NULL;
    while (current != NULL){
        previous = current;
        current = current->next;
        free(previous);
    }
    free(l);
}

// ############################################################################
// Main
// ############################################################################
int main(void) {
    char line[1000];
    // Uncomment each for loop as you implement that part

    clock_t begin_vec = clock();
   Vec* v = vec_create();

   for (int i = 0; i < 100000; i++) {
       int a, b;
       if (getchar() == 'i') {
           scanf("%s %d %s %d\n", line, &a, line, &b);
           vec_insert_at(v, b, a);
       } else {
           scanf("%s\n", line);
           vec_print(v);
       }
   }

   vec_free(v);
    clock_t end_vec = clock();

    clock_t begin_list = clock();
   LinkedList* l = list_create();

   for (int i = 0; i < 100000; i++) {
       int a, b;
       if (getchar() == 'i') {
           scanf("%s %d %s %d\n", line, &a, line, &b);
           list_insert_at(l, b, a);
       } else {
           scanf("%s\n", line);
           list_print(l);
       }
   }

   list_free(l);
    clock_t end_list = clock();

    clock_t begin_dlist = clock();
    DoublyLinkedList* dlist = dlist_create();

    for (int i = 0; i < 100000; i++) {
        int a, b;
        if (getchar() == 'i') {
            scanf("%s %d %s %d\n", line, &a, line, &b);
            dlist_insert_at(dlist, b, a);
        } else {
            scanf("%s\n", line);
            dlist_print(dlist);
        }
    }

    dlist_free(dlist);
    clock_t end_dlist = clock();

    fprintf(stderr, "Vector: %gs\n", (double)(end_vec - begin_vec) / CLOCKS_PER_SEC);
    fprintf(stderr, "Linked List: %gs\n", (double)(end_list - begin_list) / CLOCKS_PER_SEC);
    fprintf(stderr, "Doubly Linked List: %gs\n", (double)(end_dlist - begin_dlist) / CLOCKS_PER_SEC);
}
