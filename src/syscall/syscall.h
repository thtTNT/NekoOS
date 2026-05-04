#pragma once

// POSIX-style syscall numbers
#define SYS_YIELD 0
#define SYS_WRITE 1
#define SYS_EXIT  2

// file descriptors
#define FD_STDOUT 1
#define FD_STDERR 2

void handleEcall();
