#include <unistd.h>
#include <sched.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <linux/sched.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>
#include <errno.h>

void* child_proc(void * args) {
	for(int i = 0; i < 10; i++) {
        printf("In thread %d: i = %d\n", *((int *)(args)), i);
        fflush(stdout);
        sleep(5); //sleep for 5 seconds
    }
	return NULL;
}

int main() {
	sthread_t threads[10];
    int ids[10];

    for(int i=0; i<10; i++) {
        ids[i] = i;
        int ret = sthread_create(&threads[i], NULL, child_proc, &ids[i]);
        if (ret !=0) {
            perror("Error creating sthread\n");
            exit(1);
        }
    }

    for(int i=0; i<10; i++) {
        sthread_join(threads[i], NULL);
        printf("Joined on Thread %d\n", i);
    }

	return 0;
}
