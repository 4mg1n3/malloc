#include "malloc.h"

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    if (size == 0)
        return NULL;
    return my_malloc(size);
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    if (!ptr)
        return;
    my_free(ptr);
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    size_t total;
    if (__builtin_mul_overflow(nmemb, size, &total))
        return NULL;

    void *p = my_malloc(total);
    if (!p)
        return NULL;

    memset(p, 0, total);
    return p;
}

static size_t get_allocation_size_internal(void *ptr)
{
    if (!ptr)
        return 0;

    pthread_mutex_lock(&g_alloc.lock);

    struct page_header *page = g_alloc.pages;
    while (page)
    {
        void *start = page + 1;
        void *end = (char *)start + page->block_size * page->blocks_per_page;
        if (ptr >= start && ptr < end)
        {
            size_t sz = page->block_size;
            pthread_mutex_unlock(&g_alloc.lock);
            return sz;
        }
        page = page->next;
    }

    pthread_mutex_unlock(&g_alloc.lock);

    size_t *meta = (size_t *)ptr - 1;
    return *meta ? *meta - sizeof(size_t) : 0;
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return malloc(size);

    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    size_t old_size = get_allocation_size_internal(ptr);
    if (old_size >= size)
        return ptr;

    void *newptr = my_malloc(size);
    if (!newptr)
        return NULL;

    size_t copy_size = old_size < size ? old_size : size;
    memcpy(newptr, ptr, copy_size);
    my_free(ptr);

    return newptr;
}
