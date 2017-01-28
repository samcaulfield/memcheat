#define _GNU_SOURCE

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/uio.h>

#include "proc.h"

proc readProcData(uint32_t pid)
{
#define FILENAME_LENGTH 100
	char fileName[FILENAME_LENGTH];
	snprintf(fileName, FILENAME_LENGTH, "/proc/%" PRIu32 "/maps", pid);

	proc process;
	memset(&process, 0, sizeof(process));

	FILE *file = fopen(fileName, "r");
	if (!file) {
		return process;
	}
	// fseek, fread etc. can't be used for this file, so use fgets to read
	// the file line-by-line instead.
	int numLines = 0;
#define LINEBUFFER_LENGTH 1024
	char lineBuffer[LINEBUFFER_LENGTH];
	char lineBufferCopy[LINEBUFFER_LENGTH];
#define FIRST_TOKEN_SIZE 100
	char firstToken[FIRST_TOKEN_SIZE];
	uint32_t lineNumber = 0;
	while (fgets(lineBuffer, LINEBUFFER_LENGTH, file)) {
		// Copy the line because strtok modifies it.
		strncpy(lineBufferCopy, lineBuffer, LINEBUFFER_LENGTH);

		char *token;
		token = strtok(lineBuffer, " ");
		strncpy(firstToken, token, FIRST_TOKEN_SIZE);
		while (token) {
			if (!strcmp(token, "[heap]\n")) {
				// Heap section
				char *start = strtok(firstToken, "-");
				char *end   = strtok(NULL, "-");

				uint64_t startN = strtoul(start, NULL, 16);
				uint64_t endN   = strtoul(end,   NULL, 16);

				process.heapSizeBytes = endN - startN;
				process.heapVirtualAddress = (uint8_t *) startN;
				process.heap = calloc(process.heapSizeBytes, sizeof(*process.heap));

				struct iovec local = {
					.iov_base = (void *) process.heap,
					.iov_len  = process.heapSizeBytes
				};
				struct iovec remote = {
					.iov_base = (void *) process.heapVirtualAddress,
					.iov_len  = process.heapSizeBytes
				};

				process_vm_readv(pid, &local, 1, &remote, 1, 0);
			} else if (!strcmp(token, "[stack]\n")) {
				// Stack section
				char *start = strtok(firstToken, "-");
				char *end   = strtok(NULL, "-");

				uint64_t startN = strtoul(start, NULL, 16);
				uint64_t endN   = strtoul(end,   NULL, 16);

				process.stackSizeBytes = endN - startN;
				process.stackVirtualAddress = (uint8_t *) startN;
				process.stack = calloc(process.stackSizeBytes, sizeof(*process.stack));

				struct iovec local = {
					.iov_base = (void *) process.stack,
					.iov_len  = process.stackSizeBytes
				};
				struct iovec remote = {
					.iov_base = (void *) process.stackVirtualAddress,
					.iov_len  = process.stackSizeBytes
				};

				process_vm_readv(pid, &local, 1, &remote, 1, 0);
			} else if (lineNumber == 2) {
				// Writable data segment
				char *start = strtok(firstToken, "-");
				char *end   = strtok(NULL, "-");

				uint64_t startN = strtoul(start, NULL, 16);
				uint64_t endN   = strtoul(end,   NULL, 16);

				process.dataSegmentSizeBytes = endN - startN;
				process.dataSegmentVirtualAddress = (uint8_t *) startN;

				process.dataSegment = calloc(process.dataSegmentSizeBytes, sizeof(*process.dataSegment));

				struct iovec local = {
					.iov_base = (void *) process.dataSegment,
					.iov_len  = process.dataSegmentSizeBytes
				};
				struct iovec remote = {
					.iov_base = (void *) process.dataSegmentVirtualAddress,
					.iov_len  = process.dataSegmentSizeBytes
				};

				process_vm_readv(pid, &local, 1, &remote, 1, 0);
				break; // There are other tokens afterwards.
			}

			token = strtok(NULL, " ");
		}

		lineNumber++;
	}

	return process;
}

