#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h> 

#define my_stack_push 335
#define my_stack_pop  336

int main() {
    int res;
    int random_value;
    int i;
    srand(time(NULL));

    for (i = 0; i < 10; i++) {
        random_value = rand();
        res = syscall(my_stack_push, random_value);
        if (res == 0) {
            printf("Push: %d\n", random_value);
        } else {
            printf("Error %s\n", strerror(errno));
        }
    }

    for (i = 0; i < 10; i++) {
        res = syscall(my_stack_pop);
        if (res >= 0) {
            printf("Pop: %d\n", res);
        } else {
            printf("Error %s\n", strerror(errno));
        }
    }

    return 0;
}
