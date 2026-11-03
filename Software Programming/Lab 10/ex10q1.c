#include "ex10q1.h"


int main(){
    Ring *front_half = init_empty_ring();
    Ring *back_half = init_empty_ring();

    char line[50];
    char command;
    int x, id, temp;
    while(fgets(line, 50, stdin) != NULL){
        sscanf(line, "%c %d %d", &command, &x, &id);
        switch(command){
            case 'L':
                // Last person leaves
                if(back_half->length > 0) pop_back(back_half);
                else(pop_back(front_half));
                break;
            case 'F':
                // Front guy gets autograph
                if(front_half->length > 0) temp = pop_front(front_half);
                else temp = pop_front(back_half);
                if (temp != -1) printf("%d\n", temp);
                break;
            case 'N':
                //enque normie if less than x in both queue
                if (front_half->length + back_half->length < x)
                    push_back(back_half, id);
                break;
            case 'C':
                // enque cool cmput guy if less than x in front queue
                if (front_half->length < x)
                    push_back(front_half, id);
                break;
        }

        rebalance(front_half, back_half);
    }

    free_ring(front_half);
    free_ring(back_half);
    return 0;
}
