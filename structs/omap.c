#include <omap.h>

static void _left_rotate(omap_t* map, omap_entry_t* x) {
    omap_entry_t* y = x->right;
    x->right = y->left;
    if (y->left != map->nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;
    if (x->parent == map->nil) map->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    
    y->left = x;
    x->parent = y;
}

static void _right_rotate(omap_t* map, omap_entry_t* y) {
    omap_entry_t* x = y->left;
    y->left = x->right;
    if (x->right != map->nil) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == map->nil) map->root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    
    x->right = y;
    y->parent = x;
}

static void _insert_fixup(omap_t* map, omap_entry_t* z) {
    while (z->parent->color == 1) {
        if (z->parent == z->parent->parent->left) {
            omap_entry_t* y = z->parent->parent->right;
            if (y->color == 1) {
                z->parent->color = 0;
                y->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            } 
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    _left_rotate(map, z);
                }

                z->parent->color = 0;
                z->parent->parent->color = 1;
                _right_rotate(map, z->parent->parent);
            }
        } 
        else {
            omap_entry_t* y = z->parent->parent->left;
            if (y->color == 1) {
                z->parent->color = 0;
                y->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            } 
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    _right_rotate(map, z);
                }

                z->parent->color = 0;
                z->parent->parent->color = 1;
                _left_rotate(map, z->parent->parent);
            }
        }
    }

    map->root->color = 0;
}

static omap_entry_t* _minimum(omap_t* map, omap_entry_t* x) {
    while (x->left != map->nil) x = x->left;
    return x;
}

static void _transplant(omap_t* map, omap_entry_t* u, omap_entry_t* v) {
    if (u->parent == map->nil) map->root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

static void _delete_fixup(omap_t* map, omap_entry_t* x) {
    while (x != map->root && !x->color) {
        if (x == x->parent->left) {
            omap_entry_t* w = x->parent->right;
            if (w->color == 1) {
                w->color = 0;
                x->parent->color = 1;
                _left_rotate(map, x->parent);
                w = x->parent->right;
            }
            
            if (w->left->color == 0 && w->right->color == 0) {
                w->color = 1;
                x = x->parent;
            } 
            else {
                if (w->right->color == 0) {
                    w->left->color = 0;
                    w->color = 1;
                    _right_rotate(map, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                _left_rotate(map, x->parent);
                x = map->root;
            }
        } 
        else {
            omap_entry_t* w = x->parent->left;
            if (w->color == 1) {
                w->color = 0;
                x->parent->color = 1;
                _right_rotate(map, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == 0 && w->left->color == 0) {
                w->color = 1;
                x = x->parent;
            } 
            else {
                if (w->left->color == 0) {
                    w->right->color = 0;
                    w->color = 1;
                    _left_rotate(map, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                _right_rotate(map, x->parent);
                x = map->root;
            }
        }
    }
    x->color = 0;
}

int omap_init(omap_t* map) {
    if (!map) return 0;
    
    map->nil = (omap_entry_t*)mm_malloc(sizeof(omap_entry_t));
    if (!map->nil) return 0;
    
    map->nil->color = 0;
    map->nil->left = map->nil->right = map->nil->parent = map->nil;
    map->root = map->nil;
    map->size = 0;
    return 1;
}

int omap_put(omap_t* map, long key, void* value) {
    if (!map) return 0;

    omap_entry_t* z = (omap_entry_t*)mm_malloc(sizeof(omap_entry_t));
    if (!z) return 0;

    z->key = key;
    z->value = value;
    z->left = z->right = z->parent = map->nil;
    z->color = 1;

    omap_entry_t* y = map->nil;
    omap_entry_t* x = map->root;

    while (x != map->nil) {
        y = x;
        if (z->key < x->key) x = x->left;
        else if (z->key > x->key) x = x->right;
        else {
            x->value = value;
            mm_free(z);
            return 1;
        }
    }

    z->parent = y;
    if (y == map->nil) map->root = z;
    else if (z->key < y->key) y->left = z;
    else y->right = z;

    _insert_fixup(map, z);
    map->size++;
    return 1;
}

int omap_get(omap_t* map, long key, void** value) {
    if (!map || !value) return 0;
    omap_entry_t* x = map->root;
    while (x != map->nil) {
        if (key < x->key) x = x->left;
        else if (key > x->key) x = x->right;
        else {
            *value = x->value;
            return 1;
        }
    }

    return 0;
}

int omap_remove(omap_t* map, long key) {
    if (!map) return 0;

    omap_entry_t* z = map->root;
    while (z != map->nil) {
        if (key < z->key) z = z->left;
        else if (key > z->key) z = z->right;
        else break;
    }

    if (z == map->nil) return 0;
    omap_entry_t* y = z;
    omap_entry_t* x;
    int y_original_color = y->color;

    if (z->left == map->nil) {
        x = z->right;
        _transplant(map, z, z->right);
    } 
    else if (z->right == map->nil) {
        x = z->left;
        _transplant(map, z, z->left);
    } 
    else {
        y = _minimum(map, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) x->parent = y;
        else {
            _transplant(map, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        _transplant(map, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (!y_original_color) {
        _delete_fixup(map, x);
    }

    mm_free(z);
    map->size--;
    return 1;
}

int omap_iter_init(omap_t* map, omap_iter_t* iter) {
    if (!map || !iter) return 0;
    iter->map = map;
    iter->current = map->nil;
    iter->started = 0;
    return 1;
}

int omap_iter_next(omap_iter_t* iter, long* key, void** value) {
    if (!iter || !key || !value) return 0;

    if (!iter->started) {
        iter->current = _minimum(iter->map, iter->map->root);
        iter->started = 1;
    } 
    else {
        omap_entry_t* x = iter->current;
        if (x->right != iter->map->nil) {
            iter->current = _minimum(iter->map, x->right);
        } else {
            omap_entry_t* y = x->parent;
            while (y != iter->map->nil && x == y->right) {
                x = y;
                y = y->parent;
            }
            iter->current = y;
        }
    }

    if (iter->current == iter->map->nil) return 0;
    *key = iter->current->key;
    *value = iter->current->value;
    return 1;
}

size_t omap_size(omap_t* map) {
    return map ? map->size : 0;
}

int omap_is_empty(omap_t* map) {
    return !map || map->size == 0;
}

void omap_free(omap_t* map) {
    if (!map) return;
    omap_entry_t* stack[MAP_INITIAL_CAPACITY];
    int top = -1;
    
    if (map->root != map->nil) {
        stack[++top] = map->root;
    }

    while (top >= 0) {
        omap_entry_t* node = stack[top--];
        if (node->left != map->nil) stack[++top] = node->left;
        if (node->right != map->nil) stack[++top] = node->right;
        mm_free(node);
    }

    mm_free(map->nil);
    map->root = NULL;
    map->nil = NULL;
    map->size = 0;
}
