#include "llalloc.h"
#include <stdio.h>
#include <string.h>

int main() {
    ll_init();
    char* data = (char*)ll_malloc(20);
    strcpy(data, "Hello world!");
    printf("%s\n", data);
    ll_free(data);
    return 0;
}
