#include <list.h>
#include <assert.h>

int main() {
    list_t lst;
    int rc = list_init(&lst);
    assert(rc == 1);
    assert(list_size(&lst) == 0);

    for (int i = 0; i < 10; i++) {
        rc = list_add(&lst, (void*)(long)i);
        assert(rc == 1);
    }
    assert(list_size(&lst) == 10);

    list_iter_t it;
    rc = list_iter_hinit(&lst, &it);
    assert(rc == 1);

    int expected = 0;
    void* data;
    while (list_iter_next(&it, &data)) {
        assert((long)data == expected);
        expected++;
    }
    assert(expected == 10);

    rc = list_iter_tinit(&lst, &it);
    assert(rc == 1);

    expected = 9;
    while (list_iter_prev(&it, &data)) {
        assert((long)data == expected);
        expected--;
    }
    assert(expected == -1);

    rc = list_remove(&lst, (void*)(long)5);
    assert(rc == 1);
    assert(list_size(&lst) == 9);

    assert((long)list_get_head(&lst) == 0);
    assert((long)list_get_tail(&lst) == 9);

    rc = list_free(&lst);
    assert(rc == 1);
    assert(list_size(&lst) == 0);
    return 0;
}
