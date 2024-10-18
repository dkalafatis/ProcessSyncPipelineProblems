#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    char *command = "your_command";  // Replace with the actual command
    char *arg = "your_argument";     // Replace with the actual argument
    char *inputfile = "input.txt";   // Replace with the actual input file

    // Open the input file
    int fd_in = open(inputfile, O_RDONLY);
    if (fd_in < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Create a new process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(fd_in);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        // Redirect standard input to the input file
        if (dup2(fd_in, STDIN_FILENO) < 0) {
            perror("dup2");
            close(fd_in);
            exit(EXIT_FAILURE);
        }
        close(fd_in);  // Close the file descriptor after duplication

        // Execute the command with the argument
        execlp(command, command, arg, (char *)NULL);
        perror("execlp");  // If execlp returns, an error occurred
        exit(EXIT_FAILURE);
    } else {  // Parent process
        close(fd_in);  // Close the file descriptor in the parent
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        // Optionally, check the exit status here
    }

    return 0;
}
