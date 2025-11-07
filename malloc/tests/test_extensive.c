#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>

#define TEST_PASS "\033[32m[PASS]\033[0m"
#define TEST_FAIL "\033[31m[FAIL]\033[0m"
#define TEST_INFO "\033[34m[INFO]\033[0m"

static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT_TEST(condition, test_name) do { \
    if (condition) { \
        printf("%s %s\n", TEST_PASS, test_name); \
        tests_passed++; \
    } else { \
        printf("%s %s\n", TEST_FAIL, test_name); \
        tests_failed++; \
    } \
} while(0)

/* Test 1: Allocate 64 pointers of size 64 */
void test_64_allocations_size_64(void)
{
    printf("\n%s Test 1: Allocating 64 pointers of size 64\n", TEST_INFO);
    void *ptrs[64];
    int success = 1;
    
    for (int i = 0; i < 64; i++)
    {
        ptrs[i] = malloc(64);
        if (!ptrs[i])
        {
            printf("  Allocation %d failed\n", i);
            success = 0;
            break;
        }
        /* Write unique pattern to each block */
        memset(ptrs[i], i & 0xFF, 64);
    }
    
    ASSERT_TEST(success, "All 64 allocations succeeded");
    
    /* Verify data integrity */
    int data_ok = 1;
    for (int i = 0; i < 64 && ptrs[i]; i++)
    {
        unsigned char *bytes = ptrs[i];
        for (int j = 0; j < 64; j++)
        {
            if (bytes[j] != (unsigned char)(i & 0xFF))
            {
                data_ok = 0;
                break;
            }
        }
    }
    
    ASSERT_TEST(data_ok, "Data integrity maintained across all 64 blocks");
    
    /* Free all */
    for (int i = 0; i < 64; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
    
    printf("  Freed all 64 blocks\n");
}

/* Test 2: Various small sizes */
void test_small_sizes(void)
{
    printf("\n%s Test 2: Small size allocations\n", TEST_INFO);
    size_t sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    void *ptrs[11];
    int success = 1;
    
    for (int i = 0; i < 11; i++)
    {
        ptrs[i] = malloc(sizes[i]);
        if (!ptrs[i])
        {
            printf("  Allocation of size %zu failed\n", sizes[i]);
            success = 0;
        }
        else
        {
            memset(ptrs[i], 0xAA, sizes[i]);
        }
    }
    
    ASSERT_TEST(success, "All small size allocations succeeded");
    
    for (int i = 0; i < 11; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 3: Large allocations */
void test_large_allocations(void)
{
    printf("\n%s Test 3: Large allocations (>1024 bytes)\n", TEST_INFO);
    size_t sizes[] = {2048, 4096, 8192, 16384, 65536, 1024*1024};
    void *ptrs[6];
    int success = 1;
    
    for (int i = 0; i < 6; i++)
    {
        ptrs[i] = malloc(sizes[i]);
        if (!ptrs[i])
        {
            printf("  Large allocation of size %zu failed\n", sizes[i]);
            success = 0;
        }
        else
        {
            /* Write pattern to verify */
            memset(ptrs[i], 0xBB, sizes[i]);
        }
    }
    
    ASSERT_TEST(success, "All large allocations succeeded");
    
    for (int i = 0; i < 6; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 4: Zero size allocation */
void test_zero_size(void)
{
    printf("\n%s Test 4: Zero size allocation\n", TEST_INFO);
    void *ptr = malloc(0);
    ASSERT_TEST(ptr == NULL, "malloc(0) returns NULL");
}

/* Test 5: Free NULL pointer */
void test_free_null(void)
{
    printf("\n%s Test 5: Free NULL pointer\n", TEST_INFO);
    free(NULL);
    ASSERT_TEST(1, "free(NULL) doesn't crash");
}

/* Test 6: Allocate, free, reallocate pattern */
void test_alloc_free_realloc(void)
{
    printf("\n%s Test 6: Allocate-Free-Reallocate pattern\n", TEST_INFO);
    void *ptrs[10];
    
    /* Allocate */
    for (int i = 0; i < 10; i++)
    {
        ptrs[i] = malloc(128);
        if (ptrs[i])
            memset(ptrs[i], i, 128);
    }
    
    /* Free odd indices */
    for (int i = 1; i < 10; i += 2)
    {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    /* Reallocate freed slots */
    int success = 1;
    for (int i = 1; i < 10; i += 2)
    {
        ptrs[i] = malloc(128);
        if (!ptrs[i])
            success = 0;
    }
    
    ASSERT_TEST(success, "Reallocations after partial free succeeded");
    
    /* Free all */
    for (int i = 0; i < 10; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 7: Stress test - many allocations */
void test_stress_many_allocations(void)
{
    printf("\n%s Test 7: Stress test - 1000 allocations\n", TEST_INFO);
    void **ptrs = malloc(1000 * sizeof(void*));
    int success = 1;
    int alloc_count = 0;
    
    for (int i = 0; i < 1000; i++)
    {
        size_t size = (i % 512) + 1;
        ptrs[i] = malloc(size);
        if (ptrs[i])
        {
            alloc_count++;
            memset(ptrs[i], i & 0xFF, size);
        }
        else
        {
            success = 0;
        }
    }
    
    printf("  Successfully allocated %d/1000 blocks\n", alloc_count);
    ASSERT_TEST(alloc_count > 900, "Most allocations succeeded (>90%)");
    
    for (int i = 0; i < 1000; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
    
    free(ptrs);
}

/* Test 8: Alignment test */
void test_alignment(void)
{
    printf("\n%s Test 8: Pointer alignment\n", TEST_INFO);
    void *ptrs[20];
    int all_aligned = 1;
    
    for (int i = 0; i < 20; i++)
    {
        ptrs[i] = malloc(64);
        if (ptrs[i])
        {
            uintptr_t addr = (uintptr_t)ptrs[i];
            if (addr % 16 != 0)
            {
                printf("  Pointer %p not aligned to 16 bytes\n", ptrs[i]);
                all_aligned = 0;
            }
        }
    }
    
    ASSERT_TEST(all_aligned, "All pointers properly aligned");
    
    for (int i = 0; i < 20; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 9: Interleaved allocation sizes */
void test_interleaved_sizes(void)
{
    printf("\n%s Test 9: Interleaved allocation sizes\n", TEST_INFO);
    void *ptrs[100];
    int success = 1;
    
    for (int i = 0; i < 100; i++)
    {
        size_t size;
        if (i % 4 == 0)
            size = 64;
        else if (i % 4 == 1)
            size = 128;
        else if (i % 4 == 2)
            size = 256;
        else
            size = 512;
            
        ptrs[i] = malloc(size);
        if (!ptrs[i])
            success = 0;
        else
            memset(ptrs[i], i & 0xFF, size);
    }
    
    ASSERT_TEST(success, "Interleaved size allocations succeeded");
    
    for (int i = 0; i < 100; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 10: Double free detection (should not crash) */
void test_double_free(void)
{
    printf("\n%s Test 10: Double free handling\n", TEST_INFO);
    void *ptr = malloc(128);
    free(ptr);
    free(ptr);  /* Double free */
    ASSERT_TEST(1, "Double free doesn't crash");
}

/* Test 11: Fragmentation test */
void test_fragmentation(void)
{
    printf("\n%s Test 11: Fragmentation handling\n", TEST_INFO);
    void *ptrs[50];
    
    /* Allocate 50 blocks */
    for (int i = 0; i < 50; i++)
    {
        ptrs[i] = malloc(64);
        if (ptrs[i])
            memset(ptrs[i], i, 64);
    }
    
    /* Free every other block */
    for (int i = 0; i < 50; i += 2)
    {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    /* Try to allocate in the gaps */
    int realloc_success = 1;
    for (int i = 0; i < 50; i += 2)
    {
        ptrs[i] = malloc(64);
        if (!ptrs[i])
            realloc_success = 0;
    }
    
    ASSERT_TEST(realloc_success, "Fragmented space reused successfully");
    
    for (int i = 0; i < 50; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 12: Boundary sizes */
void test_boundary_sizes(void)
{
    printf("\n%s Test 12: Boundary size allocations\n", TEST_INFO);
    size_t sizes[] = {15, 16, 17, 63, 64, 65, 127, 128, 129, 
                      1023, 1024, 1025, 2047, 2048, 2049};
    void *ptrs[15];
    int success = 1;
    
    for (int i = 0; i < 15; i++)
    {
        ptrs[i] = malloc(sizes[i]);
        if (!ptrs[i])
        {
            printf("  Boundary size %zu failed\n", sizes[i]);
            success = 0;
        }
        else
        {
            memset(ptrs[i], 0xCC, sizes[i]);
        }
    }
    
    ASSERT_TEST(success, "All boundary size allocations succeeded");
    
    for (int i = 0; i < 15; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 13: Sequential allocation and free */
void test_sequential_alloc_free(void)
{
    printf("\n%s Test 13: Sequential allocate and free\n", TEST_INFO);
    int success = 1;
    
    for (int i = 0; i < 100; i++)
    {
        void *ptr = malloc(256);
        if (!ptr)
        {
            success = 0;
            break;
        }
        memset(ptr, i & 0xFF, 256);
        free(ptr);
    }
    
    ASSERT_TEST(success, "100 sequential alloc/free cycles succeeded");
}

/* Test 14: Mixed size stress test */
void test_mixed_size_stress(void)
{
    printf("\n%s Test 14: Mixed size stress test\n", TEST_INFO);
    void *ptrs[200];
    int success = 1;
    
    srand(time(NULL));
    
    for (int i = 0; i < 200; i++)
    {
        size_t size = (rand() % 2000) + 1;
        ptrs[i] = malloc(size);
        if (!ptrs[i])
        {
            success = 0;
        }
        else
        {
            memset(ptrs[i], i & 0xFF, size);
        }
    }
    
    ASSERT_TEST(success, "200 random size allocations succeeded");
    
    /* Free in random order */
    for (int i = 0; i < 200; i++)
    {
        int idx = rand() % 200;
        if (ptrs[idx])
        {
            free(ptrs[idx]);
            ptrs[idx] = NULL;
        }
    }
    
    /* Clean up remaining */
    for (int i = 0; i < 200; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 15: Page boundary test */
void test_page_boundary(void)
{
    printf("\n%s Test 15: Page boundary allocations\n", TEST_INFO);
    void *ptrs[10];
    int success = 1;
    
    /* Allocate sizes around page boundaries */
    size_t sizes[] = {4090, 4094, 4095, 4096, 4097, 8190, 8192, 8194, 16384, 32768};
    
    for (int i = 0; i < 10; i++)
    {
        ptrs[i] = malloc(sizes[i]);
        if (!ptrs[i])
        {
            printf("  Page boundary size %zu failed\n", sizes[i]);
            success = 0;
        }
        else
        {
            memset(ptrs[i], 0xDD, sizes[i]);
        }
    }
    
    ASSERT_TEST(success, "Page boundary allocations succeeded");
    
    for (int i = 0; i < 10; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 16: Verify no memory corruption */
void test_no_corruption(void)
{
    printf("\n%s Test 16: Memory corruption check\n", TEST_INFO);
    void *ptrs[30];
    int no_corruption = 1;
    
    /* Allocate and write unique patterns */
    for (int i = 0; i < 30; i++)
    {
        ptrs[i] = malloc(128);
        if (ptrs[i])
        {
            unsigned char *bytes = ptrs[i];
            for (int j = 0; j < 128; j++)
            {
                bytes[j] = (i * 7 + j) & 0xFF;
            }
        }
    }
    
    /* Verify patterns are intact */
    for (int i = 0; i < 30; i++)
    {
        if (ptrs[i])
        {
            unsigned char *bytes = ptrs[i];
            for (int j = 0; j < 128; j++)
            {
                if (bytes[j] != (unsigned char)((i * 7 + j) & 0xFF))
                {
                    printf("  Corruption detected in block %d at offset %d\n", i, j);
                    no_corruption = 0;
                    break;
                }
            }
        }
    }
    
    ASSERT_TEST(no_corruption, "No memory corruption detected");
    
    for (int i = 0; i < 30; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 17: Exact power of 2 sizes */
void test_power_of_2_sizes(void)
{
    printf("\n%s Test 17: Power of 2 size allocations\n", TEST_INFO);
    void *ptrs[12];
    int success = 1;
    
    for (int i = 0; i < 12; i++)
    {
        size_t size = 1 << i;  /* 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 */
        ptrs[i] = malloc(size);
        if (!ptrs[i])
        {
            printf("  Power of 2 size %zu failed\n", size);
            success = 0;
        }
        else
        {
            memset(ptrs[i], 0xEE, size);
        }
    }
    
    ASSERT_TEST(success, "All power of 2 allocations succeeded");
    
    for (int i = 0; i < 12; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

/* Test 18: Alternating alloc/free */
void test_alternating_alloc_free(void)
{
    printf("\n%s Test 18: Alternating allocate/free pattern\n", TEST_INFO);
    void *ptr1, *ptr2;
    int success = 1;
    
    for (int i = 0; i < 50; i++)
    {
        ptr1 = malloc(64);
        ptr2 = malloc(128);
        
        if (!ptr1 || !ptr2)
        {
            success = 0;
            if (ptr1) free(ptr1);
            if (ptr2) free(ptr2);
            break;
        }
        
        memset(ptr1, 0xAA, 64);
        memset(ptr2, 0xBB, 128);
        
        free(ptr1);
        free(ptr2);
    }
    
    ASSERT_TEST(success, "50 alternating alloc/free cycles succeeded");
}

/* Test 19: Very large allocation */
void test_very_large_allocation(void)
{
    printf("\n%s Test 19: Very large allocation (10MB)\n", TEST_INFO);
    size_t large_size = 10 * 1024 * 1024;
    void *ptr = malloc(large_size);
    
    int success = (ptr != NULL);
    if (success)
    {
        /* Write to first and last bytes to ensure it's accessible */
        unsigned char *bytes = ptr;
        bytes[0] = 0xFF;
        bytes[large_size - 1] = 0xFF;
        
        success = (bytes[0] == 0xFF && bytes[large_size - 1] == 0xFF);
        free(ptr);
    }
    
    ASSERT_TEST(success, "10MB allocation succeeded and accessible");
}

/* Test 20: Multiple 64-byte blocks from different pages */
void test_multiple_pages_64(void)
{
    printf("\n%s Test 20: Allocate beyond single page capacity\n", TEST_INFO);
    void *ptrs[128];  /* More than can fit in one page */
    int success = 1;
    
    for (int i = 0; i < 128; i++)
    {
        ptrs[i] = malloc(64);
        if (!ptrs[i])
        {
            printf("  Allocation %d failed\n", i);
            success = 0;
            break;
        }
        memset(ptrs[i], i & 0xFF, 64);
    }
    
    ASSERT_TEST(success, "128 allocations (multi-page) succeeded");
    
    for (int i = 0; i < 128; i++)
    {
        if (ptrs[i])
            free(ptrs[i]);
    }
}

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf("  EXTENSIVE MALLOC LIBRARY TEST SUITE  \n");
    printf("========================================\n");
    
    test_64_allocations_size_64();
    test_small_sizes();
    test_large_allocations();
    test_zero_size();
    test_free_null();
    test_alloc_free_realloc();
    test_stress_many_allocations();
    test_alignment();
    test_interleaved_sizes();
    test_double_free();
    test_fragmentation();
    test_boundary_sizes();
    test_sequential_alloc_free();
    test_mixed_size_stress();
    test_page_boundary();
    test_no_corruption();
    test_power_of_2_sizes();
    test_alternating_alloc_free();
    test_very_large_allocation();
    test_multiple_pages_64();
    
    printf("\n========================================\n");
    printf("  TEST RESULTS\n");
    printf("========================================\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("  Total:  %d\n", tests_passed + tests_failed);
    printf("========================================\n\n");
    
    return tests_failed > 0 ? 1 : 0;
}
