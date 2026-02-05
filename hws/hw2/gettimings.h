#ifndef GETTIMINGS_H
#define GETTIMINGS_H
#include <unistd.h>

long long nsecs(void);

void emptyFunction(void); // 1
void randNumGenerator(void); // 2
void doGetppid(void); // 3
pid_t retInParent(void); // 4
int terminateBeforeWaitpid(pid_t processID); // 5
pid_t forkThenTerminateBeforeWaitpid(void); // 6
int runBinTrue(void); // 7
void createRemoveDir(void); // 8

int timeEightScenarios(int option);


#endif
