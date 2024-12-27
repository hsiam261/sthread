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
	printf("Hello World\n");
	fflush(stdout);
	return NULL;
}

int main() {
	sthread_t thread1, thread2;
	int ret = sthread_create(&thread1, NULL, child_proc, NULL);
	if (ret !=0) {
		perror("Error creating sthread\n");
		exit(1);
	}
	sthread_join(thread1, NULL);

	ret = sthread_create(&thread2, NULL, child_proc, NULL);
	if (ret !=0) {
		perror("Error creating sthread\n");
		exit(1);
	}
	sthread_join(thread2, NULL);
	return 0;
}
