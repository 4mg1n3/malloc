#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PASS "\033[32m[PASS]\033[0m"
#define FAIL "\033[31m[FAIL]\033[0m"
#define INFO "\033[34m[INFO]\033[0m"

static int pass = 0;
static int fail = 0;

static void test(int cond, const char *name)
{
    if (cond)
    {
        printf("%s %s\n", PASS, name);
        pass++;
    }
    else
    {
        printf("%s %s\n", FAIL, name);
        fail++;
    }
}

void t1(void)
{
    printf("\n%s: 128*16 allocation\n", INFO);
    void *p[128];
    int ok = 1;
    for (int i = 0; i < 128; i++)
    {
        p[i] = malloc(16);
        if (!p[i]) { ok = 0; break; }
        memset(p[i], 0x42, 16);
    }
    test(ok, "128 allocs ok");
    ok = 1;
    for (int i = 0; i < 128 && p[i]; i++)
    {
        unsigned char *b = p[i];
        for (int j = 0; j < 16; j++)
            if (b[j] != 0x42) { ok = 0; break; }
    }
    test(ok, "data ok");
    for (int i = 0; i < 128; i++)
        if (p[i]) free(p[i]);
}

void t2(void)
{
    printf("\n%s: small allocs\n", INFO);
    size_t sz[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    void *p[11];
    int ok = 1;
    for (int i = 0; i < 11; i++)
    {
        p[i] = malloc(sz[i]);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz[i]);
    }
    test(ok, "small sizes ok");
    for (int i = 0; i < 11; i++)
        if (p[i]) free(p[i]);
}

void t3(void)
{
    printf("\n%s: large allocs\n", INFO);
    size_t sz[] = {2048, 4096, 8192, 16384, 65536, 1024*1024};
    void *p[6];
    int ok = 1;
    for (int i = 0; i < 6; i++)
    {
        p[i] = malloc(sz[i]);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz[i]);
    }
    test(ok, "large allocs ok");
    for (int i = 0; i < 6; i++)
        if (p[i]) free(p[i]);
}

void t4(void)
{
    printf("\n%s: zero size\n", INFO);
    void *p = malloc(0);
    test(p == NULL, "malloc(0) = NULL");
    printf("%p\n", p);
}

void t5(void)
{
    printf("\n%s: free NULL\n", INFO);
    free(NULL);
    test(1, "free(NULL) ok");
}

void t6(void)
{
    printf("\n%s: alloc-free-realloc\n", INFO);
    void *p[10];
    for (int i = 0; i < 10; i++)
    {
        p[i] = malloc(128);
        if (p[i]) memset(p[i], 0x42, 128);
    }
    for (int i = 1; i < 10; i += 2)
    {
        free(p[i]);
        p[i] = NULL;
    }
    int ok = 1;
    for (int i = 1; i < 10; i += 2)
    {
        p[i] = malloc(128);
        if (!p[i]) ok = 0;
    }
    test(ok, "realloc ok");
    for (int i = 0; i < 10; i++)
        if (p[i]) free(p[i]);
}

void t7(void)
{
    printf("\n%s: 1000 allocs\n", INFO);
    void **p = malloc(1000 * sizeof(void*));
    int cnt = 0;
    for (int i = 0; i < 1000; i++)
    {
        size_t sz = (i % 512) + 1;
        p[i] = malloc(sz);
        if (p[i])
        {
            cnt++;
            memset(p[i], 0x42, sz);
        }
    }
    test(cnt > 900, "1000 allocs >90%");
    for (int i = 0; i < 1000; i++)
        if (p[i]) free(p[i]);
    free(p);
}

void t8(void)
{
    printf("\n%s: alignment\n", INFO);
    void *p[20];
    int ok = 1;
    for (int i = 0; i < 20; i++)
    {
        p[i] = malloc(64);
        if (p[i] && ((size_t)p[i] % 16 != 0))
            ok = 0;
    }
    test(ok, "aligned");
    for (int i = 0; i < 20; i++)
        if (p[i]) free(p[i]);
}

void t9(void)
{
    printf("\n%s: interleaved sizes\n", INFO);
    void *p[100];
    int ok = 1;
    for (int i = 0; i < 100; i++)
    {
        size_t sz = (i % 4 == 0) ? 64 : (i % 4 == 1) ? 128 : (i % 4 == 2) ? 256 : 512;
        p[i] = malloc(sz);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz);
    }
    test(ok, "interleaved ok");
    for (int i = 0; i < 100; i++)
        if (p[i]) free(p[i]);
}

void t10(void)
{
    printf("\n%s0: fragmentation\n", INFO);
    void *p[50];
    for (int i = 0; i < 50; i++)
    {
        p[i] = malloc(64);
        if (p[i]) memset(p[i], 0x42, 64);
    }
    for (int i = 0; i < 50; i += 2)
    {
        free(p[i]);
        p[i] = NULL;
    }
    int ok = 1;
    for (int i = 0; i < 50; i += 2)
    {
        p[i] = malloc(64);
        if (!p[i]) ok = 0;
    }
    test(ok, "reuse ok");
    for (int i = 0; i < 50; i++)
        if (p[i]) free(p[i]);
}

void t11(void)
{
    printf("\n%s: boundary sizes\n", INFO);
    size_t sz[] = {15, 16, 17, 63, 64, 65, 127, 128, 129, 1023, 1024, 1025, 2047, 2048, 2049};
    void *p[15];
    int ok = 1;
    for (int i = 0; i < 15; i++)
    {
        p[i] = malloc(sz[i]);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz[i]);
    }
    test(ok, "boundary ok");
    for (int i = 0; i < 15; i++)
        if (p[i]) free(p[i]);
}

void t12(void)
{
    printf("\n%s: sequential\n", INFO);
    int ok = 1;
    for (int i = 0; i < 100; i++)
    {
        void *p = malloc(256);
        if (!p) { ok = 0; break; }
        memset(p, 0x42, 256);
        free(p);
    }
    test(ok, "100 cycles ok");
}

void t13(void)
{
    printf("\n%s: mixed stress\n", INFO);
    void *p[200];
    int ok = 1;
    srand(42);
    for (int i = 0; i < 200; i++)
    {
        size_t sz = (rand() % 2000) + 1;
        p[i] = malloc(sz);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz);
    }
    test(ok, "200 random ok");
    for (int i = 0; i < 200; i++)
    {
        int idx = rand() % 200;
        if (p[idx]) { free(p[idx]); p[idx] = NULL; }
    }
    for (int i = 0; i < 200; i++)
        if (p[i]) free(p[i]);
}

void t14(void)
{
    printf("\n%s: page boundary\n", INFO);
    void *p[10];
    size_t sz[] = {4090, 4094, 4095, 4096, 4097, 8190, 8192, 8194, 16384, 32768};
    int ok = 1;
    for (int i = 0; i < 10; i++)
    {
        p[i] = malloc(sz[i]);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz[i]);
    }
    test(ok, "page boundary ok");
    for (int i = 0; i < 10; i++)
        if (p[i]) free(p[i]);
}

void t15(void)
{
    printf("\n%s: corruption check\n", INFO);
    void *p[30];
    int ok = 1;
    for (int i = 0; i < 30; i++)
    {
        p[i] = malloc(128);
        if (p[i])
        {
            unsigned char *b = p[i];
            for (int j = 0; j < 128; j++)
                b[j] = (i * 7 + j) & 0xFF;
        }
    }
    for (int i = 0; i < 30; i++)
    {
        if (p[i])
        {
            unsigned char *b = p[i];
            for (int j = 0; j < 128; j++)
                if (b[j] != (unsigned char)((i * 7 + j) & 0xFF))
                { ok = 0; break; }
        }
    }
    test(ok, "no corruption");
    for (int i = 0; i < 30; i++)
        if (p[i]) free(p[i]);
}

void t16(void)
{
    printf("\n%s: pow2 sizes\n", INFO);
    void *p[12];
    int ok = 1;
    for (int i = 0; i < 12; i++)
    {
        size_t sz = 1 << i;
        p[i] = malloc(sz);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz);
    }
    test(ok, "pow2 ok");
    for (int i = 0; i < 12; i++)
        if (p[i]) free(p[i]);
}

void t17(void)
{
    printf("\n%s: alternating\n", INFO);
    void *p1, *p2;
    int ok = 1;
    for (int i = 0; i < 50; i++)
    {
        p1 = malloc(64);
        p2 = malloc(128);
        if (!p1 || !p2)
        {
            ok = 0;
            if (p1) free(p1);
            if (p2) free(p2);
            break;
        }
        memset(p1, 0x42, 64);
        memset(p2, 0x42, 128);
        free(p1);
        free(p2);
    }
    test(ok, "50 cycles ok");
}

void t18(void)
{
    printf("\n%s: 100MB\n", INFO);
    size_t sz = 100 * 1024 * 1024;
    void *p = malloc(sz);
    int ok = (p != NULL);
    if (ok)
    {
        unsigned char *b = p;
        b[0] = 0x42;
        b[sz - 1] = 0x42;
        ok = (b[0] == 0x42 && b[sz - 1] == 0x42);
        free(p);
    }
    test(ok, "100MB ok");
}

void t19(void)
{
    printf("\n%s: lots of pages\n", INFO);
    void *p[128];
    int ok = 1;
    for (int i = 0; i < 128; i++)
    {
        p[i] = malloc(64);
        if (!p[i]) { ok = 0; break; }
        memset(p[i], 0x42, 64);
    }
    test(ok, "128*64 ok");
    for (int i = 0; i < 128; i++)
        if (p[i]) free(p[i]);
}

int main(void)
{
    printf("\n=============================\n");
    printf("EXTENSIVE TESTSUITE\n");
    printf("=============================\n");
    t1(); t2(); t3(); t4(); t5(); t6(); t7(); t8(); t9(); t10();
    t11(); t12(); t13(); t14(); t15(); t16(); t17(); t18(); t19();
    printf("\n=============================\n");
    printf("Passed: %d | Failed: %d\n", pass, fail);
    printf("=============================\n\n");
    return fail > 0 ? 1 : 0;
}
