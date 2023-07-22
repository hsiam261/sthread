#include <stddef.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <linux/sched.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct sthread_attr {
	void *stack_ptr;
	size_t stack_size;
} sthread_attr;

typedef struct sthread {
	int return_value;
	uint32_t lock;
	uint64_t tid;
	int pidfd;
	sthread_attr thread_attributes; 
} sthread;

typedef sthread* sthread_t;

sthread_attr default_sthread_attr = { NULL, 4096 };

void initialize_default_sthread_attrs() {
	default_sthread_attr.stack_size = 1<<22;
}

void* allocate_stack(sthread_attr* thread_attr) {
	if(thread_attr == NULL) {
		thread_attr = &default_sthread_attr;
	}

	if(thread_attr->stack_ptr) {
		return thread_attr->stack_ptr;
	}

	void* stack_ptr = mmap(NULL, thread_attr->stack_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE| MAP_STACK, 0, 0);
	if(stack_ptr == MAP_FAILED) {
		return NULL;
	}

	thread_attr->stack_ptr = stack_ptr;
	return stack_ptr;
}

void thread_done(sthread_t tcb, int ret_val) {
	tcb->lock = 1;
	tcb->return_value = ret_val;
	syscall(SYS_futex, &tcb->lock, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
}

void sthread_create(sthread_t* thread, sthread_attr* thread_attr, void * (*func)(void *), void* args) {
	if(thread_attr == NULL) {
		thread_attr = &default_sthread_attr;
	}

	void* stack_ptr = allocate_stack(thread_attr);
	*thread = stack_ptr;
	sthread_t tcb = stack_ptr;

	int clone_flags = (CLONE_VM | CLONE_THREAD | CLONE_SIGHAND | 
			CLONE_FILES | CLONE_FS | CLONE_PARENT_SETTID | 
			CLONE_CHILD_CLEARTID);
	printf("%p\n", stack_ptr+thread_attr->stack_size);
	struct clone_args cl_args = {
		.flags = clone_flags,
		.child_tid = (__u64)&(tcb->tid),
		.pidfd = (__u64)&(tcb->tid),
		.stack = (__u64)(stack_ptr),
		.stack_size = thread_attr->stack_size
	};

	printf("Here\n");
	fflush(stdout);

	pid_t pid;
	
	__asm__ volatile(
		"movq %3, %%rbx\n\t"
		"movq $0, %%rax\n\t"
		"mov %1, %%eax\n\t"
		"movq %4, %%rdi\n\t"
		"movq %5, %%rsi\n\t"
		"syscall\n\t"		/* Linux/i386 system call */
		"testq %%rax,%%rax\n\t"	/* check return value */
		"jne 1f\n\t"		/* jump if parent */
		"callq *%%rbx\n\t"		/* start subthread function */
		"movl %2,%0\n\t"
		"syscall\n"		/* exit system call: exit subthread */
		"1:\t"
		:"=a" (pid)
		:"i" (SYS_clone3),"i" (__NR_exit),
		"r" (func),
		"r" (&cl_args),
		"r" (sizeof(cl_args))
		:"rdi", "rsi"
	);

	if(pid < 0) {
		printf("Oh man %d\n",pid);
	}

	printf("Here2\n");
}

void* child_proc(void * args) {
	printf("Hello World\n");
	fflush(stdout);
	return NULL;
}

int main() {
	initialize_default_sthread_attrs();

	sthread_t thread1, thread2;
	sthread_create(&thread1, NULL, child_proc, NULL);
	sleep(5);
	sthread_create(&thread2, NULL, child_proc, NULL);
	sleep(5);
	return 0;
}
