# Memory-Leakage-Tool
A lightweight, real-time memory leak detection tool for C programs on Linux. This tool intercepts malloc and free calls, logs allocation metadata, detects memory leaks, and optionally frees leaked memory at program termination. It helps developers debug memory issues in native applications without modifying source code.

✨ Features

Detects memory leaks at runtime using LD_PRELOAD
Logs stack traces for every unfreed allocation
Automatically frees leaked memory to prevent resource exhaustion
Thread-safe using pthreads
Works with any C/C++ program on Linux

🔧 How It Works

Overrides malloc() and free() using LD_PRELOAD and dlsym
Tracks memory allocations and deallocations in a linked list
On program exit, prints leaked memory info (address, size, and stack trace)
Automatically frees any memory not explicitly freed by the program

🚀 Quick Start

Clone or copy this repository

Compile the shared library:

bash
Copy
Edit
gcc -shared -fPIC -o memleakdetector.so memleakdetector.c -ldl -lpthread
Compile your test program (example):

bash
Copy
Edit
gcc -o test test.c
Run your program with memory leak detection enabled:

bash
Copy
Edit
LD_PRELOAD=./memleakdetector.so ./test
Output (on program exit):

bash
Copy
Edit
Memory leaks detected and automatically freeing:
Leak: 0x556e54013260 of size 100
  ./test(main+0x24) [0x556e54012414]
  /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xea) [...]
  ./test(_start+0x2a) [0x556e540123aa]
🧪 Example Test Code (test.c)

c
Copy
Edit
#include <stdlib.h>

int main() {
    char *leak = (char *)malloc(100); // not freed
    char *ok = (char *)malloc(50);
    free(ok);
    return 0;
}
📈 Real-Time Analysis Use Case

Analyze memory usage in complex C/C++ applications

Catch leaks early in development and automated test pipelines

Prevent long-running daemons or services from exhausting memory

Aid teaching/learning OS memory management concepts in academics

💡 Research & Innovation Notes

This tool implements real-time detection and remediation of memory leaks with minimal overhead. It can be extended with:

Logging to file or external monitoring service

Visualization of leak trends

Integration into CI/CD pipelines

Real-time alerts for embedded systems or IoT applications

📂 File Structure

memleakdetector.c — Main source file

test.c — Sample C program for testing (user-created)

memleakdetector.so — Compiled shared object (generated)

📌 Requirements

Linux (any distro)

gcc

pthreads and dynamic linking (dlopen)
