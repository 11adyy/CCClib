#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <binalloc.h>

int main() {
    b_init();
    char* src = "Hello, world!";
    char* msg = (char*)b_malloc(16);
    assert(msg);

    for (int i = 0; i < 16 && src[i]; i++) {
        msg[i] = src[i];
    }

    mm_info_t mi;
    b_info(&mi);

    assert(mi.allocated == 64);
    assert(!strcmp(src, msg));

    b_free(msg);
    b_info(&mi);
    assert(mi.allocated == 0);
    return 0;
}
