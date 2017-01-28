#ifndef PROC_H_INCLUDED
#define PROC_H_INCLUDED

#include <stdint.h>

typedef struct {
	// Process stack.
	uint8_t *stack;
	uint64_t stackSizeBytes;
	uint8_t *stackVirtualAddress;

	// Process heap.
	uint8_t *heap;
	uint64_t heapSizeBytes;
	uint8_t *heapVirtualAddress;

	// Process writable data segment.
	uint8_t *dataSegment;
	uint64_t dataSegmentSizeBytes;
	uint8_t *dataSegmentVirtualAddress;
} proc;

// Returns a proc object that contains a copy of the heap, stack and writable
// data segment of the process with process ID `pid`.
proc readProcData(uint32_t pid);

#endif

