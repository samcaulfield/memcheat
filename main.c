#define _GNU_SOURCE

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/uio.h>

#include "proc.h"

#define PID 4515

#define CLI_BUF_SIZE_BYTES 1024

int main(int argc, char **argv)
{
	char cliBuf[CLI_BUF_SIZE_BYTES];

	proc p;

	while (1) {
		printf("Enter command: ");
		fgets(cliBuf, CLI_BUF_SIZE_BYTES, stdin);

		//"search <value>", "write <value> <address>"
		char *token = strtok(cliBuf, " ");

		if (!strcmp(token, "search")) {
			char *value = strtok(NULL, "\n");
			uint32_t target = (uint32_t) strtoul(value, NULL, 10);

			p = readProcData(PID);

			for (uint64_t i = 0; i < p.stackSizeBytes / sizeof(target); i++) {
				const uint32_t value = *(((uint32_t *) p.stack) + i);

				if (value == target) {
					const uint64_t byteOffset = i * sizeof(target);
					printf("Hit at 0x%08lx\n", (uint64_t) p.stackVirtualAddress + byteOffset);
				}
			}

			for (uint64_t i = 0; i < p.heapSizeBytes / sizeof(target); i++) {
				const uint32_t value = *(((uint32_t *) p.heap) + i);

				if (value == target) {
					const uint64_t byteOffset = i * sizeof(target);
					printf("Hit at 0x%08lx\n", (uint64_t) p.heapVirtualAddress + byteOffset);
				}
			}

			for (uint64_t i = 0; i < p.dataSegmentSizeBytes / sizeof(target); i++) {
				const uint32_t value = *(((uint32_t *) p.dataSegment) + i);

				if (value == target) {
					const uint64_t byteOffset = i * sizeof(target);
					printf("Hit at 0x%08lx\n", (uint64_t) p.dataSegmentVirtualAddress + byteOffset);
				}
			}
		}

		if (!strcmp(token, "write")) {
			char *valueToken = strtok(NULL, " ");
			uint32_t value = (uint32_t) strtoul(valueToken, NULL, 10);

			char *addressToken = strtok(NULL, "\n");
			void *address = (void *) strtoul(addressToken, NULL, 16);

			struct iovec local, remote;
			local.iov_base = &value;
			local.iov_len = 4;
			remote.iov_base = address;
			remote.iov_len = 4;

			const ssize_t ret = process_vm_writev(PID, &local, 1, &remote, 1, 0);
			if (ret == -1) {
				switch (errno) {
				case EFAULT:
					printf("EFAULT\n");
					break;
				case EINVAL:
					printf("EINVAL\n");
					break;
				case ENOMEM:
					printf("ENOMEM\n");
					break;
				case EPERM:
					printf("EPERM\n");
					break;
				case ESRCH:
					printf("ESRCH\n");
					break;
				}
			}
		}

	}
	return EXIT_SUCCESS;
}

