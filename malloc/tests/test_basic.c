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
    
    void *ptrs[128];
    size_t size = 16;
    
    for (int i = 0; i < 128; i++)
    {
        ptrs[i] = malloc(size);
        if (!ptrs[i])
        {
            printf("malloc(%zu) failed at %d\n", size, i);
            return 1;
        }
    }
    
    printf("Allocated 128 blocks of %zu bytes\n", size);
    printf("Block 0:   %p\n", ptrs[0]);
    printf("Block 63:  %p\n", ptrs[63]);
    printf("Block 64:  %p\n", ptrs[64]);
    printf("Block 127: %p\n", ptrs[127]);
    
    memset(ptrs[0], 0x42, size);
    memset(ptrs[63], 0x42, size);
    memset(ptrs[64], 0x42, size);
    memset(ptrs[127], 0x42, size);
    
    unsigned char *b0 = ptrs[0];
    unsigned char *b63 = ptrs[63];
    unsigned char *b64 = ptrs[64];
    unsigned char *b127 = ptrs[127];
    
    printf("b0[0]:   0x%X\n", b0[0]);
    printf("b63[0]:  0x%X\n", b63[0]);
    printf("b64[0]:  0x%X\n", b64[0]);
    printf("b127[0]: 0x%X\n", b127[0]);
    
    for (int i = 0; i < 128; i++)
        free(ptrs[i]);
    
    printf("Freed all blocks\n");
    return 0;
}
