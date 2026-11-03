#include <stdio.h>
#include <stdlib.h>

int main(){
    int input; // for getchar()
    float profits = 0;

    while ((input = getchar()) != '\n' && input != EOF) {
        int is_damaged = 0;
        if (input == '#') is_damaged = 1; // destroyed crop

        input = getchar();
        switch (input) {
            case 'C':
                if (is_damaged == 0)
                    profits += 0.45;
                else
                    profits -= 0.05;
                break;
            case 'T':
                if (is_damaged == 0)
                    profits += 1.00;
                else
                    profits -= 0.25;
                break;
            case 'P':
                if (is_damaged == 0)
                    profits += 2.30;
                else
                    profits -= 0.70;
                break;
            case 'L':
                if (is_damaged == 0)
                    profits += 0.7;
                else
                    profits -= 0.30;
                break;
        }
    }

    char print_str[100];
    if (profits >= 0.0){
        sprintf(print_str, "Net profit: $%.2f", profits);
    } else {
        profits = -(profits); // go from - to + for formatting sake
        sprintf(print_str, "Net profit: -$%.2f", profits);
    }

    printf("%s\n", print_str);
    return 0;
}
