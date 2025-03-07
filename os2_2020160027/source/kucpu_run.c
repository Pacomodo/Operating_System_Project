#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define KU_CPU 339 // Define the syscall number

int main(int argc, char **argv) {
    int jobTime;
    int delayTime;
    char name[4];
    int wait = 0;
    int responseTime = -1; // Initialize response time to -1 to capture the first successful access

    if (argc < 4) {
        printf("\nInsufficient Arguments..\n");
        return 1;
    }

    // first argument: job time (second)
    // second argument: delay time before execution (second)
    // third argument: process name
    jobTime = atoi(argv[1]);
    delayTime = atoi(argv[2]);
    strcpy(name, argv[3]);

    // wait for 'delayTime' seconds before execution
    sleep(delayTime);
    printf("\nProcess %s: I will use CPU for %ds.\n", name, jobTime);
    jobTime *= 10; // execute system call in every 0.1 second

    // Continue requesting the system call as long as jobTime remains
    while (jobTime > 0) {
        int result = syscall(KU_CPU, name, jobTime);
        if (result == 0) {
            // The request was accepted
            jobTime--; // decrease job time on each successful access
            // If this is the first successful request, set response time
            if (responseTime == -1) {
                responseTime = wait; // Response time is the wait count at the first acceptance
            }
        } else {
            // The request was rejected, increase wait time
            wait++;
        }
        usleep(100000); // delay 0.1 second
    }

    // Signal the completion of the job to ku_cpu
    syscall(KU_CPU, name, 0);

    printf("\nProcess %s: Finish! My response time is %.1fs and My total wait time is %.1fs.\n",
           name, responseTime * 0.1, wait * 0.1);

    return 0;
}
