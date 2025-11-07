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
    
    // Test with 128 small allocations to verify both bitmaps work
    void *ptrs[128];
    size_t size = 16;
    
    for (int i = 0; i < 128; i++)
    {
        ptrs[i] = malloc(size);
        if (!ptrs[i])
        {
            printf("malloc(%zu) failed at allocation %d\n", size, i);
            return 1;
        }
    }
    
    printf("Successfully allocated 128 blocks of %zu bytes\n", size);
    printf("First block: %p\n", ptrs[0]);
    printf("64th block:  %p\n", ptrs[63]);
    printf("65th block:  %p\n", ptrs[64]);
    printf("128th block: %p\n", ptrs[127]);
    
    // Test writing to blocks in both bitmap ranges
    memset(ptrs[0], 0xAA, size);
    memset(ptrs[63], 0xBB, size);
    memset(ptrs[64], 0xCC, size);
    memset(ptrs[127], 0xDD, size);
    
    unsigned char *b0 = ptrs[0];
    unsigned char *b63 = ptrs[63];
    unsigned char *b64 = ptrs[64];
    unsigned char *b127 = ptrs[127];
    
    printf("Block 0 first byte:   0x%X\n", b0[0]);
    printf("Block 63 first byte:  0x%X\n", b63[0]);
    printf("Block 64 first byte:  0x%X\n", b64[0]);
    printf("Block 127 first byte: 0x%X\n", b127[0]);

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
