#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_BUFFER 120

void add(char*, char*);
void cat(char*, char*);
void rot(char*, char*);


int main(){
    int n; scanf("%d", &n);
    for(int i = 0; i < n; i++) {
        char lhs[MAX_LINE_BUFFER];
        char rhs[MAX_LINE_BUFFER];
        char op[4];
        scanf("%s %s %s", lhs, op, rhs);

        //strcmp returns 0 when matched???
        if (!strcmp(op, "+"))
            add(lhs, rhs);
        else if(!strcmp(op, "@"))
            cat(lhs, rhs);
        else
            rot(lhs, rhs);
        }
    return 0;
}

void add(char* lhs, char* rhs){
    int lhs_len = strlen(lhs), rhs_len = strlen(rhs);
    int max_len = (lhs_len > rhs_len) ? lhs_len : rhs_len;

    char* result = (char*)malloc((max_len + 2) * sizeof(char));
    int carry = 0;
    int i = lhs_len - 1, j = rhs_len - 1;
    int k = 0;

    // Adding while either lhs/rhs has a digit, or we have a carry
    while (i >= 0 || j >= 0 || carry) {
        int digit1 = (i >= 0) ? (lhs[i] - '0') : 0;
        int digit2 = (j >= 0) ? (rhs[j] - '0') : 0;

        int sum = digit1 + digit2 + carry; // sum value
        carry = sum / 10; // carry value used in next iteration

        // Add digit to result, carry will already be accounted for
        result[k++] = (sum % 10) + '0';

        if (i >= 0) i--;
        if (j >= 0) j--;
    }

    // add null to end, also stopping memory leaks ;(
    result[k] = '\0';

    // reverse result
    int start = 0;
    int end = k - 1;
    while (start < end){
        char temp = result[start];
        result[start] = result[end];
        result[end] = temp;
        start++; end--;
    }

    printf("%s\n", result);
    free(result);
}

void cat(char* lhs, char* rhs){
    strcat(lhs, rhs);
    printf("%s\n", lhs);
}

void rot(char* lhs, char* rhs){
    int rot_num; sscanf(rhs, "%d", &rot_num);
    int len = strlen(lhs);
    rot_num = rot_num % len; // rot_num mod len(num), only need first "loop"

    for(int i = 0; i < rot_num; i++){
        char tmp = lhs[0];

        for(int j = 0; j < len - 1; j++)
            lhs[j] = lhs[j+1];

        lhs[len-1] = tmp;
    }
    //count the number of leading zero, excluding the number 0 itself
    int k = 0;
    if(len != 1) while(lhs[k] == '0') k++;

    for (int i = k; i < len; i++)
        printf("%c", lhs[i]);
    printf("\n");
}
