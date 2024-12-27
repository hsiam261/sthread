#ifndef STACK_UTIL_H
#define STACK_UTIL_H

#include <sthread.h>

void* allocate_stack(const sthread_attr* thread_attr);

int deallocate_stack(const sthread_attr* thread_attr);

#endif
