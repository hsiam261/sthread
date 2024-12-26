#ifndef STACK_UTIL_H
#define STACK_UTIL_H

#include <sthread.h>

void* allocate_stack(sthread_attr* thread_attr);

#endif
