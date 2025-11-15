#ifndef BALLOC_H_
#define BALLOC_H_
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
#define BLOCK_MAGIC 0xC07DEL

typedef struct {
    unsigned long allocated;
} mm_info_t;

typedef struct mm_bblock {
    unsigned int      magic;
    unsigned int      size;
    unsigned char     free;
    struct mm_bblock* l;
    struct mm_bblock* r;
} mm_bblock_t;

int b_init();
void* b_malloc(unsigned int size);
int b_free(void* ptr);
int b_info(mm_info_t* i);

#ifdef __cplusplus
}
#endif
#endif