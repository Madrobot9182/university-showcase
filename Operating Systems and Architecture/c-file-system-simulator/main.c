#include "fs-sim.h"

extern Superblock* super_block;     // Used to free the super_block in main
extern int file_system;             // Used to close the File descriptor of the mounted file
extern char* disk_name;             // Used to free the disk_name

/**
 * @brief Tokenize a C string
 * Credit to Dragonshell from CMPUT379 F24 lab 1
 *
 * @param str - The C string to tokenize
 * @param delim - The C string containing delimiter character(s)
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
int tokenize(char* str, const char* delim, char ** argv){
    char* token;
    int i = 0;

    // Special case for Buffer command
    if (str[0] == 'B' && str[1] == ' ') {
        argv[i++] = str;        // First token is 'B'
        argv[i++] = str + 2;    // Will be parsed in fs_buffer

        return i;
    }

    token = strtok(str, delim);
    for(i = 0; token != NULL; ++i){
    if (i < MAX_ARGS) {
        // Skip adding into unallocated space, let error checker handle it
        argv[i] = token;
    }

    token = strtok(NULL, delim);
    }

    return i;
}

/**
 * Generalized function to check if each inputted argument is valid before calling
 * @param file_name - The current file name where inputs are taken
 * @param line_count - The current line the program is on
 * @param args - The arguments itself
 * @param arg_count - The number of arguments
 * @param arg_req - Correct number of arguments
 * @param max_length - Max length of argument string, set to -1 if no strings needed
 * @param max_valid_index - The second int argument max value. Set to -1 if not used;
 */
int validate_args (char* file_name, int line_count, char** args, int arg_count, int arg_req, int max_length, int max_valid_index) {
    int valid = 1;
    if (arg_count != arg_req) valid = 0;

    if (valid) {
        switch(arg_req) {   // 2 args also check 1 args but not the other way around
        case 2:         // All second arguments are ints with limits on size
            if (max_valid_index != 1 && (atoi(args[2]) < 0 || atoi(args[2]) > max_valid_index)) valid = 0;
        case 1:         // All first arguments are filename [5]
            if (max_length != -1 && strlen(args[1]) > max_length) valid = 0;
        case 0:         // Ensure correct amount of args
            break;
        }
    }

    if (!valid) {
        fprintf(stderr, "Command Error: %s, %d\n", file_name, line_count);
    } else if (strncmp(args[0], "M", 1) && file_system == -1) {
        fprintf(stderr, "Error: No file system is mounted\n");
        valid = 0;
    }
    return valid;
}

/**
 * Fix a string to be exactly 5 characters, padding with nulls if shorter
 * @param dest - destination buffer (of size 5)
 * @param src - source string
 */
void fix_string(char* dest, const char* src) {
    // Clear the destination first
    memset(dest, '\0', 5);

    // Copy up to 5 characters from source
    size_t len = strlen(src);
    memcpy(dest, src, 6);
    while (len < 5) {
        dest[len++] = '\0';
    }
}

/**
 *
 * @brief Main function. Reads the file and executes all commands within the virtual file system
 *
 * @param argc - A int that indicates how many arguments are in argv
 * @param argv - A char* array that will contain the tokenized strings
 */
int main(int argc, char *argv[]) {
    // Check if we have a valid input
    if (argc == 0) {
        perror("No input file given");
        return 1;
    }

    char* file_name = argv[1];
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
        perror("Invalid file");
        return 1;
    }

    int line_count = 1;
    char input[2048];
    char fixed_string[5];   // For functions that only need a fixed string as args
    char** args = malloc(sizeof(char*) * MAX_ARGS);
    super_block = malloc(sizeof(Superblock));   // Initialize the super_block
    while(fgets(input, 2048, fp) != NULL) {

        memset(fixed_string, '\0', 5);  // Flush the string

        // Fetch all arguments"
        int arg_count = tokenize(input, " \n\t", args) - 1; // Count # arguments passed into function

        switch(input[0]){
            case ('M'):                                 // Mount file system <disk name>
                if (validate_args(file_name, line_count, args, arg_count, 1, -1, -1))
                    fs_mount(args[1]);
                break;
            case ('C'):                                 // Create a file <file name> <file size>
                if (validate_args(file_name, line_count, args, arg_count, 2, 5, 127)) {
                    fix_string(fixed_string, args[1]);
                    fs_create(fixed_string, atoi(args[2]));
                }
                break;
            case ('D'):                                 // Delete a file <file name>
                if (validate_args(file_name, line_count, args, arg_count, 1, 5, -1)) {
                    fix_string(fixed_string, args[1]);
                    fs_delete(fixed_string);
                }
                break;
            case ('R'):                                 // Read from a file <file name> <block number>
                if (validate_args(file_name, line_count, args, arg_count, 2, 5, 126)) {
                    fix_string(fixed_string, args[1]);
                    fs_read(fixed_string, atoi(args[2]));
                }
                break;
            case ('W'):                                 // Write to a file <file name> <block number>
                if (validate_args(file_name, line_count, args, arg_count, 2, 5, 126)) {
                    fix_string(fixed_string, args[1]);
                    fs_write(fixed_string, atoi(args[2]));
                }
                break;
            case ('B'):                                 // Update buffer <new buffer characters>
                if (validate_args(file_name, line_count, args, arg_count, 1, 1024, -1))
                    fs_buff(args[1]);
                break;
            case ('L'):                                 // List files and directories
                if (validate_args(file_name, line_count, args, arg_count, 0, -1, -1))
                    fs_ls();
                break;
            case ('E'):                                 // Change size of file <file name> <new size>
                if (validate_args(file_name, line_count, args, arg_count, 2, 5, 127)) {
                    fix_string(fixed_string, args[1]);
                    fs_resize(fixed_string, atoi(args[2]));
                }
                break;
            case ('O'):                                 // De-fragment the disk
                if (validate_args(file_name, line_count, args, arg_count, 0, -1, -1))
                    fs_defrag();
                break;
            case ('Y'):                                 // Change current working directory
                if (validate_args(file_name, line_count, args, arg_count, 1, 5, -1)) {
                    fix_string(fixed_string, args[1]);
                    fs_cd(fixed_string);
                }
                break;
            default:
                fprintf(stderr, "Command Error: %s, %d\n", file_name, line_count);
                break;
        }
        line_count++;
    }

    fclose(fp);
    free(args);
    free(disk_name);
    free(super_block);
    if (file_system != -1) close(file_system);
    return 0;
}
