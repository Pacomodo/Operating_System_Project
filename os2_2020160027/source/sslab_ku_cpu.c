#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/string.h>

#define IDLE -1

typedef struct {
    pid_t pid;             // 프로세스 ID
    int jobTime;           // 작업 시간
    struct list_head list; // 대기 큐를 위한 링크드 리스트 구조체
} ku_cpu_process_t;

static pid_t now = IDLE;   // 현재 `ku_cpu`를 사용 중인 프로세스 ID (IDLE 상태로 초기화)
static LIST_HEAD(waiting_queue); // 대기 큐

// 대기 큐에 프로세스를 추가하는 함수
void ku_push(ku_cpu_process_t *process) {
    list_add_tail(&process->list, &waiting_queue);
    printk(KERN_INFO "Process %d added to waiting queue\n", process->pid);
}

// 대기 큐에서 다음 프로세스를 가져오는 함수
ku_cpu_process_t* ku_pop(void) {
    if (list_empty(&waiting_queue))
        return NULL;

    ku_cpu_process_t *next_process = list_first_entry(&waiting_queue, ku_cpu_process_t, list);
    list_del(&next_process->list);
    printk(KERN_INFO "Process %d removed from waiting queue\n", next_process->pid);
    return next_process;
}
// SJF: 대기 큐에서 jobTime이 가장 짧은 프로세스를 가져오는 함수
ku_cpu_process_t* ku_sjf_pop(void) {
    if (list_empty(&waiting_queue))
        return NULL;

    ku_cpu_process_t *shortest_job = NULL;
    ku_cpu_process_t *process;
    int min_job_time = INT_MAX;

    list_for_each_entry(process, &waiting_queue, list) {
        if (process->jobTime < min_job_time) {
            min_job_time = process->jobTime;
            shortest_job = process;
        }
    }

    if (shortest_job) {
        list_del(&shortest_job->list);
        printk(KERN_INFO "Process %d with shortest job time %d removed from waiting queue\n", 
                shortest_job->pid, shortest_job->jobTime);
    }
    return shortest_job;
}

// 대기 큐에 해당 프로세스가 있는지 확인하는 함수
bool is_in_queue(pid_t pid) {
    ku_cpu_process_t *process;
    list_for_each_entry(process, &waiting_queue, list) {
        if (process->pid == pid) {
            return true;
        }
    }
    return false;
}

// 시스템 호출 핸들러
SYSCALL_DEFINE2(os2024_ku_cpu, char*, name, int, jobTime) {
    ku_cpu_process_t *newJob;

    // 현재 작업을 마친 프로세스에 대한 처리
    if (jobTime == 0 && now == current->pid) {
        printk("Process Finished: %s (PID: %d)\n", name, current->pid);
        
        // 현재 프로세스가 종료되었으므로 now를 초기화
        now = IDLE;

        // 대기 큐에서 다음 프로세스를 가져옴
        if (!list_empty(&waiting_queue)) {
            // ku_cpu_process_t *next_process = ku_pop(); //FCFS
    	    ku_cpu_process_t *next_process = ku_sjf_pop();
            if (next_process) {
                now = next_process->pid;
                // printk("Process %s (PID: %d) started from queue\n", name, next_process->pid);
		printk("Process %s (PID: %d) with shortest job time %d started from queue\n", name, next_process->pid, next_process->jobTime); //SJF
                kfree(next_process); // 다음 작업이 시작되었으므로 메모리 해제
            }
        } else {
            printk("Queue is empty after finishing process %s\n", name);
        }
        return 0;
    }

    // 현재 작업 중인 프로세스가 있는 경우 요청을 거부
    if (now != IDLE && now != current->pid) {
        if (!is_in_queue(current->pid)) {
            // 대기 큐에 프로세스를 추가
            newJob = kmalloc(sizeof(ku_cpu_process_t), GFP_KERNEL);
            if (!newJob) {
                printk(KERN_ERR "ku_cpu: Memory allocation failed\n");
                return -ENOMEM;
            }
            newJob->pid = current->pid;
            newJob->jobTime = jobTime;
            INIT_LIST_HEAD(&newJob->list);
            ku_push(newJob);
        }
        printk("Working Denied: %s (PID: %d)\n", name, current->pid);
        return 1; // 요청 거부
    }

    // 현재 CPU가 유휴 상태이거나 요청한 프로세스가 현재 `ku_cpu`를 사용 중인 경우
    if (now == IDLE) {
        newJob = kmalloc(sizeof(ku_cpu_process_t), GFP_KERNEL);
        if (!newJob) {
            printk(KERN_ERR "ku_cpu: Memory allocation failed\n");
            return -ENOMEM;
        }
        newJob->pid = current->pid;
        newJob->jobTime = jobTime;
        INIT_LIST_HEAD(&newJob->list);
        now = newJob->pid;
        printk("Process %s (PID: %d) started immediately\n", name, newJob->pid);
        kfree(newJob); // 바로 실행하는 프로세스는 메모리 해제
        return 0;
    }

    // 현재 프로세스가 요청하는 작업을 대기열에 추가
    if (now == current->pid && jobTime > 0) {
        printk("Working: %s (PID: %d)\n", name, current->pid);
        return 0;
    }

    // 작업이 요청되었으나 실행 중이므로 대기 큐에 추가
    newJob = kmalloc(sizeof(ku_cpu_process_t), GFP_KERNEL);
    if (!newJob) {
        printk(KERN_ERR "ku_cpu: Memory allocation failed\n");
        return -ENOMEM;
    }
    newJob->pid = current->pid;
    newJob->jobTime = jobTime;
    INIT_LIST_HEAD(&newJob->list);
    ku_push(newJob);
    printk("Process %s (PID: %d) added to queue\n", name, newJob->pid);
    return 1; // 대기 중임을 나타냄
}

