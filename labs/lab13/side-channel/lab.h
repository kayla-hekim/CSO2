#ifndef LAB_H_
#define LAB_H_

extern long measure_once(int *presult, char *p, int (*f)(char*));
extern int check_passphrase(char *p);
extern void find_passphrase(char*, int);

#endif
