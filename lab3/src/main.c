#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHILD "./child"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1; 
    }

    const char *filename = argv[1];

    // char testfile_path[256];
    // snprintf(testfile_path, sizeof(testfile_path), "tests/%s", filename);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("File open failed");
        return 1; 
    }

    struct stat file_info;
    if (fstat(fd, &file_info) == -1) {
        perror("File stat failed");
        close(fd);
        return 1;
    }

    int *mapped_data = (int *)mmap(NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped_data == MAP_FAILED) {
        perror("Memory mapping failed");
        close(fd);
        return 1;
    }

    close(fd);

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        munmap(mapped_data, file_info.st_size);
        return 1; 
    }

    if (pid == 0) {
        execl(CHILD, CHILD, filename, (char *)NULL);
        perror("Execl failed");
        _exit(1); 
    } else {
        int status;
        waitpid(pid, &status, 0);

        // Проверяем, успешно ли завершился дочерний процесс
        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process terminated by signal: %d\n", WTERMSIG(status));
        }

        munmap(mapped_data, file_info.st_size);
    }

    return 0; 
}
