#include "malloc.h"

struct allocator_state g_alloc = { NULL, PTHREAD_MUTEX_INITIALIZER };

static size_t next_pow2(size_t n)
{
    if (n <= 16)
        return 16;
    if (n > 2048)
        return n;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}

static struct page_header *new_page(size_t block_size)
{
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize <= 0)
        pagesize = 4096;

    void *mem = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED)
        return NULL;

    struct page_header *page = mem;
    page->block_size = block_size;
    page->blocks_per_page =
        (pagesize - sizeof(struct page_header)) / block_size;
    if (page->blocks_per_page > 64)
        page->blocks_per_page = 64;
    page->bitmap = UINT64_MAX;
    for (size_t bit = 0; bit < page->blocks_per_page; bit++)
        page->bitmap ^= (1 << bit);
    page->next = g_alloc.pages;
    g_alloc.pages = page;

    return page;
}

static void *alloc_big(size_t size)
{
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize <= 0)
        pagesize = 4096;

    size_t aligned = (size + pagesize - 1) & ~(pagesize - 1);

    void *mem = mmap(NULL, aligned, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED)
        return NULL;

    size_t *meta = mem;
    *meta = aligned;
    return meta + 1;
}

void *my_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    if (size > 1024)
        return alloc_big(size);

    size_t block_size = next_pow2(size);
    if (block_size < 64)
        block_size = 64;

    pthread_mutex_lock(&g_alloc.lock);

    struct page_header *page = g_alloc.pages;
    while (page)
    {
        if (page->block_size == block_size && page->bitmap != UINT64_MAX)
            break;
        page = page->next;
    }

    if (!page)
    {
        page = new_page(block_size);
        if (!page)
        {
            pthread_mutex_unlock(&g_alloc.lock);
            return NULL;
        }
    }

    size_t bit;
    for (bit = 0; bit < page->blocks_per_page; bit++)
    {
        if (!(page->bitmap & (1 << bit)))
        {
            page->bitmap |= (1 << bit);
            break;
        }
    }
    void *vpage = page;
    char *endofpage = vpage;
    endofpage += 4096;
    endofpage -= (page->blocks_per_page + 1 - bit) * (page->block_size / 8);

    void *ptr = endofpage;

    pthread_mutex_unlock(&g_alloc.lock);
    return ptr;
}
