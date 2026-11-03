#include "sparse_table.h"
#include "string.h"
#include "stdlib.h"
#include <assert.h>


#define MIN(a, b) (a < b ? a : b)

void SparseTable_dtor(SparseTable* table){
    free(table->table);
    free(table);
}

SparseTable* SparseTable_ctor(Element* array, size_t size){
    assert(size > 2);

    SparseTable* sparse_table = malloc(sizeof(SparseTable));
    assert(sparse_table);

    unsigned logn = lg2(size - 1) + 1;
    Element* table = malloc(sizeof(Element) * size * logn);
    assert(table);
    memcpy(table, array, size * sizeof(Element));

    for (int i = 0; i < size; i++)
        table[i*logn + 0] = array[i];


    for (int l = 0; l < logn - 1; l++)
        for (int i = 0; i + (2 << l) <= size; i++)
            table[i*logn + l + 1] = MIN(table[i*logn + l], table[(i + (1 << l))*logn + l]);


    sparse_table->table = table;
    sparse_table->size  = logn;

    return sparse_table;
}

// min on [l, r)
Element SparseTable_get_min_on_segment(SparseTable* sparse_table, int l, int r){
    int t = lg2(r - l);

    Element res = MIN(
        sparse_table->table[t*sparse_table->size + l],
        sparse_table->table[t*sparse_table->size + r - (1 << t)]
    );
    return res;
}

// ==========================================================
//  YOUR FIX VERSION
// ==========================================================

SparseTable* SparseTable_FIX_ctor(Element* array, size_t size){
    return SparseTable_ctor(array, size);
}

// min on [l, r)
Element SparseTable_FIX_get_min_on_segment(SparseTable* sparse_table, int l, int r){
    return SparseTable_get_min_on_segment(sparse_table, l, r);
}