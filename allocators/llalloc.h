#ifndef LLALLOC_H_
#define LLALLOC_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
    #define NULL ((void*)0)
#endif
#ifndef ALLOC_BUFFER_SIZE
    #define ALLOC_BUFFER_SIZE 131072
#endif
#define ALIGNMENT   8  
#define BLOCK_MAGIC 0xC07DEL
#define NO_OFFSET   0

typedef struct {
    unsigned long allocated;
} mm_info_t;

typedef struct mm_llblock {
    unsigned int       magic;
    unsigned int       size;
    unsigned char      free;
    struct mm_llblock* next;
} mm_llblock_t;

int ll_init();
void* ll_malloc(unsigned int size);
void* ll_mallocoff(unsigned int size, unsigned int offset);
int ll_free(void* ptr);
int ll_info(mm_info_t* i);

#ifdef __cplusplus
}
#endif
#endif