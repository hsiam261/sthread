#include <sthread.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <linux/futex.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include "../common/defaults.h"
#include "../stack/stack.h"

static void thread_done(sthread_t tcb, void* ret_val) {
	tcb->lock = 1;
	tcb->return_value = ret_val;
	syscall(SYS_futex, &tcb->lock, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
	syscall(SYS_exit, 0);
}

static void thread_func_wrapper(void *(*func)(void *), void * args, sthread_t tcb) {
	void* ret = func(args);
	thread_done(tcb, ret);
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

	// These variables are required just so that
	// we won't need to manually handle setting up
	// the registers in assembly
	// only relevant to the assembly code
	size_t sz = sizeof(struct clone_args);
	__u64 clone_args_addr = &cl_args;
	__u64 wrapper_func_addr = &thread_func_wrapper;

	__asm__ volatile(
		"movq %%rax, %%r15\n\t" // save value of rax
		// The next 4 values need to be copied into registers because
		// they are required by the child thread and the child won't be able
		// to read them from stack variables since the stack would change
		"movq %5, %%rbx\n\t"    // start thread func (wrapper over func) need
		"movq %2, %%r12\n\t"    // start thread func ARGUMENT 0: func need
		"movq %6, %%r13\n\t"    // start thread func ARGUMENT 1: args need
		"movq %7, %%r14\n\t"	// start thread func ARGUMENT 2: tcb need
		// setting the syscall value
		"movq $0, %%rax\n\t"
		"mov %1, %%eax\n\t"     // SYSTEM CALL NUMBER : SYS_clone3
		"movq %3, %%rdi\n\t"    // zeroth argument to clone3: &cl_args
		"movq %4, %%rsi\n\t"    // first argument to clone3: sizeof(cl_args)
		"syscall\n\t"		// Linux/amd64 system call */
		"testq %%rax,%%rax\n\t"	// check return value */
		"jne 1f\n\t"		// jump if parent (if return value is non zero)*/
		// call the function in child thread
		"movq %%r12, %%rdi\n\t"    // FUNCTION ARGUMENT 0: func
		"movq %%r13, %%rsi\n\t"    // FUNCTION ARGUMENT 1: args
		"movq %%r14, %%rdx\n\t"	// FUNCTION ARGUMENT 2: tcb
		"callq *%%rbx\n\t"	// start subthread function */
		"1:\n\t" // parent thread will jump to here
		"movq %%rax, %0\n\t" //copy syscall return to pid in parent thread
		"movq %%r15, %%rax\t" //restore rax value in in parent thread
		:"=m" (pid)
		:"i" (SYS_clone3),
		"m" (func),
		"m" (clone_args_addr),
		"m" (sz),
		"m" (wrapper_func_addr),
		"m" (args),
		"m" (tcb)
		:"rdi", "rsi", "rbx", "rdx", "r12", "r13", "r14", "r15"
	);

	if(pid < 0) {
		errno = -pid; // store errorno incase of error;
					  // syscalls return errorno * -1;
					  // negative value indicates an error
					  // and the value indicates the actual error
		return -1;
	}

	return 0;
}
