/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2018-2022 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 * macOS adaptation with enhanced library support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <limits.h>

// Function to parse compiler flags from environment variable
char **parse_compiler_flags(const char *env_var, int *count) {
    char *flags_str = getenv(env_var);
    if (!flags_str || !flags_str[0]) {
        *count = 0;
        return NULL;
    }
    
    // Count spaces to estimate number of arguments
    int num_args = 1;
    for (const char *p = flags_str; *p; p++) {
        if (*p == ' ' && *(p+1) && *(p+1) != ' ') {
            num_args++;
        }
    }
    
    // Allocate array for arguments
    char **args = malloc((num_args + 1) * sizeof(char *));
    if (!args) {
        *count = 0;
        return NULL;
    }
    
    // Create a copy of the flags string that we can modify
    char *flags_copy = strdup(flags_str);
    if (!flags_copy) {
        free(args);
        *count = 0;
        return NULL;
    }
    
    // Parse into individual arguments
    int i = 0;
    char *token = strtok(flags_copy, " ");
    while (token && i < num_args) {
        args[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    *count = i;
    
    free(flags_copy);
    return args;
}

// Function to build and execute the compiler command
void exec_compiler(const char *cc, const char *output_path) {
    int compiler_flags_count = 0;
    char **compiler_flags = parse_compiler_flags("CSCRIPT_FLAGS", &compiler_flags_count);
    
    // Base arguments (always included)
    const char *base_args[] = {
        cc, "-pipe", "-xc", "-o", output_path, "-O2"
    };
    int base_count = sizeof(base_args) / sizeof(base_args[0]);
    
    // Calculate total arguments
    int total_args = base_count + compiler_flags_count + 2; // +1 for "-", +1 for NULL
    
    // Allocate argument array
    char **args = calloc(total_args, sizeof(char*));
    if (!args) {
        perror("Error: cannot allocate memory for arguments");
        if (compiler_flags) {
            for (int i = 0; i < compiler_flags_count; i++) {
                free(compiler_flags[i]);
            }
            free(compiler_flags);
        }
        _exit(1);
    }
    
    // Fill in the arguments
    int arg_index = 0;
    
    // Add base arguments
    for (int i = 0; i < base_count; i++) {
        args[arg_index++] = (char*)base_args[i];
    }
    
    // Add compiler flags
    for (int i = 0; i < compiler_flags_count; i++) {
        args[arg_index++] = compiler_flags[i];
    }
    
    // Add standard input indicator and NULL terminator
    args[arg_index++] = "-";
    args[arg_index] = NULL;
    
    // Execute the compiler
    execvp(cc, args);
    
    // Clean up if execvp fails
    if (compiler_flags) {
        for (int i = 0; i < compiler_flags_count; i++) {
            free(compiler_flags[i]);
        }
        free(compiler_flags);
    }
    free(args);
}

int main(int argc, char *argv[], char *envp[])
{
    int input = -1, pipes[2] = { 0 }, status;
    pid_t compiler_pid;
    char *output_path;
    char temp_path[PATH_MAX];
    
    if (argc > 1 && argv[argc - 1][0] != '-') {
        input = open(argv[1], O_RDONLY);
        ++argv;
        --argc;
        if (input < 0) {
            perror("Error: unable to open input file");
            return 1;
        }
        if (pipe(pipes) < 0) {
            perror("Error: unable to open filter pipe");
            return 1;
        }
    }

    // Create a temporary file instead of memfd
    snprintf(temp_path, PATH_MAX, "/tmp/cscript_tmp_XXXXXX");
    int fd = mkstemp(temp_path);
    if (fd < 0) {
        perror("Error: unable to create temporary file");
        return 1;
    }
    
    // We'll set the executable bit after compilation
    output_path = strdup(temp_path);
    if (!output_path) {
        perror("Error: unable to allocate memory for path string");
        return 1;
    }

    // Unlink the file so it gets cleaned up on program exit
    if (unlink(temp_path) < 0) {
        perror("Warning: unable to unlink temporary file");
        // Non-fatal, continue
    }

    compiler_pid = fork();
    if (compiler_pid < 0) {
        perror("Error: unable to fork for compiler");
        return 1;
    }

    if (compiler_pid == 0) {
        // Use standard C ternary operator for better compatibility
        const char *cc = getenv("CC");
        if (!cc) cc = "clang";  // Default to clang on macOS
        
        close(input);
        if (pipes[0] != 0) {
            close(pipes[1]);
            if (dup2(pipes[0], 0) < 0) {
                perror("Error: unable to duplicate pipe fd");
                _exit(1);
            }
            close(pipes[0]);
        }
        
        // Use the new function instead of execlp
        exec_compiler(cc, output_path);
        
        // If we reach here, exec failed
        perror("Error: failed to execute compiler");
        _exit(1);
    }

    if (input != -1) {
        char beginning[2];
        ssize_t len;
        char buffer[4096];

        close(pipes[0]);
        len = read(input, beginning, 2);
        if (len < 0) {
            perror("Error: unable to read from input file");
            return 1;
        } else if (len == 2 && beginning[0] == '#' && beginning[1] == '!') {
            len = write(pipes[1], "//", 2);
            if (len < 0) {
                perror("Error: unable to write input preamble");
                return 1;
            }
        } else if (len > 0) {
            len = write(pipes[1], beginning, len);
            if (len < 0) {
                perror("Error: unable to write input preamble");
                return 1;
            }
        }
        
        // Read/write loop for data transfer
        while ((len = read(input, buffer, sizeof(buffer))) > 0) {
            if (write(pipes[1], buffer, len) != len) {
                perror("Error: unable to write to compiler pipe");
                return 1;
            }
        }
        
        if (len < 0) {
            perror("Error: unable to read from input file");
            return 1;
        }
        
        close(pipes[1]);
    }

    if (waitpid(compiler_pid, &status, 0) != compiler_pid || (!WIFEXITED(status) || WEXITSTATUS(status))) {
        fprintf(stderr, "Error: compiler process did not complete successfully\n");
        return 1;
    }

    // Make the output file executable
    if (fchmod(fd, 0755) < 0) {
        perror("Error: unable to make the output file executable");
        return 1;
    }

    // Execute the compiled program
    if (execve(output_path, argv, envp) < 0) {
        perror("Error: could not execute compiled program");
        return 1;
    }
    
    return 0;
}
