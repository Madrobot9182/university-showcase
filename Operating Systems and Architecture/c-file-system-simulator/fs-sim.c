#include "fs-sim.h"

int file_system = -1;                   // File descriptor of the mounted file
char* disk_name;                        // Name of the current disk
char buffer[1024] = {0};                // Buffer for reading/writing data
const char zeros[1024] = {0};           // Used to clear out data blocks
Superblock* super_block;                // Local copy of the super block

uint8_t working_directory = 0x7F;       // The current working directory within the file system

void print_bin(uint8_t num) {
    for (uint8_t i = 1 << 7; i > 0; i = i >> 1) {
        if ((num & i) != 0) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

void print_bin_64(uint64_t num) {
    for (uint64_t i = 1 << 31; i > 0; i = i >> 1) {
        if ((num & i) != 0) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

void fs_mount(char *new_disk_name) {
    int error_code = 0;
    // First check if the virtual disk exist
    if (access(new_disk_name, F_OK) != 0) {
        fprintf(stderr, "Error: Cannot find disk %s\n", new_disk_name);
        return;
    }

    // Open the file stream and load the super block (first KB)
    int fd = open(new_disk_name, O_RDWR);
    read(fd, super_block, 1024);

    // Cross tables to check for duplicates within directories
    int free_block_table[128] = {0};

    // Perform the check for each inode
    for (uint8_t index = 0; index < 126; index++) {
        // Initialize inode
        Inode inode = super_block->inode[index];

        uint8_t used_size = inode.used_size;
        uint8_t start_block = inode.start_block;
        uint8_t dir_parent = inode.dir_parent;

        // Check if the state of the inode is free than all bits are free, otherwise there should be a inode name
        if ((used_size >> 7) == 0) {
            if (strcmp(inode.name, "") != 0 || used_size + start_block + dir_parent != 0) {
                error_code = 1;
                break;
            } else continue;    // Skip the rest of the checks
        } else {    // The inode is used and should have a name
            if (strcmp(inode.name, "") == 0) {
                error_code = 1;
                break;
            }
        }

        if ((dir_parent & 128) == 0) {  // first bit is 0 - file
            // Check the start block has valid index [1, 127]
            if (start_block < 1 || start_block > 127) {
                error_code = 2;
                break;
            }

            // Check if the size is such that the last block is between [1, 127]
            // start block + size (num of occupied blocks) = end block -- in boundary?
            uint32_t end_block = start_block + (used_size & 127);
            if (end_block < 1 || end_block > 127) { // Exclude first bit, check size
                error_code = 2;
                break;
            }

        } else {    // This is a directory
            // Check if size of file and start_block of a directory is zero
            if ((used_size & 127) != 0 || start_block != 0) {
                error_code = 3;
                break;
            }
        }

        // Check if parent index is valid 127 = 0x7F
        if ((dir_parent & 127) == 126 || (dir_parent & 127) < 0 || (dir_parent & 127) > 127) {
            error_code = 4;
            break;
        } else {
        // Check if the parent inode is in use and marked a directory
            if ((dir_parent & 127) != 127) {      // Skip the root directory
                // Check the parent inode
                Inode parent_inode = super_block->inode[dir_parent & 127];

                uint8_t is_used = parent_inode.used_size >> 7;
                uint8_t is_dir = parent_inode.dir_parent >> 7;

                if (!is_used || !is_dir) {
                    error_code = 4;
                    break;
                }
            }

        }
        // Track which blocks has files allocated
        for (uint8_t i = 0; i < (used_size & 127); i++)
            free_block_table[start_block + i] += 1; // Track which blocks have how many files allocated

        // Check for duplicate file names within directories

        for (uint8_t j = 0; j < 126; j++) {
            Inode temp = super_block->inode[j];
            if ((temp.used_size & 0x80) == 0) continue;
            if ((inode.dir_parent & 0x7F) == (temp.used_size & 0x7F) && strncmp(inode.name, temp.name, 5) == 0) {
                error_code = 5;
                break;
            }
        }
    }

    if (!error_code) {
        // Check if fsl matches status of inodes TODO may need a revision
        free_block_table[0] = +1;    // The superblock is 1 by default
        for (uint8_t i = 0; i < 16; i++) {
            uint8_t at_byte = super_block->free_block_list[i];
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t index_at_byte = 1 << (7 - bit);
                uint8_t is_occupied = at_byte & index_at_byte;
                if (is_occupied) {
                    if (free_block_table[8*i + bit] != 1) {     // Can only be allocated one file
                        error_code = 6;
                        break;
                    }
                } else if (free_block_table[8*i + bit] != 0) {  // Else no files were allocated
                    error_code = 6;
                    break;
                }
            }
            if (error_code) break;  // End early on error code
        }
    }

    if (error_code) {
        fprintf(stderr, "Error: File system in %s is inconsistent (error code: %d)\n", new_disk_name, error_code);
        close(fd);
    } else {
        // Mount the file system, set the working directory
        if (file_system != -1) close(file_system);
        file_system = fd;

        // Allocate a new string for the disk name
        if (disk_name != NULL) {
            free(disk_name);
            disk_name = NULL;
        }
        disk_name = malloc(strlen(new_disk_name) + 1);  // Malloc new string copy
        if (disk_name != NULL) {
            strcpy(disk_name, new_disk_name);
        }

        working_directory = 0x7F;
    }
}

void fs_create(char name[5], int size) {
    uint8_t is_dir = (size) ? 0 : 1;
    // Search for the first open inode
    uint8_t free_space = 0;
    uint8_t free_fsl_index = 0;
    int file_success = 0, block_success = 0;    // Flags to check if we find free blocks and space for an inode

    for (uint8_t j = 0; j < 126; j++) {     // First Check if the filename already exist
        Inode temp = super_block->inode[j];
        if ((temp.used_size & 0x80) == 0) continue;
        if (temp.dir_parent == working_directory && strncmp(name, temp.name, 5) == 0) {
                fprintf(stderr, "Error: File or directory %s already exists\n", name);
                return;
        }
    }

    // Scan the fsl, find enough concurrent open blocks
    for (uint8_t fsl_index = 1; fsl_index < 128 && !block_success; fsl_index++) {
        // Calculate the current bit in the free space list
        uint8_t at_byte = super_block->free_block_list[fsl_index / 8];
        uint8_t index_at_byte = 1 << (7 - fsl_index % 8);   // Which bit in the byte
        uint8_t is_occupied = at_byte & index_at_byte;

        if (!is_occupied) {
            // Continue Looping, count free spaces
            if(!free_fsl_index) free_fsl_index = fsl_index;     // Initialize the free fsl index, count from here
            free_space += 1;
            if (free_space >= size) {   // Valid space, add file to file system
                Inode new_inode = {"",
                ((1<<7) | (size & 0x7F)),
                ((is_dir) ? 0 : free_fsl_index),
                (is_dir << 7) | (working_directory & 0x7F)};
                memcpy(new_inode.name, name, sizeof(char[5]));

                // Find the next available inode and update the super block
                for (uint8_t index = 0; index < 126 && !file_success; index++) {
                    uint8_t is_occupied = super_block->inode[index].used_size;
                    if (!is_occupied) {     // Successfully found space to write
                        // Update the local and file super block
                        super_block->inode[index] = new_inode;

                        for (uint8_t i = 0; i < size; i++) {    // Set the fsl bits at set byte
                            uint8_t mask = 1 << (7 - ((free_fsl_index + i) % 8));
                            super_block->free_block_list[(free_fsl_index + i) / 8] |= mask;
                        }

                        file_success = 1;
                    }
                }
                if (!file_success){
                    fprintf(stderr, "Superblock in disk %s is full, cannot create %s\n", disk_name, name);
                    return;
                } else {
                    // Rewrite the super_block back into the file system
                    lseek(file_system, 0, SEEK_SET);    // Set the offset back to the beginning
                    write(file_system, super_block, sizeof(Superblock));
                }

                block_success = 1;
            }
        } else {    // Not enough space, skip the space and continue searching
            free_fsl_index = 0;
            free_space = 0;
        }
    }

    if (!block_success) {
        fprintf(stderr, "Error: Cannot allocate %d blocks on %s\n", size, disk_name);
        return;
    }
}

void fs_delete(char name[5]) {
    int success = 0;
    static uint8_t original_working_directory = 0xFF;  // Static to track top-level call

    // First time call, set original working directory
    if (original_working_directory == 0xFF) {
        original_working_directory = working_directory;
    }

    // First pass: find and recurse into all sub-directories
    // First pass: find and recurse into all sub-directories
    for (uint8_t i = 0; i < 126; i++) {
        Inode temp = super_block->inode[i];
        if (!(temp.used_size & 0x80)) continue;  // Skip unused inodes

        // Check if this is a directory in the current working directory
        if ((temp.dir_parent & 0x80) &&                                 // Is a directory
            (temp.dir_parent & 0x7F) == working_directory &&            // In current working directory
            (original_working_directory == working_directory ?          // If at top-level delete
             strncmp(name, temp.name, 5) == 0 : 1)) {                   // Only match named dir at top level

            // Recursively delete contents of this directory
            uint8_t prev_working_directory = working_directory;
            working_directory = i;
            fs_delete(name);
            working_directory = prev_working_directory;
        }
    }

    // Second pass, delete all file and folders within the current directory != delete flag
    // If at delete flag, just delete the file/folder with name[5]
    for (uint8_t i = 0; i < 126; i++) {
        Inode temp = super_block->inode[i];
        if (!(temp.used_size & 0x80)) continue;  // Skip unused inodes

        if ((temp.dir_parent & 0x7F) == working_directory &&                                 // Object in the current directory
            (original_working_directory == working_directory ?
            strncmp(name, temp.name, 5) == 0 : 1)) {          // If in the original directory, Only delete the original named object

            // Delete data blocks
            uint8_t data_block = temp.start_block;
            uint8_t size = temp.used_size & 0x7F;

            // Zero out data block
            for (uint8_t j = 0; j < size; j++) {
                lseek(file_system, (data_block + j) * 1024, SEEK_SET);
                write(file_system, zeros, 1024);
            }

            // Update FSL
            for (uint8_t j = 0; j < size; j++) {
                uint8_t mask = 1 << (7 - ((data_block + j) % 8));
                super_block->free_block_list[(data_block + j) / 8] &= ~mask;
            }

            // Clear inode
            memset(&super_block->inode[i], 0, sizeof(Inode));
            success = 1;
        }
    }

    // Update superblock
    lseek(file_system, 0, SEEK_SET);
    write(file_system, super_block, sizeof(Superblock));

    // On the final return (top-level call)
    if (original_working_directory == working_directory) {
        // Reset the static variable
        original_working_directory = 0xFF;

        // Error if no success at top-level delete
        if (!success) {
            fprintf(stderr, "Error: File or directory %s does not exist\n", name);
        }
    }
}


void fs_read(char name[5], int block_num) {
     // Find the inode in the working directory
    int success = 0;
    char cmp_name[5];
    for (uint8_t i = 0; i < 126 && !success; i++) {
        Inode temp = super_block->inode[i];
        memcpy(cmp_name, temp.name, sizeof(char[5]));   // Get a null terminated string for proper comparison

        if ((temp.dir_parent & 0x7F) == working_directory && strcmp(cmp_name, name) == 0) {
            // Read the n-th block and store it in the buffer
            uint8_t data_block = temp.start_block;
            uint8_t size = temp.used_size & 0x7F;

            if (block_num > size - 1) {
                fprintf(stderr, "Error: %s does not have block %d\n", name, block_num);
            } else {
                // Read the block into the buffer
                lseek(file_system, (data_block + block_num) * 1024, SEEK_SET);
                read(file_system, buffer, 1024);
            }

            success = 1;
            break;
        }
    }

    if (!success)
        fprintf(stderr, "Error: File %s does not exist\n", name);
}

// Identical copy of read but replaced with write
void fs_write(char name[5], int block_num) {
    // Find the inode in the working directory
    int success = 0;
    char cmp_name[5];
    for (uint8_t i = 0; i < 126 && !success; i++) {
        Inode temp = super_block->inode[i];
        memcpy(cmp_name, temp.name, sizeof(char[5]));   // Get a null terminated string for proper comparison

        if ((temp.dir_parent & 0x7F) == working_directory && strncmp(temp.name, name, 5) == 0) {
            // Read the n-th block and store it in the buffer
            uint8_t data_block = temp.start_block;
            uint8_t size = temp.used_size & 0x7F;

            if (block_num > size - 1) {
                fprintf(stderr, "Error: %s does not have block %d\n", name, block_num);
            } else {
                // Write the buffer into the data block
                lseek(file_system, (data_block + block_num) * 1024, SEEK_SET);
                write(file_system, buffer, 1024);
            }

            success = 1;
            break;
        }
    }

    if (!success)
        fprintf(stderr, "Error: File %s does not exist\n", name);
}

void fs_buff(char buff[1024]) {
    memset(buffer, 0, 1024);
    strcpy(buffer, buff);

    // Remove the newline character, if it exists
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';  // Replace the newline with the null terminator
    }
}

void fs_ls(void) {
    // Current files and folders point to current directory
    // Second search in parent folder, all files pointing to parent value + 2
    // Recursive third search sub directories, all files pointing to sun directory + 2

    // First find the parent and current count
    int current_count = 0; int parent_count = 0;    // Track the number of objects in current/parent directory
    char fixed_name[5];     // For proper printing

    // Get current and parent count
    uint8_t parent_index = (working_directory == 0x7F) ? working_directory : super_block->inode[working_directory].dir_parent & 0x7F;
    for (uint8_t i = 0; i < 126; i++) {
        Inode current= super_block->inode[i];

        if ((current.dir_parent & 0x7F) == working_directory)     // Object in current dir
            current_count += 1;
        if ((current.dir_parent & 0x7F) == parent_index)
            parent_count += 1;
    }
    printf(".     %3d\n", current_count + 2);
    printf("..    %3d\n", parent_count + 2);


    // Print out every sub item
    for (uint8_t i = 0; i < 126; i++) {
        Inode current= super_block->inode[i];

        if ((current.dir_parent & 0x7F) == working_directory && !(current.dir_parent & 0x80)) {     // a file in the current directory
            // Calculate the size and print the name
            uint8_t filesize = (current.used_size & 0x7F);
            memcpy(fixed_name, current.name, sizeof(char[5]));   // Get a null terminated string for proper comparison
            printf("%-5s %3d KB\n", fixed_name, filesize);

        } else if (current.dir_parent == (working_directory | 0x80)) {      // A folder in the current directory
            // Second search, find all things that are children of this folder
            int children_count = 0;
            uint8_t folder_index = i;     // Folder index that children point to
            for (uint8_t c = 0; c < 126; c++) {
                if ((super_block->inode[c].dir_parent & 0x7F) == folder_index) {
                    children_count += 1;
                }
            }
            memcpy(fixed_name, current.name, sizeof(char[5]));
            printf("%-5s %3d\n", fixed_name, children_count + 2);
        }
    }
}

void fs_resize(char name[5], int new_size) {
    int success = 0;
    char cmp_name[5];
    uint8_t free_index = 0;     // The datablock index to start expanding at
    for (uint8_t i = 0; i < 126 && !success; i++) {
        Inode temp = super_block->inode[i];
        memcpy(cmp_name, temp.name, sizeof(char[5]));   // Get a null terminated string for proper comparison

        if ((temp.dir_parent & 0x7F) == working_directory && !(temp.dir_parent & 0x80) && strcmp(cmp_name, name) == 0) {    // The file in the current directory
            success = 1;    // The file exist
            if (new_size < (temp.used_size & 0x7F)) {   // Shrink the file
                // Deleting data blocks
                uint8_t data_block = temp.start_block;
                uint8_t delete_size = (temp.used_size & 0x7F) - new_size;

                for (uint8_t j = 0; j < delete_size; j++) {
                    // Update the FSL
                    uint8_t mask = 1 << (7 - ((data_block + new_size + j) % 8));
                    super_block->free_block_list[(data_block + new_size + j) / 8] &= ~mask;

                    // Zero out data block
                    lseek(file_system, (data_block + new_size + j) * 1024, SEEK_SET);
                    write(file_system, zeros, 1024);
                }

                // Update the superblock
                super_block->inode[i].used_size = new_size | 0x80;

                // Update superblock on file
                lseek(file_system, 0, SEEK_SET);
                write(file_system, super_block, sizeof(Superblock));
                break;      // End the loop
            } else {        // Expand the file
                // First check if we have enough contiguous space in the current datablocks
                int has_space = 1;

                for (uint8_t k = (temp.used_size & 0x7F); k < new_size; k++) {   // Check the next blocks
                    uint8_t mask = 1 << (7 - (temp.start_block + (temp.used_size & 0x7F) + k) % 8);
                    if (((super_block->free_block_list[(temp.start_block + (temp.used_size & 0x7F) + k) / 8]) & mask)) {    // There is a something in the way
                        has_space = 0; break;
                    }
                }
                if (has_space) {free_index = temp.start_block;}     // Expand at the current start block
                else {   // Search the fsl for a big enough space
                    int counter = 0;
                    for (uint8_t k = 0; k < 128; k++) {
                        if ((super_block->free_block_list[k / 8] & (1 << (7 - (k % 8)))) == 0) {
                            if (free_index == 0) {free_index = k;}
                                counter += 1;

                            } else {free_index = 0; counter = 0;}     // Increment for every free space, reset if somethings already there

                        if (counter >= new_size) {  // Found a new space
                            has_space = 1;

                            // Move the old data to the new blocks
                            uint8_t start_block = temp.start_block;
                            uint8_t used_size = (temp.used_size & 0x7F);
                            for (uint8_t m = 0; m < used_size; m++) {                       // Shifting one block (1024) at a time
                                lseek(file_system, (start_block + m) * 1024, SEEK_SET);     // Read og block
                                read(file_system, super_block, 1024);                       // Using the super_block as a block buffer
                                write(file_system, zeros, 1024);                                // Clear the og block out
                                lseek(file_system, (free_index + m) * 1024, SEEK_SET);      // Write to new block
                                write(file_system, super_block, 1024);
                            }
                            // Retrieve the super block again
                            lseek(file_system, 0, SEEK_SET);
                            read(file_system, super_block, sizeof(Superblock));

                            // Delete the old fsl entries
                            for (uint8_t l = 0; l < used_size; l++) {
                                uint8_t mask = 1 << (7 - ((start_block + l) % 8));
                                super_block->free_block_list[(start_block + l) / 8] &= ~mask;
                            }
                            break;
                        }
                    }
                    if (counter < new_size) {
                        fprintf(stderr, "Error: File %s cannot expand to size %d\n", name, new_size);
                        has_space = 0;
                    }
                }
                // Perform the expansion if there is space
                if (has_space) {
                    // Set the new size and start of the superblock
                    super_block->inode[i].used_size = new_size | 0x80;
                    super_block->inode[i].start_block = free_index;

                    // Set the new fsl
                    uint8_t expand_size = new_size - (temp.used_size & 0x7F);
                    for (uint8_t l = 0; l < new_size; l++) {
                        uint8_t mask = 1 << (7 - ((free_index + l) % 8));
                        super_block->free_block_list[(free_index + l) / 8] &= ~mask;
                        super_block->free_block_list[(free_index + l) / 8] |= 1 << (7 - ((free_index + l) % 8));
                    }

                    // Update the file super block copy
                    lseek(file_system, 0, SEEK_SET);
                    write(file_system, super_block, sizeof(Superblock));
                }
            }
        }
    }

    if (!success)
        fprintf(stderr, "Error: File %s does not exist\n", name);
}

void fs_defrag(void) {
    // Double loop, Look at fsl to find first block with free space in front
    // Second loop to find inode, update first block and copy data block over
    uint8_t free_flag = 0;
    uint8_t file_size = 0;
    uint8_t start_block = 0;

    for (uint8_t i = 1; i < 128; i++) {     // i - fsl
        uint8_t mask = 1 << (7 - (i % 8));
        uint8_t is_used = super_block->free_block_list[i / 8] & mask;
        if (is_used) {  // This block is occupied
            if (!free_flag) continue;   // No free space, continue

            // Else we need to update the inode and data block
            for (uint8_t k = 0; k < 126; k++) {        // k - inodes

                start_block = super_block->inode[k].start_block;
                if (start_block == i) {
                    // Save the file_size and update the inode
                    file_size = super_block->inode[k].used_size & 0x7F;
                    super_block->inode[k].start_block = free_flag;

                    // Update the fsl, remove the old bit and set the new bit
                    for (uint8_t l = 0; l < file_size; l++) {       // l - fsl update
                        // Negate mask
                        uint8_t mask = 1 << (7 - ((i + l) % 8));
                        super_block->free_block_list[(i + l) / 8] &= ~mask;

                        // Set mask
                        mask = 1 << (7 - ((free_flag + l) % 8));
                        super_block->free_block_list[(free_flag + l) / 8] |= mask;
                    }

                    // Update the file super block copy
                    lseek(file_system, 0, SEEK_SET);
                    write(file_system, super_block, sizeof(Superblock));
                    break;      // Exit the search loop
                }
            }
            // Copy all the data blocks, increment i for position of old block
            for (uint8_t j = 0; j < file_size; j++) {                           // j - update datablock
                // Read the old data block
                lseek(file_system, (start_block + j) * 1024, SEEK_SET);  // Set to old block
                read(file_system, super_block, 1024);  // Read the actual data block into buffer

                // Write the data block to the new location first
                lseek(file_system, (free_flag + j) * 1024, SEEK_SET);  // Write to new block
                write(file_system, super_block, 1024);  // Write the buffered data to the new block

                // After the new block is written, clear the old block
                lseek(file_system, (start_block + j) * 1024, SEEK_SET);  // Set to old block
                write(file_system, zeros, 1024);  // Clear the original block
            }
            // Then retrieve the super block after
            lseek(file_system, 0, SEEK_SET);
            read(file_system, super_block, sizeof(Superblock));
            free_flag = 0;                  // Reset the search for fragments
            i = free_flag + file_size;      // Continue to search from end of moved block

        } else {        // This block is free
            if (!free_flag) free_flag = i;  // Set flag to first free block
        }
    }
}

void fs_cd(char name[5]) {
    int success = 0;
    // Check for the two dot commands
    if (strcmp(name, ".") == 0) {   // Navigate to itself?
        //working_directory = 0x7F;
        success = 1;
    } else if (strcmp(name, "..") == 0) {   // Navigate to parent
        if (working_directory != 0x7F) {   // Cant nav up from root
            working_directory = super_block->inode[working_directory].dir_parent & 0x7F;
        }
        success = 1;
    }

    // Else find the directory
    for (uint8_t i = 0; i < 126 && !success; i++) {
        Inode temp = super_block->inode[i];
        if ((temp.dir_parent) == (working_directory | 0x80) && strncmp(name, temp.name, 5) == 0) {
            working_directory = i;
            success = 1;
        }
    }

    if (!success)
        fprintf(stderr, "Error: Directory %s does not exist\n", name);
}
