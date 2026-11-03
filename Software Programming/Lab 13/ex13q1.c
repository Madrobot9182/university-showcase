#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int find_parent(int num, int *arr){
    int root = num;
    while(arr[root] != root)
        root = arr[root];

    //Union find secret sauce (yummy)
    // Aka set the original nums parent to root for faster lookup next search
    arr[num] = root;
    return root;
}


int main(int argc, char **args){
    //check for valid arguments (args[0] is just filename)
    if (argc != 4)
        exit(-1);

    int n, k;
    char filename[10];

    sscanf(args[1], "%d", &n);
    sscanf(args[2], "%d", &k);
    strncpy(filename, args[3], 10);

    // Open the file
    FILE *file = fopen(filename, "r");

    // Initalize the array used for disjoint sets. Add a 0 to treat it like a 1-based array
    int *arr = malloc((k + 1) * sizeof(int));
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

                // Set the smaller value to be the root
                arr[num1] = smaller;
                arr[num2] = smaller;
            }
            // Single number added to set
            else if (arr[num1] == 0 || arr[num2] == 0){
                int empty = (arr[num1] == 0) ? num1 : num2;
                int not_empty = (empty == num1) ? num2 : num1;

                int root = find_parent(not_empty, arr);

                arr[empty] = root;
            }
            else { // Both are already a set, combine both
                int root1 = find_parent(num1, arr);
                int root2 = find_parent(num2, arr);

                // Find the smaller root value
                int smaller_root = (root1 < root2) ? root1 : root2;
                int bigger_root = (smaller_root == root1) ? root2 : root1;

                // Set the root of the bigger set to parent of smaller set
                arr[bigger_root] = smaller_root;
            }
        }

        else { // command == '?'
            int root1 = find_parent(num1, arr);
            int root2 = find_parent(num2, arr);

            char result;
            // Check if both have same parent, and if parent isn't zero
            if ((root1 == root2) && (arr[num1] != 0))
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
