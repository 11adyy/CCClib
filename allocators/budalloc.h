#ifndef BDALLOC_H_
#define BDALLOC_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
    #define NULL ((void*)0)
#endif
#ifndef ALLOC_BUFFER_SIZE
    #define ALLOC_BUFFER_SIZE 131072
#endif
#define MIN_BLOCK   16
#define ALIGNMENT   8
#define MAX_LEVELS  14
#define BLOCK_MAGIC 0xBADDCAFE

typedef struct {
    unsigned long allocated;
} mm_info_t;

typedef struct mm_bdblock {
    unsigned int       magic;
    unsigned int       size;
    struct mm_bdblock* next;
} mm_bdblock_t;

void bd_init();
void* bd_malloc(unsigned int size);
int bd_free(void* ptr);
int bd_info(mm_info_t* info);

#ifdef __cplusplus
}
#endif
#endif