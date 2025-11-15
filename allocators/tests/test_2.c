#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <budalloc.h>

int main() {
    bd_init();
    char* src = "Hello, world!";
    char* msg = (char*)bd_malloc(16);
    assert(msg);

    for (int i = 0; i < 16 && src[i]; i++) {
        msg[i] = src[i];
    }

    mm_info_t mi;
    bd_info(&mi);

    assert(mi.allocated == 16);
    assert(!strcmp(src, msg));

    bd_free(msg);
    bd_info(&mi);

    assert(mi.allocated == 0);
    return 0;
}
