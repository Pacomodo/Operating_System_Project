#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to read memory usage from /proc/[PID]/status
void get_memory_usage(int pid) {
    char path[40], line[256];
    FILE *status_file;
    long physical_memory = 0, virtual_memory = 0, shared_memory = 0;

    // Construct the path to /proc/[PID]/status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    // Open the /proc/[PID]/status file
    status_file = fopen(path, "r");
    if (!status_file) {
        perror("Error opening /proc/[PID]/status");
        exit(EXIT_FAILURE);
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "VmRSS:", 6) == 0) { // Physical Memory
            sscanf(line, "VmRSS: %ld kB", &physical_memory);
        } else if (strncmp(line, "VmSize:", 7) == 0) { // Virtual Memory
            sscanf(line, "VmSize: %ld kB", &virtual_memory);
        } else if (strncmp(line, "RssShmem:", 9) == 0) { // Shared Memory
            sscanf(line, "RssShmem: %ld kB", &shared_memory);
        }
    }

    fclose(status_file);

    // Print the memory usage
    printf("Physical Memory current: %ld kB\n", physical_memory);
    printf("Virtual Memory current: %ld kB\n", virtual_memory);
    printf("Shared Memory current: %ld kB\n", shared_memory);
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // take argument only 2. first is ku_mem, second is PID
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pid = atoi(argv[1]);

    // Run the memory check every 0.5 seconds
    for (int i = 0; i < 120; ++i) { // 1 minute
        printf("Memory check for PID: %d\n", pid);
        get_memory_usage(pid);
        printf("------------------------------------\n");
        usleep(500000); // Sleep for 0.5 seconds
    }

    return EXIT_SUCCESS;
}

