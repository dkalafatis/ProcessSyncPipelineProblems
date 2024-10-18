#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int fd[2];          // File descriptors for the pipe
    pid_t pid;          // Process ID after fork
    char buffer[BUFFER_SIZE];

    // Check if the message is provided as a command-line argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s message\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a pipe
    if (pipe(fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Fork the current process
    pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        close(fd[0]);  // Close the unused read end of the pipe

        // Write the message to the pipe
        if (write(fd[1], argv[1], strlen(argv[1]) + 1) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }

        close(fd[1]);  // Close the write end of the pipe after writing
    } else {
        // Child process
        close(fd[1]);  // Close the unused write end of the pipe

        // Read the message from the pipe
        if (read(fd[0], buffer, BUFFER_SIZE) == -1) {
            perror("Read from pipe failed");
            exit(EXIT_FAILURE);
        }

        printf("Child received message: %s\n", buffer);
        close(fd[0]);  // Close the read end of the pipe after reading
    }

    return 0;
}
