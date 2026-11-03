#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./build_in.h"
#include "./external.h"
#include "./signals.h"
#include "./constants.h"

enum COMMAND_ENUM { CD, PWD, EXIT };
const char* const COMMAND_STRING[MAX_COMMAND] = {"cd", "pwd", "exit"};


/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
int tokenize(char* str, const char* delim, char ** argv){
  char* token;
  size_t i;
  token = strtok(str, delim);

  for(i = 0; token != NULL; ++i){
    argv[i] = token;
    token = strtok(NULL, delim);
  }
  argv[i] = NULL;
  return i;
}


/**
 * @brief runs the inputted command
 *
 * @param argv - A char* array that will contain the tokenized strings
 * @param arg_count - The number of tokens in argv
 * @param exit - a flag to indicates whether to continue running the shell
 */
void run_command(char** argv, int arg_count, int* exit) {
  int command = -1;
  int pipe_exist = -1;
  int adr_exist = -1;
  int arr_index[arg_count];

  // First check if command is one of the built in commands
  for (size_t i = 0; i < MAX_COMMAND; i++) {
    if (strcmp(COMMAND_STRING[i], argv[0]) == 0){
      // command is one of the predefined options
      command = i;
      break;
    }
  }

  switch (command){
    case CD:
      cd(argv[1]);
      break;
    case PWD:
      pwd();
      break;
    case EXIT:
      *exit = exit_shell();
      break;
    default:
      // Checking for which kind of command we're running
      pipe_exist = find_token(argv, "|");
      adr_exist = find_token(argv, "&");

      if (find_token(argv, ">") != -1 || find_token(argv, "<") != -1) {
        find_arrow_token(argv, arr_index);
        redirected_process(argv, arr_index, adr_exist);
      }

      else if (pipe_exist != -1)
        pipe_process(argv, pipe_exist);

      else
        run_process(argv, adr_exist, 1); // normal process with bg print

      break;
    }
}


/**
 * @brief Main function. Holds the main logic of the shell
 *
 * @param argc - A int that indicates how many arguments are in argv
 * @param argv - A char* array that will contain the tokenized strings
 */
int main(int argc, char *argv[]){
  // Set the handlers
  set_handlers();

  // Initialize the strings
  char* delim = " \n\t";

  char* input_line = (char*) malloc(LINE_LENGTH * sizeof(char));
  if (input_line == NULL) {
    perror("Memory allocation failed\n");
    return 1;
  }

  // Initialize string array and each individual string
  char** tokenized_array = (char**) malloc(MAX_TOKENS * sizeof(char*));
  if (tokenized_array == NULL) {
    perror("Memory allocation failed\n");
    return 1;
  }

  // Print the welcome message!
  printf("Welcome to Dragon Shell!\n\n");

  // Main loop of the program. All inputs are read and executed here
  int is_running = 1;
  do {
    // Print the prompt
    printf("dragonshell > ");

    // Read input into input line, check if read successfully
    if(fgets(input_line, LINE_LENGTH, stdin) != NULL) {

      // Call tokenize to get all the arguments
      int arg_count = tokenize(input_line, delim, tokenized_array);

      // Skip if user entered nothing
      if (tokenized_array[0] == NULL)
        continue;

      // Run the given command
      run_command(tokenized_array, arg_count, &is_running);

    // Else we failed to read the line (signals)
    } else {
      printf("\n");
      if (feof(stdin)) // Ctrl D
        is_running = exit_shell(); // Exit the loop
    }

  } while(is_running);

  // Free the strings before exiting
  free(input_line);

  free(tokenized_array);

  _exit(0);
}
