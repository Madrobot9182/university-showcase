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
                //n1 = float_to_nfloat(f1);
                //n1 = nfloat_double(n1);
            } else {
                scanf("%f %f", &f1, &f2);
                //n1 = float_to_nfloat(f1);
                //n2 = float_to_nfloat(f2);
                //n1 = nfloat_add(n1, n2);
            }

            //nfloat_print(n1);
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

    //Manisa
    for(int k = 25; k >= 0; k--)
        printf("%u", (n >> k) & 1);

    printf("\n");
}


NewFloat bits_string_to_nfloat(char *str){
    NewFloat f = 0;

    // Shift the bits leftwords, each time setting the first (rightmost) bit to 1 when str[i] == 1
    // |= 1 = |= 000....0001, only sets right bit to 1s
    for (int i = 0; str[i] != '\0'; i++) {
        f <<= 1;  // Left shift by 1 bit
        if (str[i] == '1') {
            f |= 1;  // Set the least significant bit
        }
    }
    return f;
}


// NewFloat float_to_nfloat(float f){
//     NewFloat nf = 0;
//     uint32_t tmp;
//     memcpy(&tmp, &f, 4);
//     //nfloat_debug(tmp);
//
//     // sign 1, shift once
//     bit_shift_copy(&nf, &tmp);
//
//     // TODO: understand how to actually translate 8 bits to 5, can't just trim
//     // epxonent 8 in float, 5 in newfloat, shift 5 times.
//     // for (int i = 0; i < 5; i++)
//     //     bit_shifter(&nf, &tmp);
//     // tmp <<= 3; // shift the float 3 bits to ignore
//     for (int i = 0; i < 4; i++)
//         bit_shift_copy(&nf, &tmp);
//     tmp <<= 3; // shift the float 3 bits to ignore
//     bit_shift_copy(&nf, &tmp);
//
//     // 23 in float but 26 in newfloat, shift 3 additional times at end
//     for (int i = 0; i < 23; i++)
//         bit_shift_copy(&nf, &tmp);
//     nf <<= 3; // Add 3 zeros to end
//
//     return nf;
// }

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

void bit_shift_copy(NewFloat *dest, NewFloat *source){
    // Helper function to copy the next bit from source to dest
    uint32_t mask = 0b10000000000000000000000000000000;
    *dest <<= 1;  // Left shift by 1 bit

    if ((*source & mask) == mask) { // checking if leftmost bit is 1
        *dest |= 1;  // Set the least significant bit
    }
    *source <<= 1;
}


// void nfloat_print(NewFloat f){
//     //left shifting, read 26 masina, 5 exp, 1 sign, convert to float and solve
//     // int sign = (f >> 31) & 1;
//     // f <<= 1;
//     uint32_t sign = 0;
//     bit_shift_copy(&sign, &f);
//
//     // Exponent, recall -15 mean f is 0
//     int exp = 0;
//     for(int i = 0; i < 5; i++)
//         bit_shift_copy(&exp, &f);
//     // exp = exp - 15;
//
//     //mantissa
//     uint32_t mantissa = 0;
//     for(int i = 0; i < 26; i++)
//         bit_shift_copy(&mantissa, &f);
//
//     printf("s = %d, m = %u and e = %d ", sign, mantissa, exp);
//
//     double power;
//     if (exp >= 0) power = 1 << exp;
//     else power = 1.0 / (1 << exp);
//     printf("Power = %f = ", power);
//
//     double result = mantissa * power;
//     printf("%f\n", result);
// }


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
