#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <llalloc.h>

int main() {
    ll_init();
    char* src = "Hello, world!";
    char* msg = (char*)ll_malloc(16);
    assert(msg);

    for (int i = 0; i < 16 && src[i]; i++) {
        msg[i] = src[i];
    }

    mm_info_t mi;
    ll_info(&mi);

    assert(mi.allocated == 16);
    assert(!strcmp(src, msg));

    ll_free(msg);
    ll_info(&mi);
    assert(mi.allocated == 0);
    return 0;
}
