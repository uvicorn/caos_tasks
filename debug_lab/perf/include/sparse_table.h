#ifndef SPARSETABLE_H
#define SPARSETABLE_H

#include "utils.h"
#include "stddef.h"

typedef struct {
    Element* table;
    size_t size;
} SparseTable;


SparseTable* SparseTable_ctor(Element* array, size_t size);
SparseTable* SparseTable_FIX_ctor(Element* array, size_t size);

void SparseTable_dtor(SparseTable* table);

Element SparseTable_get_min_on_segment(SparseTable* sparse_table, int l, int r);
Element SparseTable_FIX_get_min_on_segment(SparseTable* sparse_table, int l, int r);

#endif