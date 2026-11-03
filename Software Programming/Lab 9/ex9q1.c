#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_BUFFER 120

/* strtok function referenced from https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
 * Wikipedia referenced for levenshtein and approx. string matching
 * https://en.wikipedia.org/wiki/Levenshtein_distance#Definition
 * https://en.wikipedia.org/wiki/Approximate_string_matching
 * */

int get_dict(char***);
int closest_match(int, const char*, char**);
int levenshtein(const char*, const char*);
int min(int, int, int);

int main(){
    int n, m; scanf("%d %d\n", &n, &m);
    char** dict = NULL;
    char** word = NULL;

    // get dictionary
    get_dict(&dict);
    for (int i = 0; i < m; i++){
        int len = get_dict(&word); // get "dictionary" of words;
        for(int j = 0; j < len; j++){
            int min_index = closest_match(n, word[j], dict);

            printf("%s", dict[min_index]);

            // Jerry-rig the spaces between words but not at end
            if (j != len - 1) printf(" ");
        }
        printf("\n");

        for (int k = 0; k < len; k++)
            free(word[k]); // free mem of individual word each loop
    }
    // Free remaining memory
    for (int i = 0; i < n; i++)
        free(dict[i]);
    free(dict);
    free(word);

    return 0;
}

int get_dict(char*** dict){
    char line[MAX_LINE_BUFFER];
    fgets(line, MAX_LINE_BUFFER, stdin);

    int i = 0;
    for (char* token = strtok(line, " \n"); token != NULL; token = strtok(NULL, " \n")){
        // allocate space for a new string in array
        char **tmp_ptr = (char **)realloc(*dict, (i + 1) * sizeof(char *));
        if (tmp_ptr == NULL)
            exit(1);
        *dict = tmp_ptr;

        // allocate space for string
        int str_len = strlen(token);
        (*dict)[i] = (char *)malloc((str_len + 1) * sizeof(char));

        // Copy the token to dict using strncpy
        // Must Copy or else strtok will destroy token each iteration
        strncpy((*dict)[i], token, str_len + 1);
        i++;
    }
    return i; // size of dict
}

int min(const int a, const int b, const int c){
    return (a < b) ? (a < c ? a : c) : (b < c ? b : c);
}

int levenshtein(const char* s1, const char* s2){
    // Implimentation referenced from various sources, including Wikipedia as stated above.
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    // Build the lev matrix
    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            // Bottom 2 build the outer rows
            // ie. Represents going from an empty string to the other
            if (i == 0) {
                matrix[i][j] = j;
            } else if (j == 0) {
                matrix[i][j] = i;
                // If previous letter is identical, copy upper left diagonal box
            } else if (s1[i - 1] == s2[j - 1]) {
                matrix[i][j] = matrix[i - 1][j - 1];
            } else { // add 1 plus the smallest value between the box on the left, top, and up left diag
                matrix[i][j] = 1 + min(matrix[i - 1][j], matrix[i][j - 1], matrix[i - 1][j - 1]);
            }
        }
    }

    return matrix[len1][len2]; // bottom right is the final distance
}

int closest_match(const int n, const char* word, char** dict){
    // Function to find the closest match to the word using levenshtein's.
    // First run it on last element to get starting values
    int min_distance = levenshtein(word, dict[0]);
    int min_index = 0;

    // Iterate through whole dictionary, finding the smallest distance and setting that as the closest match
    for (int i = 1; i < n; i++){
        int distance = levenshtein(word, dict[i]);

        if (distance < min_distance){ // so that the front most element in dict is prioritize
            min_distance = distance;
            min_index = i;
        }
    }

    return min_index;
}
