#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

void selectionSort(float arr[], const int n){
    // selection sort - referenced from geekforgeek selection sort in c
    for (int i = 0; i < (n - 1); i++) {
        int min = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j] < arr[min])
                min = j;

       //swap min to front
        if (min != i){
            float temp = arr[min];
            arr[min] = arr[i];
            arr[i] = temp;
        }
    }
}

int binarySearch(const float arr[], const float x, int l, int h) {
    if (x >= arr[h-1]) // x is greater than all elements aka truck can carry all h boxes
        return h;

    while (l <= h) {
        int mid = l + (h - l) / 2;;
        //Find the index with the value just under x, giving us max box count
        if (arr[mid - 1] <= x && arr[mid] >= x)
            return mid;

        // binary search
        if (arr[mid] < x)
            l = mid + 1;
        else
            h = mid - 1;
  }
  return 0; // when x <= arr[0]
}


int main() {
    int n; scanf("%d", &n);
    float boxes[n], summed_boxes[n];

    // get the box values
    for (int i = 0; i < n; i++){
        scanf("%f", &boxes[i]);
    }

    selectionSort(boxes, n); // sort the boxes ascending

    // summed_boxes[i] is the sum of all elements (i -> 0) of the sorted boxes
    // its index corrosponds to the number of summed boxes
    for (int i = 0; i < n; i++){
        summed_boxes[i] = 0;
        for(int j = 0; j <= i; j++){
            summed_boxes[i] += boxes[j];
        }
    }

    // The index of summed_array gives the box total
    float truck_weight;
    int box_total;
    while (scanf("%f", &truck_weight) == 1){
        box_total = binarySearch(summed_boxes, truck_weight, 0, n);
        printf("%d\n", box_total);
    }

    return 0;
}
