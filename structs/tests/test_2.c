#include <map.h>
#include <assert.h>

int main() {
    map_t m;
    assert(map_init(&m));

    assert(map_isempty(&m));

    assert(map_put(&m, 1, (void*)100));
    assert(map_put(&m, 2, (void*)200));
    assert(map_put(&m, 3, (void*)300));
    assert(!map_isempty(&m));
    assert(m.size == 3);

    void* val;
    assert(map_get(&m, 1, &val) && val == (void*)100);
    assert(map_get(&m, 2, &val) && val == (void*)200);
    assert(map_get(&m, 3, &val) && val == (void*)300);
    assert(!map_get(&m, 4, &val));

    assert(map_put(&m, 2, (void*)250));
    assert(map_get(&m, 2, &val) && val == (void*)250);

    assert(map_remove(&m, 2));
    assert(m.size == 2);
    assert(!map_get(&m, 2, &val));
    assert(!map_remove(&m, 2));

    map_iter_t it;
    assert(map_iter_init(&m, &it));
    int count = 0;
    while (map_iter_next(&it, &val)) count++;
    assert(count == 2);

    map_t copy;
    assert(map_copy(&copy, &m));
    assert(map_equals(&copy, &m));

    assert(map_free(&m));
    assert(map_free(&copy));

    return 0;
}
