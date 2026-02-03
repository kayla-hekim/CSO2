// #include "my_system.h" // uncomment idk don't rly need
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int my_system(const char *command) {
    // check if command given is null - dk if need bc assignment says don't check if shell is available but whatever
    if (command == NULL) {
        return 1;
    }

    // fork
    pid_t processID = fork();

    if (processID < 0) { // error in fork, > 0 is parent or other
        return -1;
    }
    else if (processID == 0) { // process is the child - execute exec command
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127); // in system() - exits and returns as error to parent process if exec fails in child (basically, can't execute system call -> go back)
    }
    
    // wait for child to process
    int childStatus;
    int outcome = waitpid(processID, &childStatus, 0);
    
    // then return status of child waiting unless it's an error forking or other which is -1
    if (outcome < 0) {
        return -1; // waitpid had an error while waiting for child process to do what it needs to do -> child failed its doing
    }

    return childStatus;
}
