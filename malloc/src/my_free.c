#include "malloc.h"

void my_free(void *ptr)
{
    if (!ptr)
        return;

    pthread_mutex_lock(&g_alloc.lock);

    struct page_header *page = g_alloc.pages;
    struct page_header *prev = NULL;

    while (page)
    {
        void *page_start = page;
        void *page_end =
            (char *)page_start + page->block_size * page->blocks_per_page;

        if (ptr >= page_start && ptr < page_end)
            break;

        prev = page;
        page = page->next;
    }

    if (page)
    {
        size_t offset = (char *)ptr - (char *)(page + 1);
        size_t bit = offset / page->block_size;

        if (bit < 64)
            page->bitmap &= ~(1ULL << bit);

        if (page->bitmap == 0)
        {
            if (prev)
                prev->next = page->next;
            else
                g_alloc.pages = page->next;

            long pagesize = sysconf(_SC_PAGESIZE);
            if (pagesize <= 0)
                pagesize = 4096;

            munmap(page, pagesize);
        }

        pthread_mutex_unlock(&g_alloc.lock);
        return;
    }
    pthread_mutex_unlock(&g_alloc.lock);

    size_t *meta = (size_t *)ptr - 1;
    size_t alloc_size = *meta;

    if (alloc_size == 0)
        return;

    munmap(meta, alloc_size);
}
