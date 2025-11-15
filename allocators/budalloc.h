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

static unsigned char _buffer[ALLOC_BUFFER_SIZE] = { 0 };
static mm_bdblock_t* _free_lists[MAX_LEVELS] = { NULL };
static mm_info_t _mm_info = { .allocated = 0 };

static unsigned int __next_power_of_two(unsigned int size) {
    unsigned int n = MIN_BLOCK;
    while (n < size) n <<= 1;
    return n;
}

static int __get_level(unsigned int size) {
    int level = 0;
    unsigned int s = MIN_BLOCK;
    while (s < size && level < MAX_LEVELS - 1) {
        s <<= 1;
        level++;
    }
    return level;
}

static void bd_init() {
    for (int i = 0; i < MAX_LEVELS; ++i) _free_lists[i] = NULL;
    mm_bdblock_t* block = (mm_bdblock_t*)_buffer;
    block->magic = BLOCK_MAGIC;
    block->size  = ALLOC_BUFFER_SIZE;
    block->next  = NULL;
    _free_lists[MAX_LEVELS - 1] = block;
}

static int __split_block(int from_level, int to_level) {
    while (from_level > to_level) {
        if (!_free_lists[from_level]) return 0;
        
        mm_bdblock_t* block = _free_lists[from_level];
        _free_lists[from_level] = block->next;

        unsigned int half_size = block->size >> 1;
        
        mm_bdblock_t* left = block;
        mm_bdblock_t* right = (mm_bdblock_t*)((unsigned char*)block + half_size);

        left->magic = BLOCK_MAGIC;
        left->size = half_size;
        left->next = _free_lists[from_level - 1];

        right->magic = BLOCK_MAGIC;
        right->size = half_size;
        right->next = left;

        _free_lists[from_level - 1] = right;
        from_level--;
    }
    return 1;
}

static void* bd_malloc(unsigned int size) {
    if (!size || size > ALLOC_BUFFER_SIZE - sizeof(mm_bdblock_t)) return NULL;
    
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    unsigned int total_size = size + sizeof(mm_bdblock_t);
    if (total_size < MIN_BLOCK) total_size = MIN_BLOCK;

    total_size = __next_power_of_two(total_size);
    int level = __get_level(total_size);

    int found_level = -1;
    for (int i = level; i < MAX_LEVELS; ++i) {
        if (_free_lists[i]) {
            found_level = i;
            break;
        }
    }

    if (found_level == -1) return NULL;
    
    if (!__split_block(found_level, level)) return NULL;

    mm_bdblock_t* block = _free_lists[level];
    _free_lists[level] = block->next;
    block->next = NULL;
    _mm_info.allocated += size;
    
    return (void*)((unsigned char*)block + sizeof(mm_bdblock_t));
}

static mm_bdblock_t* __get_buddy(mm_bdblock_t* block) {
    unsigned long offset = (unsigned long)((unsigned char*)block - _buffer);
    unsigned long buddy_offset = offset ^ block->size;
    return (mm_bdblock_t*)(_buffer + buddy_offset);
}

static int bd_free(void* ptr) {
    if (!ptr) return 0;
    if (ptr < (void*)_buffer || ptr >= (void*)(_buffer + ALLOC_BUFFER_SIZE)) return 0;
    
    mm_bdblock_t* block = (mm_bdblock_t*)((unsigned char*)ptr - sizeof(mm_bdblock_t));
    if (block->magic != BLOCK_MAGIC) return 0;

    unsigned int size = block->size;
    unsigned int user_size = size - sizeof(mm_bdblock_t);
    
    while (size < ALLOC_BUFFER_SIZE) {
        mm_bdblock_t* buddy = __get_buddy(block);
        
        if (buddy->magic != BLOCK_MAGIC || buddy->size != size) break;
        
        int level = __get_level(size);
        mm_bdblock_t* prev = NULL;
        mm_bdblock_t* curr = _free_lists[level];
        
        while (curr) {
            if (curr == buddy) {
                if (prev) prev->next = curr->next;
                else _free_lists[level] = curr->next;
                
                if (block > buddy) block = buddy;
                size <<= 1;
                block->size = size;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        
        if (!curr) break;
    }
    
    int level = __get_level(size);
    block->magic = BLOCK_MAGIC;
    block->size = size;
    block->next = _free_lists[level];
    _free_lists[level] = block;
    
    _mm_info.allocated -= user_size;
    return 1;
}

static int bd_info(mm_info_t* info) {
    if (!info) return 0;
    info->allocated = _mm_info.allocated;
    return 1;
}

#ifdef __cplusplus
}
#endif
#endif