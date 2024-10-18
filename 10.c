#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd[2];
    pid_t pid;
    char c;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s input_file output_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create the pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { // Parent process
        int input_fd;
        // Close the read end of the pipe
        close(fd[0]);

        // Open the input file
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd == -1) {
            perror("open input file");
            close(fd[1]);
            exit(EXIT_FAILURE);
        }

        // Read from the input file character by character and write to the pipe
        while (read(input_fd, &c, 1) > 0) {
            if (write(fd[1], &c, 1) == -1) {
                perror("write to pipe");
                close(input_fd);
                close(fd[1]);
                exit(EXIT_FAILURE);
            }
        }

        close(input_fd);
        close(fd[1]); // Close write end of pipe
        wait(NULL); // Wait for child process to finish
        exit(EXIT_SUCCESS);
    } else { // Child process
        int output_fd;
        ssize_t nread;

        // Close the write end of the pipe
        close(fd[1]);

        // Open the output file (create if necessary)
        output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open output file");
            close(fd[0]);
            exit(EXIT_FAILURE);
        }

        // Read from the pipe character by character and write to the output file
        while ((nread = read(fd[0], &c, 1)) > 0) {
            if (write(output_fd, &c, 1) == -1) {
                perror("write to output file");
                close(output_fd);
                close(fd[0]);
                exit(EXIT_FAILURE);
            }
        }

        if (nread == -1) {
            perror("read from pipe");
        }

        close(output_fd);
        close(fd[0]); // Close read end of pipe
        exit(EXIT_SUCCESS);
    }

    return 0;
}
