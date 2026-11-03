#include <stdio.h>
#include <stdlib.h>
int LIMIT = 100000;

int round_usd(double value){
    int rounded_output = (int)(value * 10);
    // Get last digit to determine if to round up
    if (rounded_output % 10 >= 5)
        rounded_output += 5;

    return (int)rounded_output / 10;
}

int main() {
    int cad, usd, temp;
    int values[] = {100,50,20,10,5,2,1};

    printf("Enter CAD amount: ");
    if (scanf("%d", &cad) != 1 || cad >= LIMIT || cad < 0) // whole number, within limit
        exit(1);

    usd = round_usd(cad * 0.75);
    printf("Converted USD amount: $%d\n", usd);

    char print_str[10];
    for (int i = 0; i<7; i++){
        // Subtract total by how many x bills can fit, temp stores the remainder.
        temp = usd - ((usd / values[i]) * values[i]);

        // Formats the bill values as $x, "buffers" result to print_str
        snprintf(print_str, 5, "$%d", values[i]);

        //add a margin to bills, then int division to find bill amounts
        printf("%*s bills: %d\n", 4, print_str, usd / values[i]);
        usd = temp;
    }
    return 0;
}
