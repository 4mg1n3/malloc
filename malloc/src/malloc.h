#ifndef MALLOC_H
#define MALLOC_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define ALIGNMENT (sizeof(long double))

struct page_header
{
    uint64_t bitmap;
    size_t block_size;
    size_t blocks_per_page;
    struct page_header *next;
    char padd[52];
};

struct allocator_state
{
    struct page_header *pages;
    pthread_mutex_t lock;
};

extern struct allocator_state g_alloc;

void *my_malloc(size_t size);
void my_free(void *ptr);

void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);

size_t get_allocation_size(void *ptr);

#endif /* MALLOC_H */
