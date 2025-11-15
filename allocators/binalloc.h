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

static unsigned char _buffer[ALLOC_BUFFER_SIZE] = { 0 };
static mm_bblock_t* _mm_head = (mm_bblock_t*)_buffer;
static mm_info_t _mm_info = { .allocated = 0 };

static int __block_init(mm_bblock_t* block, unsigned int size) {
    block->magic = BLOCK_MAGIC;
    block->size  = size;
    block->free  = 1;
    block->l = NULL;
    block->r = NULL;
    return 1;
}

static int b_init() {
    if (_mm_head->magic != BLOCK_MAGIC) {
        __block_init(_mm_head, ALLOC_BUFFER_SIZE - sizeof(mm_bblock_t));
    }
    return 1;
}

static int __b_coalesce_memory(mm_bblock_t* block) {
    if (!block || !block->free) return 0;
    if (!block->l || !block->r) return 0;

    if (block->l->free && block->r->free && 
        !block->l->l && !block->l->r && 
        !block->r->l && !block->r->r) {
        block->l = NULL;
        block->r = NULL;
        return 1;
    }

    return 0;
}

static void* __b_malloc(unsigned int size, mm_bblock_t* block) {
    if (!block || !block->free) return NULL;
    if (block->size < size) return NULL;

    unsigned int half_size = block->size / 2;
    if (half_size < sizeof(mm_bblock_t) + MIN_BLOCK || 
        half_size - sizeof(mm_bblock_t) < size) {
        block->free = 0;
        return (unsigned char*)block + sizeof(mm_bblock_t);
    }

    if (!block->l && !block->r) {
        block->l = (mm_bblock_t*)((unsigned char*)block + sizeof(mm_bblock_t));
        __block_init(block->l, half_size - sizeof(mm_bblock_t));

        block->r = (mm_bblock_t*)((unsigned char*)block->l + half_size);
        __block_init(block->r, half_size - sizeof(mm_bblock_t));
    }

    void* res = __b_malloc(size, block->l);
    if (res) return res;

    res = __b_malloc(size, block->r);
    if (res) return res;

    if (__b_coalesce_memory(block)) 
        return __b_malloc(size, block);
        
    return NULL;
}

static void* b_malloc(unsigned int size) {
    if (!size) return NULL;
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    void* ptr = __b_malloc(size, _mm_head);
    if (ptr) {
        mm_bblock_t* block = (mm_bblock_t*)((unsigned char*)ptr - sizeof(mm_bblock_t));
        _mm_info.allocated += block->size;
    }
    
    return ptr;
}

static int b_free(void* ptr) {
    if (!ptr || ptr < (void*)_buffer || ptr >= (void*)(_buffer + ALLOC_BUFFER_SIZE)) 
        return 0;
        
    mm_bblock_t* block = (mm_bblock_t*)((unsigned char*)ptr - sizeof(mm_bblock_t));
    if (block->magic != BLOCK_MAGIC) return 0;
    if (block->free) return 0;
    
    block->free = 1;
    _mm_info.allocated -= block->size;
    return 1;
}

static int b_info(mm_info_t* i) {
    i->allocated = _mm_info.allocated;
    return 1;
}

#ifdef __cplusplus
}
#endif
#endif