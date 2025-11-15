#ifndef SET_H_
#define SET_H_

#ifndef mm_malloc
    #include <stdlib.h>
    #define mm_malloc malloc
    #define mm_free   free
#endif

#ifndef str_memcpy
    #include <string.h>
    #define str_memset memset
    #define str_memcpy memcpy
#endif

#include <map.h>

typedef struct {
    map_t body;
} set_t;

typedef struct {
    map_iter_t it;
} set_iter_t;

int set_init(set_t* s);
int set_has(set_t* s, void* data);

int set_add(set_t* s, void* data);
int set_remove(set_t* s, void* data);

int set_iter_init(set_t* s, set_iter_t* it);
int set_iter_next(set_iter_t* it, void** d);

int set_union(set_t* dst, set_t* a, set_t* b);
int set_minus_set(set_t* trg, set_t* s);
int set_intersect(set_t* dst, set_t* a, set_t* b);

int set_copy(set_t* dst, set_t* src);
int set_equal(set_t* a, set_t* b);
int set_size(set_t* s);

int set_free(set_t* s);
int set_free_force(set_t* s);

#endif