#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>


void writeoutput(const char *command, char *out_path, char *err_path) {
    pid_t processID = fork();

    if (processID == 0) {
        // open out_path fd
        int out_fd = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        // error path open
        int err_fd = open(err_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (err_fd < 0) {
            perror("Failed to open error file");
            close(out_fd);
            _exit(127);
        }

        // redirect STDOUT_FILENO to out_fd
        int stdout_redirect_status = dup2(out_fd, STDOUT_FILENO);
        if (stdout_redirect_status < 0) {
            perror("Failed to redirect STDOUT_FILENO");
            close(out_fd);
            close(err_fd);
            _exit(127);
        }

        // redirect STDERR_FILENO to err_fd
        if (dup2(err_fd, STDERR_FILENO) < 0) {
            perror("dup2 stderr failed");
            close(out_fd);
            close(err_fd);
            _exit(127);
        }

        // duplicated fd so close out
        close(out_fd);
        close(err_fd);


        // now: doing exec
        char *argv[] = { "/bin/sh", "-c", (char *)command, NULL };
        execvp(argv[0], argv); // will exit this if statement if worked, else continue with next line
        perror("execvp failed");
        exit(1);
    }

    waitpid(processID, NULL, 0);
}


void parallelwriteoutput(int count, const char **argv_base, const char *out_file) {
    pid_t pids[count];

    for (int i = 0; i < count; i++) {
        // FORK PARENT PROCESS ON EACH ITERATION UDNER COUNT
        pid_t curr_pid = fork();
        // printf("here: %d", i);
        
        if (curr_pid == 0) { // child process for current loop iteration at i
            // APPEND TO ARGV_BASE
            // char *argv[count + 2]; 
            // for (int j = 0; argv_base[j] != NULL; j++) {
            //     argv[j] = (char *)argv_base[j];
            // }

            // argv[count] = (char *)malloc(10 * sizeof(char));
            // snprintf(argv[count], 10, "%d", i);
            // argv[count + 1] = NULL;
            int argc = 0;
            while (argv_base[argc] != NULL) {
                argc++;
            }
            char *argv[argc + 2];
            for (int j = 0; j < argc; j++) {
                argv[j] = (char *)argv_base[j];
            }

            // in index_str put that index number at end for exec
            char index_str[12];
            snprintf(index_str, sizeof(index_str), "%d", i);
            argv[argc] = index_str;
            argv[argc + 1] = NULL;

            // FILE OPEN OUT_FILE
            int out_file_fd = open(out_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (out_file_fd < 0) {
                perror("Failed to open output file");
                _exit(1);
            }

            // DUP2 OUT_FILE
            int stdout_redirect_status = dup2(out_file_fd, STDOUT_FILENO);
            if (stdout_redirect_status < 0) {
                perror("Failed to redirect STDOUT_FILENO to out_file");
                close(out_file_fd);
                _exit(1);
            }

            close(out_file_fd);
            // EXECUTE AND REPLACE CHILD PROCESS
            execv(argv_base[0], argv); 

            // will only return to fail from execv
            perror("exec failed and returned to parallelwriteoutput");
            _exit(1);
        }
        else if (curr_pid > 0) { // parent process - add to pids
            pids[i] = curr_pid;
        }
        else {
            perror("fork failed"); // fork issue
            exit(1);
        }
    }

    for (int i = 0; i < count; i++) {
        waitpid(pids[i], NULL, 0);
    }
}
