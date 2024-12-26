#include <sthread.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <linux/futex.h>
#include <unistd.h>
#include <limits.h>

#include "../common/defaults.h"
#include "../stack/stack.h"

static void thread_done(sthread_t tcb, int ret_val) {
	tcb->lock = 1;
	tcb->return_value = ret_val;
	syscall(SYS_futex, &tcb->lock, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
	syscall(SYS_exit, 0);
}

static void thread_func_wrapper(void *(*func)(void *), void * args, sthread_t tcb) {
	func(args);
	thread_done(tcb, 0);
}

int sthread_create(sthread_t* thread, sthread_attr* thread_attr, void * (*func)(void *), void* args) {
	if(thread_attr == NULL) {
		thread_attr = &default_sthread_attr;
	}

	void* stack_ptr = allocate_stack(thread_attr);
	*thread = stack_ptr;
	sthread_t tcb = stack_ptr;
	tcb->lock = 0;

	int clone_flags = (CLONE_VM | CLONE_THREAD | CLONE_SIGHAND |
			CLONE_FILES | CLONE_FS | CLONE_PARENT_SETTID |
			CLONE_CHILD_CLEARTID);

	struct clone_args cl_args = {
		.flags = clone_flags,
		.child_tid = (__u64)&(tcb->tid),
		.pidfd = (__u64)&(tcb->tid),
		.stack = (__u64)(stack_ptr),
		.stack_size = thread_attr->stack_size
	};

	pid_t pid;

	__asm__ volatile(
		"movq %5, %%rbx\n\t"    // start thread func (wrapper over func)
		"movq %2, %%r12\n\t"    // start thread func ARGUMENT 0: func
		"movq %6, %%r13\n\t"    // start thread func ARGUMENT 1: args
		"movq %7, %%r14\n\t"	// start thread func ARGUMENT 2: tcb
		"movq $0, %%rax\n\t"
		"mov %1, %%eax\n\t"     // SYSTEM CALL NUMBER : SYS_clone3
		"movq %3, %%rdi\n\t"    // zeroth argument to clone3: &cl_args
		"movq %4, %%rsi\n\t"    // first argument to clone3: sizeof(cl_args)
		"syscall\n\t"		// Linux/amd64 system call */
		"testq %%rax,%%rax\n\t"	// check return value */
		"jne 1f\n\t"		// jump if parent */
		"movq %%r12, %%rdi\n\t"    // FUNCTION ARGUMENT 0: func
		"movq %%r13, %%rsi\n\t"    // FUNCTION ARGUMENT 1: args
		"movq %%r14, %%rdx\n\t"	// FUNCTION ARGUMENT 2: tcb
		"callq *%%rbx\n\t"	// start subthread function */
		"1:\t"
		:"=a" (pid)
		:"i" (SYS_clone3),
		"r" (func),
		"r" (&cl_args),
		"r" (sizeof(cl_args)),
		"r" (&thread_func_wrapper),
		"r" (args),
		"r" (tcb)
		:"rdi", "rsi", "r12", "r13", "r14"
	);

	if(pid < 0) {
		return 1;
	}

	return 0;

}
