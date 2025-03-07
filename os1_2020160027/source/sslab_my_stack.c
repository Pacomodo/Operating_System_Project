/* 2024 Fall COSE341 Operating System */
/* Project 1 */
/* Yeyoung Park */

#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/printk.h>
#include <linux/errno.h>

#define STACK_SIZE 100

static int stack[STACK_SIZE];
static int top = -1;

void print_stack(void) {
    int i;
    printk(KERN_INFO "Stack Top ---------------------\n");
    for (i = top; i >= 0; i--) {
        printk(KERN_INFO "%d\n", stack[i]);
    }
    printk(KERN_INFO "Stack Bottom ------------------\n");
}

SYSCALL_DEFINE1(os2024_push, int, a) {
    // overflow check
    if (top >= STACK_SIZE - 1) {
        printk(KERN_ALERT "Stack Overflow: Cannot push %d\n", a);
        return -ENOMEM; // return memory lackage error
    }
    top++;
    stack[top] = a;

    printk(KERN_INFO "[System Call] os2024_push(); ---\n");
    print_stack();
    
    return 0;
}

SYSCALL_DEFINE0(os2024_pop) {
    int value;
    // underflow check
    if (top < 0) {
        printk(KERN_ALERT "Stack Underflow: No values to pop\n");
        return -ENODATA;  // return no data error
    }

    value = stack[top];
    top--;

    printk(KERN_INFO "[System Call] os2024_pop(); ---\n");
    print_stack();
    
    return value;
}
