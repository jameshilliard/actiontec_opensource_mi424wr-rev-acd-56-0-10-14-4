#include <stdlib.h>
#include <syscalls.h>
#include <util/rg_random.h>

/* We need functions below to compile a reduced libcrypto */

int RAND_bytes(u8 *buf, int size)
{
    return random_buf_get(buf, size);
}

int RAND_pseudo_bytes(u8 *buf, int size)
{
    return RAND_bytes(buf, size);
}

void RAND_seed(const void *buf, int num)
{
    int random_seed;

    sys_get_random(&random_seed, 1);
    srand(random_seed);
}

void *CRYPTO_malloc(int num, const char *file, int line)
{
    return malloc(num);
}

char *ERR_error_string(unsigned long e,char *buf)
{
    return NULL;
}

unsigned long ERR_get_error(void )
{
    return 0;
}


