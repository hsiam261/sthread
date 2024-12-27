#include <sthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <linux/sched.h>

#include "../stack/stack.h"

int sthread_join(sthread_t thread, void** ret_val) {
	syscall(SYS_futex, &thread->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
	if(ret_val) *ret_val = thread->result;
	deallocate_stack(&thread->thread_attributes); //multiple joins on same thread will break things
	return 0;
}
