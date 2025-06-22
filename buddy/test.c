#include "budalloc.h"
#include <stdio.h>
#include <string.h>

int main() {
    b_init();
    char* data = (char*)b_malloc(20);
    strcpy(data, "Hello world!");
    printf("%s\n", data);
    b_free(data);
    return 0;
}
