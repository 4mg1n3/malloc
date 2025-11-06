#ifndef INTERNAL_MALLOC_H
#define INTERNAL_MALLOC_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define ALIGNMENT (sizeof(long double))

struct page_header
{
    size_t block_size;
    size_t blocks_per_page;
    uint64_t bitmap;
    struct page_header *next;
};

struct allocator_state
{
    struct page_header *pages;
    pthread_mutex_t lock;
};

extern struct allocator_state g_alloc;

/* internal allocator functions */
void *my_malloc(size_t size);
void my_free(void *ptr);

/* optional helpers for calloc/realloc */
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);

/* size query for realloc (returns usable bytes or 0) */
size_t get_allocation_size(void *ptr);

#endif
