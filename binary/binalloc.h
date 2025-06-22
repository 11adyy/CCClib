#ifndef BALLOC_H_
#define BALLOC_H_

#define NULL ((void*)0)
#define ALLOC_BUFFER_SIZE 131072
#define MIN_BLOCK         16
#define ALIGNMENT         8  
#define MM_BLOCK_MAGIC    0xC07DEL

typedef struct mm_block {
    unsigned int     magic;
    unsigned int     size;
    unsigned char    free;
    struct mm_block* l;
    struct mm_block* r;
} mm_block_t;

static unsigned char _buffer[ALLOC_BUFFER_SIZE] = { 0 };
static mm_block_t* _mm_head = (mm_block_t*)_buffer;

static int __block_init(mm_block_t* block, unsigned int size) {
    block->magic = MM_BLOCK_MAGIC;
    block->size  = size;
    block->free  = 1;
    block->l = _mm_head->r = NULL;
    return 1;
}

int b_init() {
    if (_mm_head->magic != MM_BLOCK_MAGIC) {
        __block_init(_mm_head, ALLOC_BUFFER_SIZE - sizeof(mm_block_t));
    }
    
    return 1;
}

static int __b_coalesce_memory(mm_block_t* block) {
    if (!block || !block->free) return 0;
    if (!block->l || !block->r) return 0;

    if (block->l->free && block->r->free) {
        block->l = NULL;
        block->r = NULL;
        return 1;
    }

    return 0;
}

static void* __b_malloc(unsigned int size, mm_block_t* block) {
    if (!block || !block->free) return NULL;
    if (block->size < size) return NULL;
    if (block->size < size + sizeof(mm_block_t) + MIN_BLOCK) {
        block->free = 0;
        return (unsigned char*)block + sizeof(mm_block_t);
    }

    if (!block->l && !block->r) {
        unsigned int half_size = block->size / 2;
        block->l = (mm_block_t*)((unsigned char*)block + sizeof(mm_block_t));
        __block_init(block->l, half_size - sizeof(mm_block_t));

        block->r = (mm_block_t*)((unsigned char*)block->l + half_size);
        __block_init(block->r, half_size - sizeof(mm_block_t));
    }

    void* res = __b_malloc(size, block->l);
    if (res) return res;

    res = __b_malloc(size, block->r);
    if (res) return res;

    if (__b_coalesce_memory(block)) {
        return __b_malloc(size, block);
    }

    return NULL;
}

void* b_malloc(unsigned int size) {
    if (!size) return NULL;
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    return __b_malloc(size, _mm_head);
}

int b_free(void* ptr) {
    if (!ptr || ptr < (void*)_buffer || ptr >= (void*)(_buffer + ALLOC_BUFFER_SIZE)) {
        return 0;
    }
    
    mm_block_t* block = (mm_block_t*)((unsigned char*)ptr - sizeof(mm_block_t));
    if (block->magic != MM_BLOCK_MAGIC) return 0;
    if (block->free) return 0;

    block->free = 1;
    return 1;
}

#endif