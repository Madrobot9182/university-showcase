/*
 * Dice Game by Ryan
 * Input credit - https://stackoverflow.com/questions/5240789/scanf-leaves-the-newline-character-in-the-buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

char get_input(){
    //for fget and sscanf
    const int bufsize = 100;
    char buf[bufsize];
    char input[bufsize];

    //gets input
    fgets(buf, bufsize, stdin);
    sscanf(buf, "%s", input);

    // Checks if valid input
    if ((input[0] != 's' && input[0] != 'r' )|| strlen(input) != 1) exit(1);
    return input[0]; // will either be r or s
}

void three_num_sort(int *a, int *b, int *c){
    //stupid 3 number sort cause I'm lazy
    // start with first 2 numbers
    int temp_sort[4];
    if (*a < *b){
        temp_sort[0] = *a;
        temp_sort[1] = *b;
    } else{
        temp_sort[0] = *b;
        temp_sort[1] = *a;
    }
    //then insert 3rd number between previous two numbers
    if (*c > temp_sort[1]){ // a b c
        temp_sort[2] = *c;
    } else if (*c > temp_sort [0]){ // a c b
        temp_sort[2] = temp_sort[1];
        temp_sort[1] = *c;
    } else { // c a b
        temp_sort [2] = temp_sort[1];
        temp_sort[1] = temp_sort[0];
        temp_sort[0] = *c;
    }
    *a = temp_sort[0];
    *b = temp_sort[1];
    *c = temp_sort[2];
}

int main(){
    //int valid_rolls[10] = {(4,5,6), (1,1,1), (2,2,2), (3,3,3), (4,4,4), (5,5,5), (6,6,6), (1,2,3)};
    int player_scores[3] = {0,0};
    int player_health[3] = {3,3};
    int winner = -1; // which player won
    bool game_over = false;

    // randomize rand seed
    // time_t t;
    // srand((unsigned) time(&t));
    srand(time(NULL));

    while(!game_over){
        // reset scores to 0
        player_scores[0] = 0;
        player_scores[1] = 0;

        //Two loops, an outer player loop and a inner turn loop

        for (int i = 0; i < 2 ; i++){
            for (int turn = 1; turn <= 3 ; turn++){
                //Ask to roll dice
                printf("Turn %d player %d: ", turn, i + 1);
                if (get_input() == 's') break;
                // roll, rand % 6 gives a value from 0 to 5, then add one to create dice values
                int a = (rand() % 6) + 1;
                int b = (rand() % 6) + 1;
                int c = (rand() % 6) + 1;

                three_num_sort(&a, &b, &c); // sort the numbers in order a < b < c

                // calculate points
                if (a == 4 && b == 5 && c == 6) {
                    player_scores[i] = 1000;
                } else if (a == 1 && b == 2 && c == 3) {
                    player_scores[i] = 993;
                } else if (a == b && b == c) {
                    player_scores[i] = (1000 - a); // 999, 998, 997...
                } else {
                    player_scores[i] = (100 * c) + (10 * b) + a;
                }

                //print out roll
                printf("You rolled: %d %d %d = %d points\n", a, b, c, player_scores[i]);
            }
        }

        // calculates winner by comparing scores
        if (player_scores[0] == player_scores[1]){ // tie
            printf("Both players tied this round with %d points\n", player_scores[0]);
        } else {
            winner = (player_scores[0] > player_scores[1]) ? 1 : 2;
            printf("Player %d wins this round with %d points\n", winner, player_scores[winner - 1]);

            if ((player_health[(winner + 2) % 2] -= 1) == 0)// reduce opponent by 1, then check if it's zero
                game_over = true;
        }
        // print final stats
        printf("Player 1 lives = %d\n"
               "Player 2 lives = %d\n\n",
               player_health[0], player_health[1]);
    }
    printf("Player %d wins!\n", winner);
}
