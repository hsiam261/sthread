#ifndef STHREAD_H
#define STHREAD_H

#include <stdint.h>
#include <stddef.h>

typedef struct sthread_attr {
	void *stack_ptr;
	size_t stack_size;
} sthread_attr;

typedef struct sthread {
	int return_value;
	void* result;
	uint32_t lock;
	uint64_t tid;
	int pidfd;
	sthread_attr thread_attributes;
} sthread;

typedef sthread* sthread_t;

int sthread_create(sthread_t* thread, sthread_attr* thread_attr, void * (*func)(void *), void* args);
int sthread_join(sthread_t thread, void** ret_val);

#endif
