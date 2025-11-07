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
    printf("\n%s: various sizes\n", INFO);
    size_t sz[] = {1, 8, 16, 64, 128, 512, 1024, 2048, 8192, 65536, 1024*1024};
    void *p[11];
    int ok = 1;
    for (int i = 0; i < 11; i++)
    {
        p[i] = malloc(sz[i]);
        if (!p[i]) ok = 0;
        else memset(p[i], 0x42, sz[i]);
    }
    test(ok, "various sizes ok");
    for (int i = 0; i < 11; i++)
        if (p[i]) free(p[i]);
}

void t2(void)
{
    printf("\n%s: calloc basic\n", INFO);
    void *p = calloc(100, 4);
    int ok = (p != NULL);
    if (ok)
    {
        unsigned char *b = p;
        for (int i = 0; i < 400; i++)
            if (b[i] != 0) { ok = 0; break; }
    }
    test(ok, "calloc zeroed");
    if (p) free(p);
}

void t3(void)
{
    printf("\n%s: calloc sizes\n", INFO);
    size_t counts[] = {1, 10, 100, 1000};
    size_t sizes[] = {1, 8, 64, 512};
    int ok = 1;
    for (int i = 0; i < 4; i++)
    {
        void *p = calloc(counts[i], sizes[i]);
        if (!p) { ok = 0; break; }
        unsigned char *b = p;
        for (size_t j = 0; j < counts[i] * sizes[i]; j++)
            if (b[j] != 0) { ok = 0; break; }
        free(p);
        if (!ok) break;
    }
    test(ok, "calloc various sizes");
}

void t4(void)
{
    printf("\n%s: realloc grow\n", INFO);
    void *p = malloc(64);
    int ok = (p != NULL);
    if (ok)
    {
        memset(p, 0x42, 64);
        void *p2 = realloc(p, 256);
        ok = (p2 != NULL);
        if (ok)
        {
            unsigned char *b = p2;
            for (int i = 0; i < 64; i++)
                if (b[i] != 0x42) { ok = 0; break; }
            free(p2);
        }
    }
    test(ok, "realloc grow ok");
}

void t5(void)
{
    printf("\n%s: realloc shrink\n", INFO);
    void *p = malloc(1024);
    int ok = (p != NULL);
    if (ok)
    {
        memset(p, 0x42, 1024);
        void *p2 = realloc(p, 64);
        ok = (p2 != NULL);
        if (ok)
        {
            unsigned char *b = p2;
            for (int i = 0; i < 64; i++)
                if (b[i] != 0x42) { ok = 0; break; }
            free(p2);
        }
    }
    test(ok, "realloc shrink ok");
}

void t6(void)
{
    printf("\n%s: realloc NULL\n", INFO);
    void *p = realloc(NULL, 128);
    int ok = (p != NULL);
    if (ok)
    {
        memset(p, 0x42, 128);
        free(p);
    }
    test(ok, "realloc(NULL) ok");
}

void t7(void)
{
    printf("\n%s: realloc chain\n", INFO);
    void *p = malloc(16);
    int ok = (p != NULL);
    size_t sizes[] = {32, 64, 128, 256, 512, 1024};
    for (int i = 0; i < 6 && ok; i++)
    {
        void *p2 = realloc(p, sizes[i]);
        if (!p2) { ok = 0; free(p); break; }
        p = p2;
        memset(p, 0x42, sizes[i]);
    }
    if (ok) free(p);
    test(ok, "realloc chain ok");
}

void t8(void)
{
    printf("\n%s: edge cases\n", INFO);
    void *p1 = malloc(0);
    void *p2 = calloc(0, 10);
    void *p3 = realloc(NULL, 0);
    free(NULL);
    int ok = (p1 == NULL && p2 == NULL && p3 == NULL);
    test(ok, "edge cases ok");
}

void t9(void)
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
    test(ok, "16-byte aligned");
    for (int i = 0; i < 20; i++)
        if (p[i]) free(p[i]);
}

void t10(void)
{
    printf("\n%s: fragmentation\n", INFO);
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
    printf("\n%s: many allocs\n", INFO);
    void **p = malloc(500 * sizeof(void*));
    int cnt = 0;
    for (int i = 0; i < 500; i++)
    {
        size_t sz = (i % 256) + 1;
        p[i] = malloc(sz);
        if (p[i])
        {
            cnt++;
            memset(p[i], 0x42, sz);
        }
    }
    test(cnt > 450, "500 allocs >90%");
    for (int i = 0; i < 500; i++)
        if (p[i]) free(p[i]);
    free(p);
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

int main(void)
{
    printf("\n=============================\n");
    printf("EXTENSIVE TESTSUITE\n");
    printf("=============================\n");
    t1(); t2(); t3(); t4(); t5(); t6(); t7(); t8(); t9(); t10();
    t11(); t12(); t13();
    printf("\n=============================\n");
    printf("Passed: %d | Failed: %d\n", pass, fail);
    printf("=============================\n\n");
    return fail > 0 ? 1 : 0;
}
