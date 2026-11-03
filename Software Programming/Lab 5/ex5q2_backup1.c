/* Purpose: Solving magic square
 * Author: Ryan
 * Date:
 * References: https://www.geeksforgeeks.org/sudoku-backtracking-7/ reference on backtracking algorithms
 */

#include "lab05.h"

// Do not touch anything in this main function (used for testing purposes)
int main(void) {
    return test_magic(magic);
}
int check_dupes(int n, int const arr[n]){
    // Checks for duplicates in an array, returns 1 if there is duplicates
    // We ignore 0's in dupe check as these are values we eventually replace
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (arr[i] == arr[j])
                if(arr[i] != 0)
                    return 1;
    return 0;
}

int check_row(int n, int grid[n][n], int row, int true_sum){
    // check if a given column is valid or not
    int hash[n], sum = 0;
    for (int i = 0; i < n; i++){
        hash[i] = grid[row][i];
        sum += hash[i];
    }

    printf("row sum %d\n", sum);
    if(check_dupes(n, hash)) return -1;// duplicate numbers
    if(sum > true_sum) return -1;
    // else
    return sum;
}

int check_col(int n, int grid[n][n], int col, int true_sum){
    // same is check_row, but for the columns
    int hash[n], sum = 0;
    for (int i = 0; i < n; i++){
        hash[i] = grid[i][col];
        sum += hash[i];
    }

    if(check_dupes(n, hash)) return -1;// duplicate numbers
    if(sum > true_sum) return -1;
    // else
    return sum;
}

int check_diag(int n, int grid[n][n], int diag_code, int true_sum){
    // same as check row but for the diagonal, diag_code checks if we want the \ diag (0) or the / diag (1)
    int hash[n], sum = 0;
    int i = (diag_code == 0) ? 0 : n-1;

    while(i != -1 && i != n){
        hash[i] = grid[i][i];
        sum += hash[i];
        (diag_code == 0) ? i++ : i--;
            // swap diagonal depending which way we want it
    }

    if(check_dupes(n, hash)) return -1;// duplicate numbers
    if(sum > true_sum) return -1;
    // else
    return sum;
}

int find_empty(int n, int grid[n][n], int loc[2]){
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (grid[i][j] == 0){
                loc[0] = i, loc[1] = j;
                return 1;
            }
    return 0; // no empty found
}


int valid_placement(int n, int grid[n][n], int row, int col, int num){
    int true_sum = n * (1 + (n * n)) / 2;

    // // temporary set the empty spot to num for check functions
    // grid[row][col] = num;
    //
    // // checks if inserting num is a valid placement
    // // check is if row/col/diag sum under true sum and not a duplicate
    // if(check_col(n, grid, col, true_sum) != -1 && check_row(n, grid, row, true_sum) != -1) {
    //     // test if we are on a diagonal
    //     if((row - col) == 0) // negative diagonal (\)
    //         if(!check_diag(n, grid, 0, true_sum)) return 0;
    //     if((row + col) == n - 1) // positive diagonal /
    //         if(!check_diag(n, grid, 1, true_sum)) return 0;
    //     printf("valid = success\n");
    //
    //     //check for dupes in whole grid
    //     int hash[n*n], h = 0;
    //     for (int i = 0; i < n; i++)
    //         for (int j = 0; j < n; j++)
    //             hash[h++] = grid[i][j];
    //     if (check_dupes(n*n, hash)) return 0;
    //
    //     return 1; //not on diagonal but still valid
    // }
    //
    // return 0;
    grid[row][col] = num;

    if(check_row(n, grid, row, true_sum) == -1 || check_col(n, grid, col, true_sum) == -1) return 0;

    if((row - col) == 0) // negative diagonal (\)
        if(check_diag(n, grid, 0, true_sum) == -1) return 0;
    if((row + col) == n - 1) // positive diagonal /
        if(check_diag(n, grid, 1, true_sum) == -1) return 0;

    int hash[n*n], h = 0;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                hash[h++] = grid[i][j];
    if (check_dupes(n*n, hash)) return 0;

    printf("valid = success\n");
    return 1;

    // printf("valid = failure\n"); // TODO: never gets here???
    // grid[row][col] = 0; // not valid, set back to 0
    // return 0;
}

void print_square(int n, int grid[n][n]){
    for (int i = 0; i < n ; i++){
        for (int j = 0; j < n; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

// This function is a non-recursive function that checks whether a given grid[n][n] is a magic square.
// Complete the function definition:
int check(int n, int grid[n][n]) {
    int true_sum = n * (1 + (n * n)) / 2;

    // Check diagonal
    if (check_diag(n, grid, 0, true_sum) != true_sum || check_diag(n, grid, 1, true_sum) != true_sum){
        printf("CHECK DIAG \\: %d / %d\n", check_diag(n, grid, 0, true_sum), check_diag(n, grid, 0, true_sum));

        return 0;
    }

    // Check rows/columns
    for (int i = 0; i < n; i++) {
        if (check_row(n, grid, i, true_sum) != true_sum || check_col(n, grid, i, true_sum) != true_sum){
            printf("CHECK R/C Row: %d Col %d\n", check_row(n, grid, 0, i), check_col(n, grid, 0, i));
            return 0;
        }
    }

    //check duplicates
    int hash[n*n], h = 0;
    for (int i = 0; i < n ; i++)
        for (int j = 0; j < n; j++)
            hash[h++] = grid[i][j];
    printf("DUPE CHECK\n");
    if (check_dupes(n*n, hash)) return 0;
    return 1; //  Valid magic box

}

// This function is a recursive function that intends to solve a given grid[n][n] as in the lab05 description.
// Complete the function definition:
int magic(int n, int grid[n][n]) {
    print_square(n, grid);
    if (check(n, grid))
        return 1;

    int loc[2] = {0,0}; // row, col
    if(!find_empty(n, grid, loc)) // find location of 0
        return 0; // all spots tested, no solution

    int row = loc[0];
    int col = loc[1];

    printf("Magic Row: %d Col %d\n", row, col);

    for (int num = 1; num <= n*n; num++){
        if (valid_placement(n, grid, row, col, num)){ // Check if placement is valid

            grid[row][col] = num;
            printf("Num: %d\n", row);

            if (magic(n, grid)) // recursive bit, returns true when success
                return 1;
        }
        //else - set back to 0 and try again
        grid[row][col] = 0;
    }
    return 0; // Starts backtracking
}
