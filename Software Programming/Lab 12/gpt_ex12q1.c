#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef uint32_t NewFloat;

void nfloat_debug(NewFloat f); //part 1

NewFloat float_to_nfloat(float f); //part 2
void bit_shift_copy(NewFloat *dest, uint32_t *source);

void nfloat_print(NewFloat f); //part 3

NewFloat nfloat_double(NewFloat f); //part 4
NewFloat nfloat_add(NewFloat a, NewFloat b);

NewFloat bits_string_to_nfloat(char *str);


int main(void) {
    int part_num;
    char s1[40];
    float f1, f2;
    NewFloat n1, n2;

    // This will scan the input for you! Uncomment pieces once you implement
    // the functions for that part
    while (1) {
        int res = scanf(" p%d", &part_num);

        if (res != 1) {
            break;
        } else if (part_num == 1) {
            scanf("%s", s1);
            n1 = bits_string_to_nfloat(s1);
            nfloat_debug(n1);
        } else if (part_num == 2) {
            scanf("%f", &f1);
            n1 = float_to_nfloat(f1);
            nfloat_debug(n1);
        } else if (part_num == 3) {
            scanf("%s", s1);
            n1 = bits_string_to_nfloat(s1);
            nfloat_print(n1);
        } else {
            scanf("%s ", s1);

            if (s1[0] == 'd') {
                scanf("%f", &f1);
                n1 = float_to_nfloat(f1);
                n1 = nfloat_double(n1);
            } else {
                scanf("%f %f", &f1, &f2);
                n1 = float_to_nfloat(f1);
                n2 = float_to_nfloat(f2);
                n1 = nfloat_add(n1, n2);
            }

            nfloat_print(n1);
        }
    }
}

void nfloat_debug(NewFloat f){
    NewFloat n;
    memcpy(&n, &f, sizeof(NewFloat));

    // Sign
    printf("%u", (n >> 31) & 1);
    printf(" ");

    // Exponent
    for(int k = 30; k >= 26; k--)
        printf("%u", (n >> k) & 1);
    printf(" ");

    // Manisa
    for(int k = 25; k >= 0; k--)
        printf("%u", (n >> k) & 1);

    printf("\n");
}

NewFloat bits_string_to_nfloat(char *str){
    NewFloat f = 0;

    // Shift the bits leftward, each time setting the first (rightmost) bit to 1 when str[i] == 1
    // |= 1 = |= 000....0001, only sets the right bit to 1s
    for (int i = 0; str[i] != '\0'; i++) {
        f <<= 1;  // Left shift by 1 bit
        if (str[i] == '1') {
            f |= 1;  // Set the least significant bit
        }
    }
    return f;
}

NewFloat float_to_nfloat(float f){
    uint32_t floatAsInt;
    memcpy(&floatAsInt, &f, 4);
    // Extracting parts of the float
    uint32_t signBit = (floatAsInt >> 31) & 1;
    uint32_t exponent = (floatAsInt >> 23) & 0xFF;
    uint32_t fraction = floatAsInt & 0x7FFFFF;

    // Adjust the bits to fit the NewFloat format
    NewFloat nf = (signBit << 31) | ((exponent - 127 + 15) << 26) | fraction;

    return nf;
}

void bit_shift_copy(NewFloat *dest, uint32_t *source){
    // Helper function to copy the next bit from source to dest
    uint32_t mask = 0b10000000000000000000000000000000;
    *dest <<= 1;  // Left shift by 1 bit

    if ((*source & mask) == mask) { // checking if the leftmost bit is 1
        *dest |= 1;  // Set the least significant bit
    }
    *source <<= 1;
}

void nfloat_print(NewFloat f){
    uint32_t signBit = (f >> 31) & 1;
    uint32_t exponent = (f >> 26) & 0x1F;
    uint32_t fraction = f & 0x3FFFFFF;

    NewFloat floatBits = (signBit << 31) | ((exponent + 127 - 15) << 23) | fraction;
    nfloat_debug(floatBits);
    float result;
    memcpy(&result, &floatBits, sizeof(float));
    printf("%f\n", result);
}

NewFloat nfloat_double(NewFloat f){
    // Implement the logic for doubling the NewFloat value
    // You can adjust the exponent accordingly
    uint32_t exponent = (f >> 26) & 0x1F;
    if (exponent == 0x1F) {
        // Handle special case for infinity or NaN
        // You may choose to handle this differently based on your requirements
        return f;
    }

    return f + (1 << 26); // Incrementing the exponent by 1 for doubling
}

NewFloat nfloat_add(NewFloat a, NewFloat b){
    // Implement the logic for adding two NewFloat values
    // You need to align the exponents and then add the fractions
    uint32_t exponent_a = (a >> 26) & 0x1F;
    uint32_t exponent_b = (b >> 26) & 0x1F;

    // Aligning the exponents
    if (exponent_a > exponent_b) {
        b >>= (exponent_a - exponent_b);
    } else {
        a >>= (exponent_b - exponent_a);
    }

    // Adding the fractions
    NewFloat result = a + b;

    return result;
}
