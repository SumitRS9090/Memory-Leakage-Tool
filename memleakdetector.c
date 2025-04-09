#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>
#include <pthread.h>


#define MAX_STACK_FRAMES 10

// Structure to store information about each memory allocation
typedef struct MemInfo {
    void *ptr;
    size_t size;
    void *stack[MAX_STACK_FRAMES];
    int stack_size;
    struct MemInfo *next;
} MemInfo;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
MemInfo *allocations = NULL;

// Original malloc and free function pointers
void *(*original_malloc)(size_t) = NULL;
void (*original_free)(void *) = NULL;

// Function to log stack trace
void log_stack_trace(void **stack, int size) {
    char **symbols = backtrace_symbols(stack, size);
    if (symbols) {
        for (int i = 0; i < size; i++) {
            printf("  %s\n", symbols[i]);
        }
        free(symbols);
    }
}

// Custom malloc to intercept memory allocations
void *malloc(size_t size) {
    if (!original_malloc) {
        original_malloc = dlsym(RTLD_NEXT, "malloc");
    }

    void *ptr = original_malloc(size);

    // Log the allocation
    pthread_mutex_lock(&lock);
    MemInfo *info = (MemInfo *)original_malloc(sizeof(MemInfo));
    info->ptr = ptr;
    info->size = size;
    info->stack_size = backtrace(info->stack, MAX_STACK_FRAMES);
    info->next = allocations;
    allocations = info;
    pthread_mutex_unlock(&lock);

    return ptr;
}

// Custom free to intercept memory deallocations
void free(void *ptr) {
    if (!original_free) {
        original_free = dlsym(RTLD_NEXT, "free");
    }

    pthread_mutex_lock(&lock);
    MemInfo **current = &allocations;
    while (*current) {
        if ((*current)->ptr == ptr) {
            // Found the allocation to free
            MemInfo *info = *current;
            *current = info->next;
            original_free(info);  // Free the MemInfo structure
            break;
        }
        current = &(*current)->next;
    }
    pthread_mutex_unlock(&lock);

    original_free(ptr);
}

// Function to print all unfreed allocations
void report_memory_leaks() {
    pthread_mutex_lock(&lock);
    MemInfo *current = allocations;
    if (current) {
        printf("Memory leaks detected:\n");
        while (current) {
            printf("Leak: %p of size %zu\n", current->ptr, current->size);
            log_stack_trace(current->stack, current->stack_size);
            current = current->next;
        }
    } else {
        printf("No memory leaks detected.\n");
    }
    pthread_mutex_unlock(&lock);
}

// Destructor to check memory leaks at program exit
__attribute__((destructor))
void cleanup() {
    report_memory_leaks();
}
