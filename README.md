# CCClib

A collection of compact, self-contained C utilities designed to be copied into applications with minimal adaptation. The repository brings together three memory allocators, timing helpers, and basic map, set, and list containers. Each component can be used independently, so applications can include only the pieces they need.

## Allocators

### Linked-list allocator

Uses a singly linked list and first-fit search. Large free regions can be split; adjacent regions are combined when explicitly requested. Its small metadata cost comes with linear lookup and possible fragmentation.

### Binary-tree allocator

Recursively divides regions into halves until a request fits. Free regions can be combined when allocation needs additional space. The tree makes subdivisions explicit and can reduce fragmentation, at the cost of metadata and tree traversal.

### Buddy allocator

Keeps free blocks in size classes based on powers of two. Allocation splits a larger block as needed; freeing merges available buddies. This supports logarithmic operations and limits external fragmentation, with some internal waste from rounding sizes.

Each allocator is provided as a single header. The exact function names vary by implementation; the common interface has initialization, allocation, and deallocation operations.

```c
#include "balloc.h"

int main() {
    b_init();
    void* ptr = b_malloc(128);
    b_free(ptr);
}
```

## Timing helper

The timer utilities accumulate measurements and report the average duration.

```c
#include <timer.h>
int main() {
    ttimer_t optimer;
    reset_time_timer(&optimer);
    add_time_timer(MEASURE_TIME_US({
        int a = 10 - 11;
    }), &optimer);
    fprintf(stdout, "time=%.2f µs", get_avg_timer_and_reset(&optimer));
}
```

## Containers

The map associates integer keys with pointer values:

```c
#include <map.h>
int main() {
    map_t m;
    map_init(&m);
    map_put(&m, 1, (void*)100);

    int val;
    map_get(&m, 1, (void**)&val);

    map_iter_t it;
    map_iter_init(&m, &it);
    while (map_iter_next(&it, (void**)&val)) {

    }

    map_free(&m);
}
```

The set is built on the map interface:

```c
#include <set.h>
int main() {
    set_t s;
    set_init(&s);
    set_put(&s, (void*)1);
    if (!set_has(&s, (void*)1)) return 1;

    int val;
    set_iter_t it;
    set_iter_init(&s, &it);
    while (set_iter_next(&it, (void**)&val)) {

    }

    set_free(&s);
}
```

The list stores pointer values and supports iteration:

```c
#include <list.h>
int main() {
    list_t l;
    list_init(&l);

    list_add(&l, (void*)100);
    int val;
    list_iter_t it;
    list_iter_hinit(&l, &it);
    while (list_iter_next(&it, (void**)&val)) {

    }

    list_free(&l);
}
```

## Build and integration

The repository keeps allocator and container sources beside their public headers, with small Makefiles for component builds and examples:

```bash
make -C allocators
make -C structs
```

For direct integration, add the required header and implementation file to your application and compile it with the rest of your code. Check each header for the exact initialization and cleanup calls; the allocators have different policies and are not interchangeable in every detail.

## Choosing an allocator and ownership

The linked-list allocator favors a simple first-fit design and small metadata, with linear search cost and possible fragmentation. The binary-tree allocator explicitly tracks recursive subdivisions, at the cost of tree metadata and traversal. The buddy allocator rounds blocks to power-of-two classes and merges free buddies, improving coalescing behavior while potentially wasting some space inside rounded blocks.

The map uses integer keys and pointer values; the set is layered on the map interface, while the list stores pointer values. Ownership of values stored as `void *` remains with the caller unless an individual API says otherwise. Initialize each structure before use and release its internal storage when finished. Measure allocator behavior with your own workload before relying on speed or fragmentation expectations.
