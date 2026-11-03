#include "ex8q1.h"

void print_array(int size, int* arr);
float* float_array_insert(int insert, float value, int size, float *arr);
int* int_array_insert(int insert, int value, int size, int* arr);

int main(void) {
    int is_print, value, size = 0;
    float index;

    float *indices = NULL;
    int *values = NULL;
    while (!feof(stdin)) {
        // Use the read_line function from ex7q1.h here! Do not try to parse the
        // input yourself (that's a lot harder)

        read_line(&is_print, &index, &value);
        //fprintf(stderr, "%d %0.4f %d\n", is_print, index, value);  // You can remove this

        if(is_print)
            print_array(size, values);

        else {
            // Time to add values
            // First check if index already exist
            int exist = 0;
            for (int i = 0; i < size; i++){
                if (indices[i] == index){
                    values[i] = value;
                    exist = 1;
                    break;
                }
            }

            if(!exist){
                int i;
                for (i = 0; i < size; i++){
                    if (index < indices[i]) // finds larger element, insert to its left
                        break; // stop search
                } // else reaching end of loop means our index is the biggest
                indices = float_array_insert(i, index, size, indices);
                values = int_array_insert(i, value, size, values);
                size++;
            }
        }
        //debugging
        // printf("\n---------------------\n");
        // printf("size = %d\n", size);
        // printf("Indices:\n");
        // for (int i = 0; i < size; i++)
        //     printf("%f ", indices[i]);
        //
        // printf("\nvalues:\n");
        // for (int i = 0; i < size; i++)
        //     printf("%d ", values[i]);
        // printf("\n---------------------\n");

    }
    // free memory
    free(indices);
    free(values);
    return 0;
}
void print_array(int size, int* arr){
    if(size == 0){
        printf("[]\n");
    } else {
        printf("[");
        for (int i = 0; i < size - 1; i++){
            printf("%d, ", arr[i]);
        }
        printf("%d]\n", arr[size - 1]);
    }
}

float* float_array_insert(int insert, float value, int size, float *arr){
    //allocate new array one larger than before
    float* temp_ptr = realloc(arr, (size + 1) * sizeof(float));
    if (temp_ptr == NULL)
        exit(1);

    for(int i = size; i > insert; i--)
        temp_ptr[i] = temp_ptr[i - 1];

    temp_ptr[insert] = value;
    return temp_ptr;
}

int* int_array_insert(int insert, int value, int size, int* arr){
    //allocate new array one larger than before
    int* temp_ptr = realloc(arr, (size + 1) * sizeof(int));
    if (temp_ptr == NULL)
        exit(1);

    for(int i = size; i > insert; i--)
        temp_ptr[i] = temp_ptr[i - 1];

    temp_ptr[insert] = value;
    return temp_ptr;
}
