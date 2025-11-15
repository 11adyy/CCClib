#ifndef omap_H_
#define omap_H_
#ifdef __cplusplus
extern "C" {
#endif

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

#ifndef NULL
    #define NULL ((void*)0)
#endif

#ifndef MAP_INITIAL_CAPACITY
    #define MAP_INITIAL_CAPACITY 16
#endif

typedef struct omap_entry {
    long               key;
    void*              value;
    struct omap_entry* left;
    struct omap_entry* right;
    struct omap_entry* parent;
    int                color;
} omap_entry_t;

typedef struct {
    omap_entry_t* root;
    omap_entry_t* nil;
    size_t        size;
} omap_t;

typedef struct {
    omap_t*       map;
    omap_entry_t* current;
    int           started;
} omap_iter_t;

int omap_init(omap_t* map);
int omap_put(omap_t* map, long key, void* value);
int omap_get(omap_t* map, long key, void** value);
int omap_remove(omap_t* map, long key);
int omap_iter_init(omap_t* map, omap_iter_t* iter);
int omap_iter_next(omap_iter_t* iter, long* key, void** value);
size_t omap_size(omap_t* map);
int omap_is_empty(omap_t* map);
void omap_free(omap_t* map);

#ifdef __cplusplus
}
#endif
#endif