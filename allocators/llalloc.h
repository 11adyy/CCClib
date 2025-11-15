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

static unsigned char _buffer[ALLOC_BUFFER_SIZE] = { 0 };
static mm_llblock_t* _mm_head = (mm_llblock_t*)_buffer;
static mm_info_t _mm_info = { .allocated = 0 };

int ll_init() {
    if (_mm_head->magic != BLOCK_MAGIC) {
        _mm_head->magic = BLOCK_MAGIC;
        _mm_head->size  = ALLOC_BUFFER_SIZE - sizeof(mm_llblock_t);
        _mm_head->free  = 1;
        _mm_head->next  = NULL;
    }
    
    return 1;
}

static int __ll_coalesce_memory() {
    int merged = 0;
    mm_llblock_t* current = _mm_head;
    
    do {
        merged = 0;
        current = _mm_head;

        while (current && current->magic == BLOCK_MAGIC && current->next) {
            if (current->free && current->next->free) {
                current->size += sizeof(mm_llblock_t) + current->next->size;
                current->next = current->next->next;
                merged = 1;
            } 
            else {
                current = current->next;
            }
        }
    } while (merged);
    return 1;
}

static void* __ll_malloc(unsigned int size, unsigned int offset, int prepare_mem) {
    if (!size) return NULL;
    if (prepare_mem) __ll_coalesce_memory();

    size   = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    offset = (offset + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    mm_llblock_t* current = _mm_head;
    
    while (current && current->magic == BLOCK_MAGIC) {
        void* aligned_addr = (unsigned char*)current + sizeof(mm_llblock_t);
        unsigned int position = (unsigned int)((unsigned char*)aligned_addr - _buffer);
        if (current->free && current->size >= size && position >= offset) {
            if (current->size >= size + sizeof(mm_llblock_t)) {
                mm_llblock_t* new_block = (mm_llblock_t*)((unsigned char*)current + sizeof(mm_llblock_t) + size);
                new_block->magic = BLOCK_MAGIC;
                new_block->size = current->size - size - sizeof(mm_llblock_t);
                new_block->free = 1;
                new_block->next = current->next;

                current->next = new_block;
                current->size = size;
            }

            current->free = 0;
            return (unsigned char*)current + sizeof(mm_llblock_t);
        }

        current = current->next;
    }

    return prepare_mem ? NULL : __ll_malloc(size, offset, 1);
}

void* ll_malloc(unsigned int size) {
    void* ptr = __ll_malloc(size, NO_OFFSET, 0);
    if (ptr) _mm_info.allocated += size;
    return ptr;
}

void* ll_mallocoff(unsigned int size, unsigned int offset) {
    void* ptr = __ll_malloc(size, offset, 0);
    if (ptr) _mm_info.allocated += size;
    return ptr;
}

int ll_free(void* ptr) {
    if (!ptr || ptr < (void*)_buffer || ptr >= (void*)(_buffer + ALLOC_BUFFER_SIZE)) {
        return 0;
    }
    
    mm_llblock_t* block = (mm_llblock_t*)((unsigned char*)ptr - sizeof(mm_llblock_t));
    if (block->magic != BLOCK_MAGIC) return 0;
    if (block->free) return 0;

    block->free = 1;
    _mm_info.allocated -= block->size;
    return 1;
}

static int ll_info(mm_info_t* i) {
    i->allocated = _mm_info.allocated;
    return 1;
}

#ifdef __cplusplus
}
#endif
#endif