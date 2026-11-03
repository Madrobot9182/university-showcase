#include "ex6q1.h"

#define LEN 200000000
uint32_t MEMO[LEN];

// This function should be pure recursion - no dynamic programming allowed
uint32_t exp_mod_1(uint32_t base, uint32_t exp, uint32_t modulo) {
    EXP_MOD_1_CALL_COUNT += 1;

    if (exp == 1)
        return base % modulo;

    uint32_t exp1, exp2;
    if(exp % 2 == 0) //even
        exp1 = exp2 = exp / 2;
    else { // is odd
        exp1 = exp/2;
        exp2 = exp1 + 1;
    }

    return (exp_mod_1(base, exp1, modulo) * exp_mod_1(base, exp2, modulo)) % modulo;
}

// This function should store the values of previous calls to exp_mod_2 in an
// array and use them for later calculations.
uint32_t exp_mod_2(uint32_t base, uint32_t exp, uint32_t modulo) {
    EXP_MOD_2_CALL_COUNT += 1;

    if (MEMO[exp-1])// not empty
        return MEMO[exp-1];

    if (exp == 1){
        MEMO[0] = base % modulo;
        return MEMO[0];
    }

    uint32_t exp1, exp2;
    if(exp % 2 == 0) //even
        exp1 = exp2 = exp/2;
    else{ // is odd
        exp1 = exp/2;
        exp2 = exp1 + 1;
    }
    // First occurance - add result to list
    MEMO[exp-1] = (exp_mod_2(base, exp1, modulo) * exp_mod_2(base, exp2, modulo)) % modulo;
    return MEMO[exp-1];
}

int main(void) {
    uint32_t base, exp, modulo;

    if (scanf("%d %d %d", &base, &exp, &modulo) != 3) {
        return 1;
    }

    uint32_t mod_1 = exp_mod_1(base, exp, modulo);
    uint32_t mod_2 = exp_mod_2(base, exp, modulo);

    fprintf(stdout, "exp_mod_1 calls: %d\n", EXP_MOD_1_CALL_COUNT);
    fprintf(stdout, "exp_mod_2 calls: %d\n", EXP_MOD_2_CALL_COUNT);
    fprintf(stdout, "%d == %d\n", mod_1, mod_2);
	return 0;
}
