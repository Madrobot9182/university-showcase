#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **args){
    //check for valid arguments (args[0] is just filename)
    if (argc != 4)
        exit(-1);

    int n, len;
    char filename[10];

    sscanf(args[1], "%d", &n);
    sscanf(args[2], "%d", &len);
    strncpy(filename, args[3], 10);

    // Open the file
    FILE *file = fopen(filename, "r");

    // Initalize the array used for disjoint sets. Treat it like a 1-based array
    int *arr = malloc((len + 1) * sizeof(int));
    arr[0] = 0;

    #ifdef FASTIO
        char *buffer = malloc(n * 2 * sizeof(char));
        int buff_index = 0;
    #endif

    // Main logic
    for(int i = 0; i < n; i++){
        // Get lines in file
        char line[30];
        fgets(line, 30, file);

        // Get commands
        char command;
        int num1, num2;
        sscanf(line, "%c %d %d", &command, &num1, &num2);

        if (command == 'j'){

            // Creates a new set
            if (arr[num1] == 0 && arr[num2] == 0){
                int smaller = (num1 < num2) ? num1 : num2;
                arr[num1] = smaller;
                arr[num2] = smaller;
            }
            // New number added to set
            else if (arr[num1] == 0 || arr[num2] == 0){
                int empty = (num1 == 0) ? num1 : num2;
                int not_empty = (empty == num1) ? num2 : num1;

                arr[empty] = arr[not_empty];
            }
            else { // Both are already a set, must add both
                // TODO
                int small_set = (arr[num1] < arr[num2]) ? num1 : num2;
                int big_set = (small_set == arr[num1]) ? num2 : num1;

                arr[big_set] = arr[small_set];
            }
        }

        else { // command == '?'

            // TODO Union find here, updating parents;
            int previous, current = num1;
            while(arr[current] != current){
                previous = current;
                current = arr[current];
                arr[previous] = current;
            }

            current = num2;
            while(arr[current] != current){
                previous = current;
                current = arr[current];
                arr[previous] = current;
            }
            //DEBUG
            for (int i = 1; i <= len; i++){
                printf("%d ", arr[i]);
            } printf("\n");

            char result;
            // Check if both have same parent, and if parent isn't zero'
            if ((arr[num1] == arr[num2]) && (arr[num1]!= 0))
                result = 'T';
            else if (num1 == num2)
                result = 'T';
            else
                result = 'F';

            // Easyio prints as it goes, Fastio adds result to buffer
            #ifdef EASYIO
                printf("%c\n", result);
            #endif

            #ifdef FASTIO
                buffer[buff_index++] = result;
                buffer[buff_index++] = '\n';
            #endif
        }
    }

    #ifdef FASTIO
        // Remove the trailing newline
        buffer[buff_index-1] = '\0';
        puts(buffer);
        free(buffer);
    #endif

    // free memeory
    fclose(file);
    free(arr);
    return 0;
}
