#ifndef BALLOC_H_
#define BALLOC_H_

#define NULL ((void*)0)
#define ALLOC_BUFFER_SIZE 131072
#define MIN_BLOCK         16
#define ALIGNMENT         8
#define MAX_LEVELS        14
#define MM_BLOCK_MAGIC    0xBADDCAFE

typedef struct mm_block {
    unsigned int magic;
    unsigned int size;
    struct mm_block* next;
} mm_block_t;

static unsigned char _buffer[ALLOC_BUFFER_SIZE];
static mm_block_t* _free_lists[MAX_LEVELS] = { NULL };

static unsigned int __next_power_of_two(unsigned int size) {
    unsigned int n = MIN_BLOCK;
    while (n < size) n <<= 1;
    return n;
}

static int __get_level(unsigned int size) {
    int level = 0;
    unsigned int s = MIN_BLOCK;
    while (s < size && level < MAX_LEVELS) {
        s <<= 1;
        level++;
    }

    return level;
}

int b_init() {
    for (int i = 0; i < MAX_LEVELS; ++i) _free_lists[i] = NULL;
    mm_block_t* block = (mm_block_t*)_buffer;
    block->magic = MM_BLOCK_MAGIC;
    block->size = ALLOC_BUFFER_SIZE;
    block->next = NULL;
    _free_lists[MAX_LEVELS - 1] = block;
    return 1;
}

static void __split_block(int from_level, int to_level) {
    while (from_level > to_level) {
        mm_block_t* block = _free_lists[from_level];
        if (!block) return;

        _free_lists[from_level] = block->next;

        unsigned int half_size = block->size / 2;
        mm_block_t* left = block;
        mm_block_t* right = (mm_block_t*)((unsigned char*)block + half_size);

        left->magic = MM_BLOCK_MAGIC;
        left->size = half_size;
        left->next = right;

        right->magic = MM_BLOCK_MAGIC;
        right->size = half_size;
        right->next = _free_lists[from_level - 1];

        _free_lists[from_level - 1] = left;
        from_level--;
    }
}

void* b_malloc(unsigned int size) {
    if (!size || size > ALLOC_BUFFER_SIZE) return NULL;
    size += sizeof(mm_block_t);
    if (size < MIN_BLOCK) size = MIN_BLOCK;

    size = __next_power_of_two(size);
    int level = __get_level(size);

    int found = -1;
    for (int i = level; i < MAX_LEVELS; ++i) {
        if (_free_lists[i]) {
            found = i;
            break;
        }
    }

    if (found == -1) return NULL;

    __split_block(found, level);

    mm_block_t* block = _free_lists[level];
    _free_lists[level] = block->next;
    block->next = NULL;
    return (void*)(block + 1);
}

static mm_block_t* __get_block(void* ptr) {
    return (mm_block_t*)((unsigned char*)ptr - sizeof(mm_block_t));
}

static void* __get_buddy(void* addr, unsigned int size) {
    unsigned long offset = (unsigned long)((unsigned char*)addr - _buffer);
    unsigned long buddy_offset = offset ^ size;
    return (void*)(_buffer + buddy_offset);
}

int b_free(void* ptr) {
    if (!ptr || ptr < (void*)_buffer || ptr >= (void*)(_buffer + ALLOC_BUFFER_SIZE))
        return 0;

    mm_block_t* block = __get_block(ptr);
    if (block->magic != MM_BLOCK_MAGIC) return 0;

    unsigned int size = block->size;
    int level = __get_level(size);

    void* addr = (void*)block;
    void* buddy;

    while (level < MAX_LEVELS - 1) {
        buddy = __get_buddy(addr, size);
        mm_block_t* prev = NULL;
        mm_block_t* curr = _free_lists[level];

        while (curr) {
            if ((void*)curr == buddy && curr->size == size) {
                if (prev) prev->next = curr->next;
                else _free_lists[level] = curr->next;
                addr = (addr < buddy) ? addr : buddy;
                size *= 2;
                level++;
                goto try_merge;
            }

            prev = curr;
            curr = curr->next;
        }

        break;

    try_merge:;
    }

    mm_block_t* new_block = (mm_block_t*)addr;
    new_block->magic = MM_BLOCK_MAGIC;
    new_block->size = size;
    new_block->next = _free_lists[level];
    _free_lists[level] = new_block;
    return 1;
}

#endif
