#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


int main(void)
{

    printf("%zu\n\n", next_pow2(129));
    size_t size = 64;
    void *ptr = malloc(size);
    void *ptr2 = malloc(size);
    void *ptr3 = malloc(256);
    if (!ptr || !ptr2 || !ptr3)
    {
        printf("malloc(%zu) failed\n", size);
        return 1;
    }

    printf("malloc(%zu) returned %p\n", size, ptr);
    printf("malloc(%zu) returned %p\n", size, ptr2);
    printf("malloc(%zu) returned %p\n", size, ptr3);

    memset(ptr, 0xAB, size);
    unsigned char *bytes = ptr;
    printf("First byte: 0x%X\n", bytes[0]);
    printf("Last byte:  0x%X\n", bytes[size - 1]);

    size_t n = 8;
    size_t s = 8;
    void *cptr = calloc(n, s);
    puts("a\n\n");
    if (!cptr)
    {
        printf("calloc(%zu, %zu) failed\n", n, s);
        free(ptr);
        return 1;
    }

    printf("calloc(%zu, %zu) returned %p\n", n, s, cptr);

    int zero_ok = 1;
    unsigned char *cbytes = cptr;
    for (size_t i = 0; i < n * s; i++)
    {
        if (cbytes[i] != 0)
        {
            zero_ok = 0;
            break;
        }
    }
    printf("calloc memory is %sinitialized to zero\n", zero_ok ? "" : "NOT ");

    free(ptr);
    free(ptr2);
    free(ptr3);
    free(cptr);
    printf("Freed all blocks.\n");
    return 0;
}
