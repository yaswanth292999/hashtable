// corrected_hashtable.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* --- Marker and item --- */
typedef enum { EMPTY = 0, OCCUPIED = 1, TOMBSTONE = 2 } marker;

typedef struct item {
    void *key;
    void *value;
    marker itemState;
} item;

/* --- function pointer typedefs --- */
typedef int   (*hash_fn_t)(void *key);                 // returns integer hash
typedef int   (*equals_fn_t)(void *a, void *b);        // nonzero if equal
typedef void  (*destroy_fn_t)(void *p);                // free / destroy callback

/* --- Hash table handle --- */
typedef struct HashTable {
    item *table;              // array of slots
    int capacity;             // current capacity
    int size;                 // number of OCCUPIED entries (does not count tombstones)
    float resizeThreshold;    // grow when loadFactor >= this
    float shrinkThreshold;    // shrink when loadFactor <= this
    /* callbacks */
    hash_fn_t hashFn;
    equals_fn_t equalsFn;
    destroy_fn_t destroyKeyFn;
    destroy_fn_t destroyValFn;
} HashTable;

/* --- defaults --- */
#define DEFAULT_CAPACITY 4
#define DEFAULT_GROW_THRESHOLD 0.7f
#define DEFAULT_SHRINK_THRESHOLD 0.25f

/* --- helpers --- */

/* safe modulo for possibly-negative hash */
static inline int idx_for_hash(unsigned long raw_hash, int capacity) {
    return (int)(raw_hash % (unsigned long)capacity);
}

/* default pointer-equality for keys when no equalsFn provided */
static int default_equals(void *a, void *b) {
    return a == b;
}

/* default pointer-hash when no hashFn provided:
   mix the pointer bits into an unsigned long */
static int default_ptr_hash(void *p) {
    uintptr_t v = (uintptr_t)p;
    /* a simple integer mixing (not cryptographic) */
    v = (~v) + (v << 21);
    v = v ^ (v >> 24);
    v = (v + (v << 3)) + (v << 8);
    v = v ^ (v >> 14);
    v = (v + (v << 2)) + (v << 4);
    v = v ^ (v >> 28);
    v = v + (v << 31);
    /* reduce to int */
    return (int)(v & 0x7fffffff);
}

/* compute load factor */
static float load_factor(HashTable *ht) {
    if (!ht || ht->capacity == 0) return 0.0f;
    return (float)ht->size / (float)ht->capacity;
}

/* create a new HashTable handle */
HashTable *createHashTable(int capacity,
                           hash_fn_t hashFn,
                           equals_fn_t equalsFn,
                           destroy_fn_t destroyKeyFn,
                           destroy_fn_t destroyValFn)
{
    if (capacity <= 0) capacity = DEFAULT_CAPACITY;

    HashTable *m = malloc(sizeof(HashTable));
    if (!m) return NULL;

    m->capacity = capacity;
    m->size = 0;
    m->resizeThreshold = DEFAULT_GROW_THRESHOLD;
    m->shrinkThreshold = DEFAULT_SHRINK_THRESHOLD;

    m->hashFn = hashFn ? hashFn : default_ptr_hash;
    m->equalsFn = equalsFn ? equalsFn : default_equals;
    m->destroyKeyFn = destroyKeyFn;
    m->destroyValFn = destroyValFn;

    m->table = calloc((size_t)m->capacity, sizeof(item));
    if (!m->table) { free(m); return NULL; }

    return m;
}

/* free the hash table (calls destroy callbacks for occupied entries) */
void destroyHashTable(HashTable *ht) {
    if (!ht) return;
    if (ht->table) {
        for (int i = 0; i < ht->capacity; i++) {
            if (ht->table[i].itemState == OCCUPIED) {
                if (ht->destroyKeyFn) ht->destroyKeyFn(ht->table[i].key);
                if (ht->destroyValFn) ht->destroyValFn(ht->table[i].value);
            }
        }
        free(ht->table);
    }
    free(ht);
}

/* internal: probe to find slot for a key (returns index):
   - if key exists -> returns index of existing key
   - else -> returns index where to insert (first tombstone if seen, else first empty)
   - if table is full (no slot available) -> returns -1
*/
static int getSlotLinearProbing_HT(HashTable *ht, void *key, item *table_override, int cap_override) {
    if (!ht && !table_override) return -1;

    item *table = table_override ? table_override : ht->table;
    int capacity = cap_override ? cap_override : ht->capacity;
    if (capacity <= 0) return -1;

    unsigned long raw = (unsigned long) ht->hashFn(key);
    int home = idx_for_hash(raw, capacity);

    int firstTombstone = -1;

    /* check home */
    marker st = table[home].itemState;
    if (st == EMPTY) return home;
    if (st == OCCUPIED) {
        if (ht->equalsFn(table[home].key, key)) return home;
    } else { /* TOMBSTONE */
        firstTombstone = home;
    }

    /* circular probe from home+1 back to home */
    int i = (home + 1) % capacity;
    while (i != home) {
        st = table[i].itemState;
        if (st == EMPTY) {
            return (firstTombstone != -1) ? firstTombstone : i;
        }
        if (st == OCCUPIED) {
            if (ht->equalsFn(table[i].key, key)) return i;
        } else { /* TOMBSTONE */
            if (firstTombstone == -1) firstTombstone = i;
        }
        i = (i + 1) % capacity;
    }

    /* full scan done; reuse tombstone if any */
    if (firstTombstone != -1) return firstTombstone;
    return -1;
}

/* wrapper to call getSlot using the table inside ht */
static int getSlotLinearProbing(HashTable *ht, void *key) {
    return getSlotLinearProbing_HT(ht, key, NULL, 0);
}

/* find index of existing key, or -1 if not found */
static int getIndexByKey(HashTable *ht, void *key) {
    if (!ht) return -1;
    int capacity = ht->capacity;
    if (capacity <= 0) return -1;

    unsigned long raw = (unsigned long) ht->hashFn(key);
    int home = idx_for_hash(raw, capacity);

    marker st = ht->table[home].itemState;
    if (st == EMPTY) return -1;
    if (st == OCCUPIED && ht->equalsFn(ht->table[home].key, key)) return home;

    int i = (home + 1) % capacity;
    while (i != home) {
        st = ht->table[i].itemState;
        if (st == EMPTY) return -1; /* probe chain ended */
        if (st == OCCUPIED && ht->equalsFn(ht->table[i].key, key)) return i;
        i = (i + 1) % capacity;
    }
    return -1;
}

/* resize helper: new_capacity must be >=1 */
static int resizeHashmap_internal(HashTable *ht, int new_capacity) {
    if (!ht || new_capacity <= 0) return -1;

    item *old = ht->table;
    int oldCap = ht->capacity;

    item *newTable = calloc((size_t)new_capacity, sizeof(item));
    if (!newTable) return -1;

    /* reinsert active entries into newTable */
    for (int i = 0; i < oldCap; i++) {
        if (old[i].itemState == OCCUPIED) {
            /* find slot in new table using its hash/equality but with override */
            /* temporarily use ht with newTable/capacity */
            /* We need a temporary HashTable-like context for hash/equal callbacks; we can reuse ht with overrides by passing table_override & cap_override. */
            /* But getSlotLinearProbing_HT above needs ht pointer for callbacks; so pass ht and overrides. */
            int newIdx = getSlotLinearProbing_HT(ht, old[i].key, newTable, new_capacity);
            if (newIdx == -1) {
                /* very unlikely unless new table full; cleanup and fail */
                free(newTable);
                return -1;
            }
            newTable[newIdx].key = old[i].key;
            newTable[newIdx].value = old[i].value;
            newTable[newIdx].itemState = OCCUPIED;
        }
        /* ignore tombstones and empty slots */
    }

    /* free old table memory (do not call destroy callbacks: ownership moved to new table) */
    free(old);
    ht->table = newTable;
    ht->capacity = new_capacity;
    /* size (number of OCCUPIED) remains the same */
    return 0;
}

/* public resize policy function */
static int maybe_resize_for_growth(HashTable *ht) {
    if (!ht) return -1;
    float lf = load_factor(ht);
    if (lf >= ht->resizeThreshold) {
        int newCap = ht->capacity * 2;
        if (newCap < 1) newCap = 1;
        return resizeHashmap_internal(ht, newCap);
    }
    return 0;
}

static int maybe_resize_for_shrink(HashTable *ht) {
    if (!ht) return -1;
    float lf = load_factor(ht);
    if (lf <= ht->shrinkThreshold && ht->capacity > DEFAULT_CAPACITY) {
        int newCap = ht->capacity / 2;
        if (newCap < DEFAULT_CAPACITY) newCap = DEFAULT_CAPACITY;
        return resizeHashmap_internal(ht, newCap);
    }
    return 0;
}

/* add or update key. returns index of inserted/updated slot, or -1 on error */
int addKey_HT(HashTable *ht, void *key, void *value) {
    if (!ht) return -1;

    /* resize if necessary before insert to preserve load factor */
    if (maybe_resize_for_growth(ht) != 0) {
        /* if resize failed, we still try to insert but might fail */
    }

    int idx = getSlotLinearProbing(ht, key);
    if (idx == -1) return -1;

    if (ht->table[idx].itemState == OCCUPIED) {
        /* update existing key -> replace value (call destroyVal if exists) */
        if (ht->destroyValFn) ht->destroyValFn(ht->table[idx].value);
        ht->table[idx].value = value;
        return idx;
    } else {
        /* inserting into empty or tombstone slot */
        ht->table[idx].key = key;
        ht->table[idx].value = value;
        ht->table[idx].itemState = OCCUPIED;
        ht->size++;
        return idx;
    }
}

/* remove key: returns value pointer if found (caller gets ownership of return),
   or NULL if not found. The table will mark tombstone and will not call destroyValFn.
   If you want table to free value automatically, call remove and ignore return (or implement otherwise).
*/
void *removeKey_HT(HashTable *ht, void *key) {
    if (!ht) return NULL;

    int idx = getIndexByKey(ht, key);
    if (idx == -1) return NULL;

    /* capture value to return */
    void *val = ht->table[idx].value;

    /* optionally destroy the key if table owns keys */
    if (ht->destroyKeyFn) ht->destroyKeyFn(ht->table[idx].key);

    /* mark tombstone */
    ht->table[idx].key = NULL;
    ht->table[idx].value = NULL;
    ht->table[idx].itemState = TOMBSTONE;
    ht->size--;

    /* maybe shrink */
    maybe_resize_for_shrink(ht);

    return val;
}

/* get value by key (returns pointer or NULL if not found) */
void *getValue_HT(HashTable *ht, void *key) {
    if (!ht) return NULL;
    int idx = getIndexByKey(ht, key);
    if (idx == -1) return NULL;
    return ht->table[idx].value;
}

/* --- Example string helpers (you can pass these into createHashTable) --- */

/* simple string hash (djb2) returning int */
int string_hash_wrapper(void *p) {
    const unsigned char *str = (const unsigned char *)p;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return (int)(hash & 0x7fffffff);
}

/* string equals wrapper */
int string_equals_wrapper(void *a, void *b) {
    if (!a || !b) return 0;
    return strcmp((const char *)a, (const char *)b) == 0;
}

/* --- End of implementation --- */

/* Optional: small smoke test (uncomment main to test) */

int main(void) {
    HashTable *ht = createHashTable(4, string_hash_wrapper, string_equals_wrapper, free, free);

    char *k1 = strdup("one");
    char *v1 = strdup("uno");
    addKey_HT(ht, k1, v1);

    char *k2 = strdup("two");
    char *v2 = strdup("dos");
    addKey_HT(ht, k2, v2);

    printf("size=%d, capacity=%d\n", ht->size, ht->capacity);

    char *g = getValue_HT(ht, "one");
    printf("got one -> %s\n", g ? g : "NULL");

    char *old = removeKey_HT(ht, "one");
    if (old) {
        printf("removed one -> %s\n", old);
        free(old); // caller owns returned value
    }

    destroyHashTable(ht);
    return 0;
}


