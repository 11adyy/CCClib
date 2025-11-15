#include <set.h>
#include <assert.h>

int main() {
    set_t a, b, c;
    assert(set_init(&a));
    assert(set_init(&b));
    assert(set_init(&c));

    assert(set_size(&a) == 0);
    assert(set_size(&b) == 0);

    assert(set_add(&a, (void*)1));
    assert(set_add(&a, (void*)2));
    assert(set_add(&a, (void*)3));
    assert(!set_add(&a, (void*)2));
    assert(set_size(&a) == 3);

    assert(set_has(&a, (void*)1));
    assert(!set_has(&a, (void*)4));

    assert(set_add(&b, (void*)2));
    assert(set_add(&b, (void*)3));
    assert(set_add(&b, (void*)4));
    assert(set_size(&b) == 3);

    assert(set_remove(&a, (void*)2));
    assert(!set_has(&a, (void*)2));
    assert(set_size(&a) == 2);

    set_iter_t it;
    assert(set_iter_init(&a, &it));
    int count = 0;
    void* val;
    while (set_iter_next(&it, &val)) count++;
    assert(count == 2);

    assert(set_copy(&c, &a));
    assert(set_equal(&c, &a));

    set_t inter;
    assert(set_init(&inter));
    assert(set_intersect(&inter, &a, &b));
    assert(set_has(&inter, (void*)3));
    assert(!set_has(&inter, (void*)1));
    assert(!set_has(&inter, (void*)2));

    set_t uni;
    assert(set_init(&uni));
    assert(set_union(&uni, &a, &b));
    assert(set_has(&uni, (void*)1));
    assert(set_has(&uni, (void*)3));
    assert(set_has(&uni, (void*)4));

    set_t diff;
    assert(set_init(&diff));
    set_copy(&diff, &uni);
    assert(set_minus_set(&diff, &a));
    assert(!set_has(&diff, (void*)1));
    assert(!set_has(&diff, (void*)3));
    assert(set_has(&diff, (void*)4));

    assert(set_free(&a));
    assert(set_free(&b));
    assert(set_free(&c));
    assert(set_free(&inter));
    assert(set_free(&uni));
    assert(set_free(&diff));

    return 0;
}
