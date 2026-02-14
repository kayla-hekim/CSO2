#ifndef FORK_RUN_H
#define FORK_RUN_H

void writeoutput(const char *command, char *out_path, char *err_path);

void parallelwriteoutput(int count, const char **argv_base, const char *out_file);

#endif
