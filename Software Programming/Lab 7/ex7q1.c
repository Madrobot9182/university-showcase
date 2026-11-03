#include <stdio.h>
#include <stdlib.h>

/*
Hints from Lab 7 document was referenced, along with my solution to lab 6 part 1
Additionall referenced https://stackoverflow.com/questions/20036408/how-to-properly-reallocate-a-two-dimensional-array-in-c
https://stackoverflow.com/questions/917783/how-do-i-work-with-dynamic-multi-dimensional-arrays-in-c
for 2d array mem allocation
*/

int get_input(int *aisle, int *serial, char *item){
    // returns 0 if no input received, else returns parameters
    // Aisle for index print function, serial to mod later, item is the char code to add to aisle
    if (scanf("%d ", serial) != 1)
        return 0;

    if(*serial == 0){
        scanf("%d", aisle);
    }
    else {
        scanf("%c", item);
    }

    return 1;
}


int main(void) {
    int n, aisle, serial; scanf("%d", &n);
    char item;

    char **aisles = malloc(n * sizeof(char*));
    int *lengths = malloc(n * sizeof(int));
    for(int i = 0;i<n;i++)
        lengths[i] = 0;

    while(get_input(&aisle, &serial, &item)){

        if(serial == 0){
            // print item in an aisle

            for (int i = 0; i < lengths[aisle]; i++){
                printf("%c", aisles[aisle][i]);
            }
            printf("\n");

        } else {
            // Compute serial code, allocate extra space, and add it to array
            int aisle = serial % n;

            // reallocate by one aditional space for a row
            char *tmp_ptr = (char *) realloc(aisles[aisle], (lengths[aisle] + 1) * sizeof(char));

            if (tmp_ptr == NULL)
                exit(1);
            aisles[aisle] = tmp_ptr;

            aisles[aisle][lengths[aisle]] = item;
            lengths[aisle] += 1;
        }
    }
    // free memory
    for(int i = 0; i < n; i++)
        free(aisles[i]);
    free(aisles);
    free(lengths);
    return 0;
}
