#include "ex6q2.h"

// Please don't change anything in this main function!
int main(void) {
    if (!test_bubble(bubble_sort))
        fprintf(stderr, "Bubble sorting failed\n");
    if (!test_gnome(gnome_sort))
        fprintf(stderr, "Gnome sorting failed\n");
    if (!test_insertion(insertion_sort))
        fprintf(stderr, "Insertion sorting failed\n");
    if (!test_custom(custom_sort)) {
        fprintf(stderr, "Custom sorting failed\n");
	}
	return 0;
}

void swap(int arr[], int a, int b){
    int temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}

void bubble_sort(int n, int arr[n]) {
    int sorted = 0;
    while(!sorted){
        for (int i = 0; i < n - 1 ; i++){
            sorted = 1; // if no elements are moved, then array is sorted
            for (int j = 0; j < n - 1 - i; j++){
                if (arr[j] > arr[j + 1]){
                    swap(arr, j, j + 1);
                    sorted = 0;
                }
            }
        }
    }
}

// Not very well known, but very easy!
// Check out the description on here https://en.wikipedia.org/wiki/Gnome_sort
void gnome_sort(int n, int arr[n]) {
    // referenced the Wikipedia article above
    int pos = 0;
    while(pos < n){
        if (pos == 0 || arr[pos] >= arr[pos - 1]){
            pos++;
        } else{
            swap(arr, pos, pos - 1);
            pos--;
        }
    }
}

// referenced https://www.geeksforgeeks.org/insertion-sort/
void insertion_sort(int n, int arr[n]) {
    for (int i = 1; i < n; i++){
        int key = arr[i]; // number for comparison
        int j = i - 1;

        while(j >= 0 && arr[j] > key){ // descend the list to find smaller value
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key; // put back at top of moved area
    }
}

int sorted(int n, int arr[n]){
    for (int i = 0; i < n - 1; i++){
        if (arr[i] > arr[i+1])
            return 0;
    }
    return 1;
}


// The sort you found! (See lab description for details)
void custom_sort(int n, int arr[n]) {
/*
 * Behold: Bogo sort! I swear this works, (at least for smaller n < 10ish), more info in the report
 * Oh yeah, guess I also cited - https://www.geeksforgeeks.org/bogosort-permutation-sort/
 */
    #include <stdlib.h>
    #include <time.h>

    time_t t;
    srand((unsigned) time(&t));
    while(!sorted(n, arr)){ // if list isn't sorted
        // Now it gets fun
        for(int i = 0; i < n; i++){
            swap(arr, i, rand() % n); // swap random elements around n times
        }
    }
}
