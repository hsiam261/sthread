#include "stack.h"
#include <sys/mman.h>

#include "../common/defaults.h"

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
