//
// Created by ryan on 21/09/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// FILE *fptr; // Access files
//
// char INPUT_PATH[100];
// char OUTPUT_PATH[100];

void min_max (const int *array, int *min, int *max) {
    for (size_t i = 0; i < 20; i++) {
        if (array[i] < *min)
            *min = array[i];
        else if (array[i] > *max)
            *max = array[i];
    }
}

void user_input(){
    int array[30];
    int x, y, min, max = 0;
    printf("Enter 22 integers: ");

    //gets input
    for (int i = 0; i < 20; i++){
        if (scanf("%d ", array + i) != 1)
            exit(1);
        // printf("%d\n", *(array + i));
    }
    // get last two digits x and y
    if (scanf(" %d %d", &x, &y) != 2)
        exit(1);

    //find min and max, print result
    min_max(array, &min, &max);
    //printf("%d * %d + %d * %d: %d\n", x, max, y, min, x * max + y * min);
    printf("x * max + y * min: %d\n", x * max + y * min);
}

// void get_file_paths(){
//     char input[100];
//     char output[100];
//     strcpy(input, getenv("HOME"));
//     strcpy(output, input);
//
//     char in_path[100] = "/CLionProjects/Cmput201/Lab3/22_input.txt";
//     char out_path[100] = "/CLionProjects/Cmput201/Lab3/output.txt";
//
//     strcat(input, in_path);
//     strcat (output, out_path);
//     strcpy(INPUT_PATH, input);
//     strcpy(OUTPUT_PATH, output);
// }
//
// void file_input(){
//     int array[30];
//     int x, y, min, max = 0;
//
//     // read file, extract data
//     fptr = fopen(INPUT_PATH,"r");
//     printf("%s", INPUT_PATH);
//
//     if(fptr == NULL) {
//         exit(100); // didn't read file
//     }
//
//     for (int i = 0; i < 20; i++){
//         if (fscanf(fptr, "%d ", array + i) != 1)
//             exit(1);
//         printf("%d\n", *(array + i));
//     }
//     if (fscanf(fptr, " %d %d", &x, &y) != 2)
//         exit(1);
//     fclose(fptr);
//
//     // find min max of array
//     min_max(array, &min, &max);
//
//     // write out to file
//     fptr = fopen(OUTPUT_PATH,"w");
//     if (fptr == NULL)
//         exit(100);
//     fprintf(fptr, "%d * %d + %d * %d: %d", x, max, y, min, x * max + y * min);
//     fclose(fptr);
// }

int main(){
    user_input();
    // Originally thought I would have to read and write files, code left here for reference.
    //get_file_paths();
    //file_input();

    return 0;
}
