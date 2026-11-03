#include <stdio.h>

int duplicate_checker(int array[10]){
    // checks if an array of integer has duplicates, returns 1 if there is dups, else return 0
    for (int i = 0; i < 8; i++){
        for (int j = i + 1; j < 9; j++){
            if (array[i] == array[j])
                return 1;
        }
    }
    return 0;
}

//return 1 for valid, 0 for non-valid setup
int check_row(int board[20][20]){
    int arr[10];
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            arr[j] = board[i][j];
        }
        if (duplicate_checker(arr))
            return 0;
    }
    return 1;
}

int check_col(int board[20][20]){
    int arr[10];
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            arr[j] = board[j][i];
        }
        if (duplicate_checker(arr))
            return 0;
    }
    return 1;
}

int check_box(int board[20][20]){
    int arr[10];
    // move along the board, row the column
    for (int a = 0; a < 9; a += 3) {
        for (int b = 0; b < 9; b += 3){
            int index = 0;

            //build 3x3 boxes
            for (int i = a; i < 3 + a; i++) {
                for (int j = b; j < 3 + b; j++) {
                    arr[index++] = board[i][j];
                }
            }
            if (duplicate_checker(arr))
                return 0;
        }
    }

    return 1;
}

int main(){
    char line[50];
    int board[20][20];

    int r = 0; // row
    for (int i = 1; i < 12; i++){
        fgets(line,sizeof(line), stdin);
        if (i % 4 != 0){ // skip the whitespaces
            int c = 0; // column
            for (int j = 1; j < 12; j++){
                if ((j % 4) != 0) { // skip empty columns
                    int num = line[j-1] - '0'; // translate char from ASCII code to int
                    board[r][c++] = num;
                }
            }
            r++;
        }
    }

    if(check_row(board) && check_col(board) && check_box(board)){
        printf("Valid\n");
    } else{
        printf("Invalid\n");
    }

//    //just to print out the sudoku
//    for (int i = 0; i < 9; i++){
//        for (int j = 0; j < 9; j++){
//            printf("%d ", board[i][j]);
//        }
//        printf("\n");
//    }
}
