#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1; 
    }

    const char *filename = argv[1];


    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("File open failed");
        return 1; 
    }

    
    struct stat file_stats;
    if (fstat(fileno(file), &file_stats) == -1) {
        perror("File stat failed");
        fclose(file);
        return 1; 
    }

    // Используем memory-mapped область, переданную в аргументах
    char *buffer = mmap(NULL, file_stats.st_size, PROT_READ, MAP_PRIVATE, fileno(file), 0);
    if (buffer == MAP_FAILED) {
        perror("Memory mapping failed");
        fclose(file);
        return 1; 
    }
    int sum = 0;
    int value;
    while (sscanf(buffer, "%d", &value) == 1) {
        printf("Read value: %d\n", value);
        sum += value;
        buffer = strchr(buffer, ' ');
        if (buffer == NULL) {
            break;
        }
        buffer++; 
    }


    printf("Final sum: %d\n", sum);

    
    fclose(file);
    munmap(buffer, file_stats.st_size);

    return 0; 
}
