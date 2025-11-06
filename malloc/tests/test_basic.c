#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    size_t size = 4096;
    void *ptr = malloc(size);
    if (!ptr)
    {
        printf("malloc(%zu) failed\n", size);
        return 1;
    }

    printf("malloc(%zu) returned %p\n", size, ptr);

    memset(ptr, 0xAB, size);
    unsigned char *bytes = ptr;
    printf("First byte: 0x%X\n", bytes[0]);
    printf("Last byte:  0x%X\n", bytes[size - 1]);

    size_t n = 10;
    size_t s = 8;
    void *cptr = calloc(n, s);
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
    free(cptr);
    printf("Freed all blocks.\n");
    return 0;
}
